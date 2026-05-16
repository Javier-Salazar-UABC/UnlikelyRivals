#include "MoverJugadores.hpp"
#include "IdleJugadores.hpp"
#include "SaltarJugador.hpp"
#include "CorrerJugador.hpp"
#include "GolpearJugador.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "../../Componentes/IJComponentes.hpp"
namespace IVJ
{
    MoverJugadores::MoverJugadores(int max_frames,float frame_rate)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},
        max_tiempo{frame_rate},
        act_tiempo{frame_rate},
        max_frames{max_frames}
    {
        nombre = "MoverJugador";
    }
    FSM* MoverJugadores::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        if (control.arr) {
            if (obj.tieneComponente<IGravedad>() && obj.getComponente<IGravedad>()->saltos_restantes > 0) {
                return new SaltarJugador();
            }
        }
        if (control.acc) {
            return new GolpearJugador(4, 0.1f);
        }
        //si nada esta presionado regresar a idle
        if(!control.abj && !control.der && !control.izq)
            return new IdleJugadores(4, 0.15f);

        // Si presionamos correr
        if (control.run)
            return new CorrerJugador(6, 0.08f);

        return nullptr;
    }
    void MoverJugadores::onEntrar(const Entidad& obj)
    {
        auto compSprite = obj.getComponente<CE::ISprite>();
        sprite = &compSprite->m_sprite;

        // Leer la configuración del componente IAnimaciones
        if (obj.tieneComponente<IAnimaciones>()) {
            if (auto* data = obj.getComponente<IAnimaciones>()->get("walk")) {
                sprite->setTexture(CE::GestorAssets::Get().getTextura(data->clave_textura));
                max_frames = data->frames;
                max_tiempo = data->frame_rate; // <-- NUEVO
                if (data->width > 0)  compSprite->width = data->width;
                if (data->height > 0) compSprite->height = data->height;
            }
        } else {
            // Fallback
            sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_walk"));
            max_frames = 4;
        }

        s_w = compSprite->width;
        s_h = compSprite->height;
        sprite->setOrigin({s_w / 2.f, s_h / 2.f});
        sprite->setTextureRect(sf::IntRect({0, 0}, {s_w, s_h}));
        id_frame = 0;
    }
    void MoverJugadores::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void MoverJugadores::onUpdate(const Entidad& obj,float dt)
    {
        (void)obj; //quitar el warning de no usar obj
        // === NUEVA LÓGICA DE ESPEJEO (FLIP) ===
        auto control = obj.getComponente<CE::IControl>();
        float esc = obj.getComponente<CE::ISprite>()->escala;

        // Pasamos los valores adentro de un sf::Vector2f para que la función setScale los acepte
        if (control->der) {
            sprite->setScale(sf::Vector2f{esc, esc});
        }
        else if (control->izq) {
            sprite->setScale(sf::Vector2f{-esc, esc});
        }
        act_tiempo = act_tiempo - dt;
        //frame rate
        if (act_tiempo <=0)
        {
            //frame mostrando
            sprite->setTextureRect(
                    sf::IntRect{
                        {//posicion id*width = frame actual
                            s_w*(id_frame%max_frames),0
                        },
                        {//tamaño
                            s_w,s_h
                        }
                    });
            id_frame++;
            act_tiempo=max_tiempo;
        }
    }
}