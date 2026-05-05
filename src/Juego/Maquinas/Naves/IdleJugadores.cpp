#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "SaltarJugador.hpp"

namespace IVJ
{
    IdleJugadores::IdleJugadores()
        :FSM{},sprite{nullptr},s_w{0},s_h{0}
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
        //si presionamos cualquier dirección cambiarnos
        //al nodo de MoverJugadores(frames,frame_rate)
        if(control.abj || control.der || control.izq)
            return new MoverJugadores(4,0.1f);
        return nullptr;
    }
    void IdleJugadores::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
    }
    void IdleJugadores::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void IdleJugadores::onUpdate(const Entidad& obj,float dt)
    {
        (void)obj;(void)dt;
        sprite->setTextureRect(
                sf::IntRect{
                    {//posicion frame 4 al de idle
                        s_w*0,0
                    },
                    {//tamaño
                        s_w,s_h
                    }
                });
    }
}