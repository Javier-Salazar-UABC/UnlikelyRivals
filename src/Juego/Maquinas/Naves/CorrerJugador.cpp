#include "CorrerJugador.hpp"
#include "IdleJugadores.hpp"
#include "SaltarJugador.hpp"
#include "MoverJugadores.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

namespace IVJ
{
    CorrerJugador::CorrerJugador(int max_frames,float frame_rate)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},
        max_tiempo{frame_rate},
        act_tiempo{frame_rate},
        max_frames{max_frames}
    {
        nombre = "CorrerJugador";
    }
    FSM* CorrerJugador::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        if (control.arr) {
            if (obj.tieneComponente<IGravedad>() && obj.getComponente<IGravedad>()->saltos_restantes > 0) {
                return new SaltarJugador();
            }
        }
        //si nada esta presionado regresar a idle
        if(!control.abj && !control.der && !control.izq)
            return new IdleJugadores(4, 0.15f);
        
        // si dejamos de correr
        if(!control.run)
            return new MoverJugadores(4, 0.1f);

        return nullptr;
    }
    void CorrerJugador::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_run"));
        id_frame=0;
    }
    void CorrerJugador::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void CorrerJugador::onUpdate(const Entidad& obj,float dt)
    {
        (void)obj;
        auto control = obj.getComponente<CE::IControl>();
        float esc = obj.getComponente<CE::ISprite>()->escala;

        if (control->der) {
            sprite->setScale(sf::Vector2f{esc, esc});
        }
        else if (control->izq) {
            sprite->setScale(sf::Vector2f{-esc, esc});
        }
        act_tiempo = act_tiempo - dt;
        if (act_tiempo <=0)
        {
            sprite->setTextureRect(
                    sf::IntRect{
                        {
                            s_w*(id_frame%max_frames),0
                        },
                        {
                            s_w,s_h
                        }
                    });
            id_frame++;
            act_tiempo=max_tiempo;
        }
    }
}
