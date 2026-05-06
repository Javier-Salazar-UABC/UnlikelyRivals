#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "SaltarJugador.hpp"
#include "CorrerJugador.hpp"
#include "GolpearJugador.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

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
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_idle"));
        id_frame=0;
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