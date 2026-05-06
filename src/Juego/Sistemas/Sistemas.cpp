#include "Sistemas.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include "../../Motor/Primitivos/GestorAssets.hpp"
#include "../objetos/Entidad.hpp"
#include <math.h>
#include "../../Motor/Utils/Lerp.hpp"
#include "../../Motor/Render/Render.hpp"
#include <memory>


namespace IVJ
{
    void SistemaControl(CE::Objeto&ente, float dt)
    {
        //no hay necesidad verificar si tiene, se asume que tiene
        //control y transformada
        auto p = ente.getTransformada();
        auto c = ente.getComponente<CE::IControl>();
        (void)dt;
        if(c->arr)
            p->velocidad.y=-200;

        if(c->abj)
            p->velocidad.y=200;
        if(c->der)
            p->velocidad.x=200;
        if(c->izq)
            p->velocidad.x=-200;
        if(c->run)
            p->velocidad.escala(2.f);

    }

    void SistemaMover(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt)
    {

        for(auto& ente : entes)
        {
            //todo ente tiene ITransform por lo que no requiere verificación
            auto trans = ente->getTransformada();
            trans->posicion.suma(trans->velocidad.escala(dt));

        }

    }
    void SistemaMover(const std::shared_ptr<CE::Objeto>& objeto, float dt)
    {
        auto trans = objeto->getTransformada();
        auto control = objeto->getComponente<CE::IControl>();

        //Si no tiene control o el control no esta activo salir
        if(!control || !control->isActivo()) return;

        //definimos cual va ser la velocidad correcta dependiendo
        //de que boton del control este presionado
        auto vel = CE::Vector2D{0.f,0.f};
        if(control->der) vel.x = trans->velocidad.x;
        if(control->izq) vel.x = -trans->velocidad.x;

        if(control->run) vel.x *= 1.6f;

        //calculamos la dirección usando un triángulo rectángulo
        if(objeto->getComponente<ITriangulo>() || objeto->getComponente<CE::ISprite>())
        {
            if (vel.x!=0 || vel.y!=0) {
                auto n = vel;
                n.normalizacion();
                auto trian = objeto->getComponente<ITriangulo>(); //nulo

                if (trian)
                    trian->angulo = std::atan2(n.x,-n.y);
                trans->angulo = std::atan2(n.x,-n.y);
            }
        }
        //actualizamos la posición del ente.
        trans->posicion.suma(vel.escala(dt));
    }

