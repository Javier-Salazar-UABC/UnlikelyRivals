#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "SaltarJugador.hpp"
#include "CorrerJugador.hpp"
#include "GolpearJugador.hpp"
#include "DownBJugador.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "../../Componentes/IJComponentes.hpp"

namespace IVJ
{
    IdleJugadores::IdleJugadores(int max_frames,float frame_rate)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},
        max_tiempo{frame_rate},
        act_tiempo{frame_rate},
        max_frames{max_frames}
    {
        nombre = "IdleJugador";
    }
    FSM* IdleJugadores::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        if (control.abj && control.sacc) {
            if (obj.tieneComponente<IAnimaciones>() && obj.getComponente<IAnimaciones>()->get("down_b")) {
                return new DownBJugador();
            }
        }
        if (control.arr) {
            if (obj.tieneComponente<IGravedad>() && obj.getComponente<IGravedad>()->saltos_restantes > 0) {
                return new SaltarJugador();
            }
        }
        if (control.acc) {
            return new GolpearJugador(4, 0.1f); // 4 frames
        }
        //si presionamos cualquier dirección cambiarnos
        //al nodo de MoverJugadores(frames,frame_rate)
        if(control.abj || control.der || control.izq) {
            if(control.run)
                return new CorrerJugador(6, 0.08f);
            return new MoverJugadores(4,0.1f);
        }
        return nullptr;
    }
    void IdleJugadores::onEntrar(const Entidad& obj)
    {
        auto compSprite = obj.getComponente<CE::ISprite>();
        sprite = &compSprite->m_sprite;
        
        // Leer la configuración del componente IAnimaciones
        if (obj.tieneComponente<IAnimaciones>()) {
            if (auto* data = obj.getComponente<IAnimaciones>()->get("idle")) {
                sprite->setTexture(CE::GestorAssets::Get().getTextura(data->clave_textura));
                max_frames = data->frames;
                max_tiempo = data->frame_rate; // <-- NUEVO
                if (data->width > 0)  compSprite->width = data->width;
                if (data->height > 0) compSprite->height = data->height;
            }
        } else {
            // Fallback
            sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_idle"));
            max_frames = 4;
        }

        s_w = compSprite->width;
        s_h = compSprite->height;
        sprite->setOrigin({s_w / 2.f, s_h / 2.f});
        sprite->setTextureRect(sf::IntRect({0, 0}, {s_w, s_h}));
        id_frame = 0;
    }
    void IdleJugadores::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void IdleJugadores::onUpdate(const Entidad& obj,float dt)
    {
        (void)obj;(void)dt;
        act_tiempo = act_tiempo - dt;
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