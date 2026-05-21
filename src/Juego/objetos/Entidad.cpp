#include "Entidad.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include <memory>

#include "Juego/Maquinas/Bosses/FSMBoss.hpp"

namespace IVJ
{
    void Entidad::onUpdate(float dt) 
    {
        //CE::Objeto::onUpdate(dt);

        //revisar si tiene ciertos componentes
        //para actualizarlos debidamente
        //o hacer una clase hija y ahí revisar

        if(tieneComponente<ITriangulo>())
        {
            auto fig = getComponente<ITriangulo>();
            auto pos = getTransformada()->posicion;
            //dibujamos el triángulo en la posición correcta
            fig->tri_shape.setPosition({pos.x,pos.y});
            //aplicamos rotación dependiendo de la dirección
            auto r = sf::radians(fig->angulo);
            fig->tri_shape.setRotation(r);
        }

        if(tieneComponente<CE::ISprite>())
        {
            auto sprite = getComponente<CE::ISprite>();
            auto pos = getTransformada()->posicion;
            sprite->m_sprite.setPosition({pos.x,pos.y});
            //auto angulo = sf::radians(getTransformada()->angulo);
            //sprite->m_sprite.setRotation(angulo);
        }
        //if(tieneComponente<CE::ISprite>() && tieneComponente<CE::IBoundingBox>())
        //{
        //    auto sprite = getComponente<CE::ISprite>();
        //    auto bound = getComponente<CE::IBoundingBox>();
        //    //sprite->m_sprite.setOrigin({bound->mitad.x,bound->mitad.y});
        //}

        // si tiene un FSM actualizar el estado
        if(tieneComponente<IVJ::IMaquinaEstado>())
        {
            auto mq = getComponente<IVJ::IMaquinaEstado>();
            if(mq->fsm)
                mq->fsm->onUpdate(*this,dt);
        }
        transform->pos_prev =  transform->posicion;

    }

    static int num_cpy = 0;
    Entidad::Entidad(const Entidad &cpy)
        : CE::Objeto{cpy}
    {
        nombre->nombre = cpy.getNombre()->nombre + "_" + std::to_string(++num_cpy);
        //copia profunda de los componentes
        //ya que no queremos la referencia si no un componente nuevo
        for (auto &comp : cpy.componentes)
        {
            componentes.push_back(comp->clonar());
        }
    }

    void Entidad::inputFSM()
    {
        if(!getComponente<IMaquinaEstado>()
        || !getComponente<IMaquinaEstado>()->fsm)
            return;
        auto mq = getComponente<IMaquinaEstado>();
        //Lab 8 Animaciones: Pequeño facade usando el nombre del nodo
        if(mq->fsm->getNombre().find("Boss")!=std::string::npos)
        {
            auto target = getComponente<ITarget>()->pos;
            //usar el nodo correcto de boss
            auto n_estado = std::static_pointer_cast<FSMBoss>(mq->fsm)->onInputs(*this,*target);
            std::shared_ptr<FSM> estado(n_estado);
            if(estado)
                setFSM(estado);
        }else
        {
            if(!getComponente<CE::IControl>())  return;
            auto control = *getComponente<CE::IControl>();

            // Si está colgado del borde, no permitir atacar
            if (tieneComponente<IGravedad>()) {
                if (getComponente<IGravedad>()->colgado_borde) {
                    control.acc = false;
                    control.sacc = false;
                }
            }

            auto n_estado = mq->fsm->onInputs(*this, control);
            std::shared_ptr<FSM> estado(n_estado);
            if(estado)
                setFSM(estado);
        }
    }

    
    void Entidad::setFSM(const std::shared_ptr<FSM>& mq)
    {
        if (getComponente<IMaquinaEstado>()->congelar)
            return;
        auto &estado_actual = getComponente<IMaquinaEstado>()->fsm;
        //transición de salida
        estado_actual->onSalir(*this);
        //transición de entrar a otro estado
        estado_actual = mq;
        estado_actual->onEntrar(*this);
    }


    void Entidad::draw(sf::RenderTarget &target, sf::RenderStates state) const
    {
        state.transform *= getTransform();
#if DEBUG
        if(tieneComponente<CE::IBoundingBox>())
        {
            auto escala = 1;//getComponente<CE::ISprite>()->escala;
            auto bound = getComponente<CE::IBoundingBox>();
            sf::RectangleShape box{{bound->tam.x*escala,bound->tam.y*escala}};
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(sf::Color::Red);
            box.setOutlineThickness(1.5f);
            box.setOrigin({bound->mitad.x*escala,bound->mitad.y*escala});
            auto pos = transform->posicion;
            box.setPosition({pos.x,pos.y});
            target.draw(box);
        }
#endif

        if(tieneComponente<ITriangulo>())
        {
            auto fig = getComponente<ITriangulo>();
            target.draw(fig->tri_shape);
        }

        if(tieneComponente<CE::ISprite>())
        {
            auto sprite = getComponente<CE::ISprite>();
            
            if(tieneComponente<CE::INormalMap>())
            {
                auto normalComponent = getComponente<CE::INormalMap>();
                const sf::Texture* diffuseTex = &sprite->m_sprite.getTexture();
                sf::Texture* normalTex = normalComponent->getNormalTexture(diffuseTex);
                
                if (normalTex)
                {
                    auto& shader = normalComponent->m_shader;
                    shader.setUniform("normalMap", *normalTex);
                    
                    if (diffuseTex) {
                        sf::Vector2u size = diffuseTex->getSize();
                        shader.setUniform("textureSize", sf::Vector2f(size.x, size.y));
                    }
                    
                    sf::IntRect rect = sprite->m_sprite.getTextureRect();
                    shader.setUniform("textureRect", sf::Glsl::Vec4(rect.position.x, rect.position.y, rect.size.x, rect.size.y));
                    
                    auto pos = getTransformada()->posicion;
                    shader.setUniform("spritePos", sf::Vector2f(pos.x, pos.y));
                    
                    sf::Vector2f origin = sprite->m_sprite.getOrigin();
                    shader.setUniform("spriteOrigin", origin);
                    
                    sf::Vector2f scale = sprite->m_sprite.getScale();
                    shader.setUniform("spriteScale", scale);
                    
                    shader.setUniform("lightPos", normalComponent->lightPos);
                    shader.setUniform("lightColor", normalComponent->lightColor);
                    shader.setUniform("ambientColor", normalComponent->ambientColor);
                    shader.setUniform("lightFalloff", normalComponent->lightFalloff);
                    shader.setUniform("lightHeight", normalComponent->lightHeight);
                    
                    target.draw(sprite->m_sprite, &shader);
                }
                else
                {
                    target.draw(sprite->m_sprite);
                }
            }
            else if(tieneComponente<CE::IShader>())
            {
                target.draw(sprite->m_sprite,&getComponente<CE::IShader>()->m_shader);
            }
            else
                target.draw(sprite->m_sprite);
        }
    }

}