    bool SistemaColAABB(CE::Objeto& A, CE::Objeto& B, bool resolucion)
    {
        if(!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
            return false;
        auto mA = A.getComponente<CE::IBoundingBox>()->mitad;
        auto *pa = &A.getTransformada()->posicion;
        auto prevA = A.getTransformada()->pos_prev;
        auto mB = B.getComponente<CE::IBoundingBox>()->mitad;
        auto pb = B.getTransformada()->posicion;
        float dX = std::abs(pb.x - pa->x);
        float dY = std::abs(pb.y - pa->y);
        float sumMidX = mA.x + mB.x;
        float sumMidY = mA.y + mB.y;
        
        float penX = sumMidX - dX;
        float penY = sumMidY - dY;
        bool hay_colision = penX > 0 && penY > 0;

        if(hay_colision && resolucion) {
            if (penX < penY) {
                pa->x = prevA.x;
            } else {
                if (A.tieneComponente<IGravedad>() && pa->y > prevA.y) {
                    auto grav = A.getComponente<IGravedad>();
                    grav->velocidad_Y = 0.0f;
                    grav->en_suelo = true;
                    grav->saltos_restantes = 2;
                }
                pa->y = prevA.y;
            }
        }
        return hay_colision;
    }

    bool SistemaColAABBMid(CE::Objeto& A, CE::Objeto& B, bool resolucion)
    {
        if(!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
            return false;
        auto midA = A.getComponente<CE::IBoundingBox>()->mitad;
        auto *pa = &A.getTransformada()->posicion;
        auto prevA = A.getTransformada()->pos_prev;
        auto midB = B.getComponente<CE::IBoundingBox>()->mitad;
        auto *pb = &B.getTransformada()->posicion;
        float dX = std::abs(pb->x - pa->x);
        float dY = std::abs(pb->y - pa->y);
        float sumMidX = midA.x + midB.x;
        float sumMidY = midA.y + midB.y;
        
        float penX = sumMidX - dX;
        float penY = sumMidY - dY;
        bool hay_colision = penX > 0 && penY > 0;
        
        if (resolucion && hay_colision) {
            if (penX < penY) { 
                pa->x = prevA.x;
            } else { 
                if (A.tieneComponente<IGravedad>() && pa->y > prevA.y) {
                    auto grav = A.getComponente<IGravedad>();
                    grav->velocidad_Y = 0.0f;
                    grav->en_suelo = true;
                    grav->saltos_restantes = 2;
                }
                pa->y = prevA.y;
            }
        }
        return hay_colision;
    }

    bool checkRayoHit(CE::Objeto& npc, CE::Vector2D& p1, CE::Vector2D& p2)
    {
        if(!npc.tieneComponente<CE::IBoundingBox>()) return false;
        
        auto midnpc = npc.getComponente<CE::IBoundingBox>()->mitad;
        auto posnpc = npc.getTransformada()->posicion;
        auto linea = (p2-p1);//.escala(1.f);
        //std::cout<<"Mid linea: "<<midlinea<<"\n";
        float dx = std::abs(posnpc.x - p1.x);
        float dy = std::abs(posnpc.y - p1.y);
        float sumMidX = midnpc.x + std::abs(linea.x) ;
        float sumMidY = midnpc.y + std::abs(linea.y) ;
        
        return (sumMidX - dx >0) && (sumMidY - dy >0);
        
    }

    void SistemaSpawn(CE::Pool& spawns)
    {
        for(auto &s : spawns.getPool())
        {
            auto respawndata = s->getComponente<CE::IRespawn>();
            if(!respawndata || respawndata->curr_spawn >= respawndata->max_spawn)
                continue;
            //respawn
            if(respawndata->timer_actual >= respawndata->timer_maximo)
            {
                //crear objeto
                auto malillo = std::make_shared<Entidad>(); //(*(respawndata->prefab));
                //auto comp = respawndata->prefab->getListaComponentes();
                //auto comp = respawndata->prefab->getComponente<CE::ISprite>();
                //for(auto &c : comp)
                //{
                    //malillo->copyComponente<CE::ISprite>(comp);
                    //malillo->copyComponente<CE::IPaths>(respawndata->prefab->getComponente<CE::IPaths>());
                //}
                 
                malillo->addComponente(std::make_shared<CE::IPaths>(60))
                .addComponente(std::make_shared<CE::ISprite>(
                            CE::GestorAssets::Get().getTextura("hoja_blue"),68,91,1.f))
                .addComponente(std::make_shared<CE::IShader>("",ASSETS "/shaders/contorno.frag"));
                malillo->getComponente<CE::IPaths>()->addCurva(
                        CE::Vector2D{500.f,300.f},CE::Vector2D{600.f,500.f},CE::Vector2D{800.f,300.f});
                malillo->getComponente<CE::IPaths>()->addCurva(
                        CE::Vector2D{800.f,300.f},CE::Vector2D{600.f,100.f},CE::Vector2D{500.f,300.f});


                //mandar datos al shader
                auto shader = malillo->getComponente<CE::IShader>();
                static sf::Texture textura=malillo->getComponente<CE::ISprite>()->m_sprite.getTexture();
                
                static sf::Glsl::Vec4 uOutlineColor{1.f,0.f,1.f,1.f};
                //static  float uEdgeThreshold= 0.15f; //0.15-0.35
                //static  float uEdgeSoftness = 0.02f;
                static  float uThickness = 0.15f;
                auto w = malillo->getComponente<CE::ISprite>()->width;
                auto h = malillo->getComponente<CE::ISprite>()->height;
                static sf::Glsl::Vec2 uTexSize{(float)w,(float)h};

                shader->setTextura("uTex",&textura);
                shader->setVector4("uOutlineColor",&uOutlineColor);
                //shader->setEscalar("uEdgeThreshold",&uEdgeThreshold);
                //shader->setEscalar("uEdgeSoftness",&uEdgeSoftness);
                shader->setEscalar("uThickness",&uThickness);
                shader->setVector2("uTexSize",&uTexSize);

                //posiciones
                malillo->getStats()->hp =100;
                //asumimos que spwan tine un cuadro , tomamos el cuadro como el area
                int size_x = respawndata->width; 
                int size_y = respawndata->height;
                float x0 = s->getTransformada()->posicion.x-size_x/2.f;
                float y0 = s->getTransformada()->posicion.y-size_y/2.f;
                float x = x0+(rand()%size_x);
                float y = y0+(rand()%size_y);
                malillo->setPosicion(x,y);
                //spawns.push_back(malillo);
                spawns.agregarPool(malillo);
                respawndata->timer_actual=0;
                respawndata->curr_spawn++;
            }
            respawndata->timer_actual++;
        }
    }
    void SistemaPaths(std::vector<std::shared_ptr<CE::Objeto>>& obj)
    {
        for(auto &o: obj)
        {
            if(o->tieneComponente<CE::IPaths>())
            {
                auto path = o->getComponente<CE::IPaths>();
                int num_curvas = path->puntos.size()/3;
                if(path->puntos.size()==0) continue;
                if(path->id_curva >= num_curvas) continue;
                float t = (path->frame_actual_curva%(path->frame_total_curva+1))/(float)path->frame_total_curva;
                //std::cout<<o<<" t: "<<t<<"\n";
                CE::Vector2D P0 = path->puntos[0+path->id_curva*(path->offset-0)];
                CE::Vector2D P1 = path->puntos[1+path->id_curva*(path->offset-0)];
                CE::Vector2D P2 = path->puntos[2+path->id_curva*(path->offset-0)];
                auto posiciones_actual = CE::lerp2(P0, P1, P2, t);
                path->frame_actual_curva++;
                o->setPosicion(posiciones_actual.x,posiciones_actual.y);
                if(t == 1.f )
                    path->id_curva++;
                    //path->frame_actual_curva=0;
                //std::cout<<"[ "<<o<<" ] "<<path->frame_actual_curva<<"\n";
                if(path->id_curva == 2 && o->tieneComponente<CE::IShader>())
                {
                    //cambiar shader
                    auto shader = o->getComponente<CE::IShader>();
                    shader->cambiarShader("",ASSETS "/shaders/color.frag");
                    static sf::Glsl::Vec4 color{0.f,1.f,0.f,1.f};
                    shader->m_vars.clear();
                    shader->setVector4("color",&color);
                }
                else
                {
                    //cambiar shader
                    auto shader = o->getComponente<CE::IShader>();
                    shader->cambiarShader("",ASSETS "/shaders/contorno.frag");
                    static sf::Glsl::Vec4 outlinecolor{0.f,1.f,0.f,1.f};
                    static float thickness = 0.1;
                    static sf::Glsl::Vec2 size{63,92};
                    shader->m_vars.clear();
                    shader->setVector4("uOutlineColor",&outlinecolor);
                    shader->setEscalar("uThickness",&thickness);
                    shader->setVector2("uTexSize",&size);
                    
                }
            }
        }
    }

    //Dialogos
    void SistemaDialogos(IVJ::IDialogo* dialogo,CE::Objeto& obj)
    {
        if(!dialogo) return;
        dialogo->activo = true;
        dialogo->onInteractuar(obj);
    }

    void SistemaGirar(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IGirar>()) return;
        auto componente = ente.getComponente<IGirar>();
        auto pos = ente.getTransformada()->posicion;
        float radio = componente->radio;
        float x = radio *cos(componente->angulo);
        float y = radio *sin(componente->angulo);
        ente.setPosicion(pos.x+x,pos.y+y);
        componente->angulo+= 3.146f*dt;
    }

    void SistemaArribaAbajo(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IArribaAbajo>()) return;
        auto componente = ente.getComponente<IArribaAbajo>();
        auto pos = ente.getTransformada()->posicion;
        float dist = componente->distancia;
        float y = dist *sin(componente->angulo);
        ente.setPosicion(pos.x,pos.y+y);
        componente -> angulo += 3.146f*dt;
    }

