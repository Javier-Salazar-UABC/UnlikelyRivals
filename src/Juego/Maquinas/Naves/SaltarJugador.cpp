#include "SaltarJugador.hpp"
#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "../../Componentes/IJComponentes.hpp"

namespace IVJ
{
    SaltarJugador::SaltarJugador()
        :FSM{},sprite{nullptr},s_w{0},s_h{0},salto_soltado{false}
    {
        nombre = "SaltarJugador";
    }
    FSM* SaltarJugador::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        // Si tocamos el suelo, salimos del estado de salto
        if (obj.tieneComponente<IGravedad>()) {
            if (obj.getComponente<IGravedad>()->en_suelo) {
                if (control.der || control.izq || control.abj)
                    return new MoverJugadores(4, 0.1f);
                else
                    return new IdleJugadores();
            }
        }
        
        // Logica para doble salto limpio (requiere soltar la tecla primero)
        if (!control.arr) {
            salto_soltado = true;
        } else if (salto_soltado) {
            if (obj.tieneComponente<IGravedad>() && obj.getComponente<IGravedad>()->saltos_restantes > 0) {
                return new SaltarJugador(); // Reinicia el estado de salto
            }
        }

        return nullptr;
    }
    void SaltarJugador::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;

        if (obj.tieneComponente<IGravedad>()) {
            auto grav = obj.getComponente<IGravedad>();
            if (grav->saltos_restantes > 0) {
                grav->velocidad_Y = -350.f; // Fuerza del salto
                grav->saltos_restantes--;
                grav->en_suelo = false; 
            }
        }
    }
    void SaltarJugador::onSalir(const Entidad& obj)
    {
        (void)obj;
    }
    void SaltarJugador::onUpdate(const Entidad& obj,float dt)
    {
        (void)obj;(void)dt;
        // Mantener el espejeo en el aire si se mueve
        auto control = obj.getComponente<CE::IControl>();
        float esc = obj.getComponente<CE::ISprite>()->escala;
        if (control->der) {
            sprite->setScale(sf::Vector2f{esc, esc});
        }
        else if (control->izq) {
            sprite->setScale(sf::Vector2f{-esc, esc});
        }

        // Mostrar un frame estático para el salto (ej. frame 2 de la textura)
        sprite->setTextureRect(
                sf::IntRect{
                    { s_w*2, 0 },
                    { s_w, s_h }
                });
    }
}