    void SistemaOnda(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IOnda>()) return;
        auto componente = ente.getComponente<IOnda>();
        auto pos = ente.getTransformada()->posicion;

        float amp = componente->amplitud;
        float frc = componente->frecuencia;
        float y = amp *sin(componente->angulo);

        ente.setPosicion(pos.x+frc,pos.y+y);
        componente -> angulo += 3.146f*dt;
    }

    void SistemaGravedad(const std::shared_ptr<CE::Objeto>& objeto, float dt)
    {
        if(!objeto->tieneComponente<IGravedad>()) return;
        auto gravedad = objeto->getComponente<IGravedad>();
        auto transform = objeto->getTransformada();
        if(!transform) return;

        gravedad->en_suelo = false;

        gravedad->velocidad_Y += gravedad->fuerza * dt;
        if (gravedad->velocidad_Y > gravedad->max_vel)
        {
            gravedad->velocidad_Y = gravedad->max_vel;
        }
        transform->posicion.y += gravedad->velocidad_Y * dt;
    }

    void SistemaGravedad(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt)
    {
        for(auto& ente : entes)
        {
            SistemaGravedad(ente, dt);
        }
    }


    void SistemaEpidemia(const MatrizEntes& matriz, float dt)
    {
        if (matriz.empty()) return;
        int filas = matriz.size();
        int columnas = matriz[0].size();

        // Direcciones: [Arriba, Abajo, Izquierda, Derecha]
        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};

        for (int y = 0; y < filas; ++y)
        {
            for (int x = 0; x < columnas; ++x)
            {
                auto ente = matriz[y][x];
                if (!ente) continue;
                auto salud = ente->getComponente<IEstadoSalud>();
                if (!salud) continue;

                salud->estado_siguiente = salud->estado_actual;
                salud->tiempo_en_estado += dt;

                // 1. LÓGICA: NORMAL
                if (salud->estado_actual == TipoSalud::Normal) {
                    // Descontar timer de inmunidad parcial si la tiene
                    if (salud->inmunidad_parcial_timer > 0.0f) {
                        salud->inmunidad_parcial_timer -= dt;
                    }
                }
                // 2. LÓGICA: ENFERMO
                else if (salud->estado_actual == TipoSalud::Enfermo) {
                    salud->timer_infeccion += dt;

                    // Cada 1 segundo, intenta contagiar a sus vecinos
                    if (salud->timer_infeccion >= 1.0f) {
                        salud->timer_infeccion -= 1.0f; // Reinicia el tick

                        for (int i = 0; i < 4; ++i) {
                            int ny = y + dy[i];
                            int nx = x + dx[i];

                            if (ny >= 0 && ny < filas && nx >= 0 && nx < columnas) {
                                auto vecino = matriz[ny][nx];
                                if (vecino) {
                                    auto s_vecino = vecino->getComponente<IEstadoSalud>();
                                    // Solo puede infectar si el vecino es Normal y NO tiene inmunidad parcial
                                    if (s_vecino && s_vecino->estado_actual == TipoSalud::Normal && s_vecino->estado_siguiente == TipoSalud::Normal) {
                                        if (s_vecino->inmunidad_parcial_timer <= 0.0f) {
                                            // Tira los dados contra la probabilidad individual del vecino
                                            if ((rand() % 100) < s_vecino->prob_contagio) {
                                                s_vecino->estado_siguiente = TipoSalud::Enfermo;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // A los 3 segundos exactos, pasa a recuperación
                    if (salud->tiempo_en_estado >= 3.0f) {
                        salud->estado_siguiente = TipoSalud::Recuperacion;
                    }
                }
                // 3. LÓGICA: RECUPERACIÓN
                else if (salud->estado_actual == TipoSalud::Recuperacion) {
                    // A los 2 segundos exactos, se decide su destino
                    if (salud->tiempo_en_estado >= 2.0f) {
                        if ((rand() % 100) < 10) {
                            // 10% de probabilidad de morir
                            salud->estado_siguiente = TipoSalud::Muerto;
                        } else {
                            // Si sobrevive, checa si obtiene inmunidad permanente
                            if ((rand() % 100) < salud->prob_inmunidad) {
                                salud->estado_siguiente = TipoSalud::Inmune;
                            } else {
                                // Si no, vuelve a Normal pero con inmunidad de 2 segundos
                                salud->estado_siguiente = TipoSalud::Normal;
                                salud->inmunidad_parcial_timer = 2.0f;
                            }
                        }
                    }
                }
            }
        }
    }

    void SistemaActualizarSalud(const MatrizEntes& matriz)
    {
        for (const auto& fila : matriz) {
            for (const auto& ente : fila) {
                if (!ente) continue;
                auto salud = ente->getComponente<IEstadoSalud>();
                auto visual = ente->getComponente<IVisualSalud>();

                if (salud && visual) {
                    if (salud->estado_actual != salud->estado_siguiente) {
                        salud->estado_actual = salud->estado_siguiente;

                        salud->tiempo_en_estado = 0.0f;
                        salud->timer_infeccion = 0.0f;

                        if (visual->figura) {
                            visual->figura->setRelleno(visual->colores[salud->estado_actual]);
                        }
                    }
                }
            }
        }
    }

    bool revisarDistanciaInteraccion(CE::Objeto& jugador, CE::Objeto& npc, float distancia_maxima)
    {
        auto pos_jugador = jugador.getTransformada()->posicion;
        auto pos_npc = npc.getTransformada()->posicion;
        float distancia = pos_jugador.distancia(pos_npc);
        return distancia <= distancia_maxima;
    }

    void SistemaNPCLookTarget(CE::Objeto &ente, CE::Vector2D& target)
    {
        if(!ente.tieneComponente<IRangoAggro>()) return;

        auto rango = ente.getComponente<IRangoAggro>();
        if (!rango->estaDentroRango(ente,target)) return;

        auto pos = ente.getTransformada()->posicion;
        auto ente_a_target =  target-pos;
        ente.getTransformada()->angulo = atan2(
                ente_a_target.y,
                ente_a_target.x
            );
        //reflejar imagen
        if (ente_a_target.x <0)
            ente.getComponente<CE::ISprite>()->m_sprite.setScale({1,-1});
        else
            ente.getComponente<CE::ISprite>()->m_sprite.setScale({1,1});
    }

    //DEBUG
    void pintarLinea(CE::Vector2D& p1, CE::Vector2D& p2, const sf::Color& color)
    {
        sf::CircleShape cp1{2.f};
        cp1.setFillColor({0,0,0,255});
        sf::CircleShape cp2{2.f};
        cp2.setFillColor({0,0,0,255});
        cp1.setPosition({p1.x,p1.y});
        cp2.setPosition({p2.x,p2.y});
        //pintar lineas de guía
        for(float t=0;t<=1;t+=0.0015)
        {
            sf::CircleShape pixel{1};
            pixel.setFillColor(color);
            auto puntopos  = CE::lerp(p1,p2,t);
            pixel.setPosition({puntopos.x,puntopos.y});
            CE::Render::Get().AddToDraw(pixel);
        }
        CE::Render::Get().AddToDraw(cp1);
        CE::Render::Get().AddToDraw(cp2);
    }

}

