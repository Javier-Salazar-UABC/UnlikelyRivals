#include "SaltarJugador.hpp"
#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "../../Componentes/IJComponentes.hpp"
#include "CorrerJugador.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "GolpearJugador.hpp"

namespace IVJ
{
    SaltarJugador::SaltarJugador(bool aplicar_fuerza)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},salto_soltado{false},
        act_tiempo{0.1f}, id_frame{0}, aterrizando{false}, tiempo_aterrizaje{0.1f},
        aplicar_fuerza{aplicar_fuerza}
    {
        nombre = "SaltarJugador";
    }
    FSM* SaltarJugador::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        // Si tocamos el suelo y ya terminó la animación de aterrizaje, salimos del estado
        if (aterrizando && tiempo_aterrizaje <= 0.0f) {
            if (control.der || control.izq || control.abj) {
                if (control.run) return new CorrerJugador(6, 0.08f);
                return new MoverJugadores(4, 0.1f);
            }
            else
                return new IdleJugadores(4, 0.15f);
        }
        
        // Logica para doble salto limpio (requiere soltar la tecla primero)
        if (!control.arr) {
            salto_soltado = true;
        } else if (salto_soltado && !aterrizando) {
            if (obj.tieneComponente<IGravedad>() && obj.getComponente<IGravedad>()->saltos_restantes > 0) {
                return new SaltarJugador(); // Reinicia el estado de salto
            }
        }
        
        if (control.acc) {
            return new GolpearJugador(4, 0.1f);
        }

        return nullptr;
    }
    void SaltarJugador::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        const std::string& clave = obj.tieneComponente<IAnimaciones>()
            ? obj.getComponente<IAnimaciones>()->get("jump", "esnupi_jump")
            : "esnupi_jump";
        sprite->setTexture(CE::GestorAssets::Get().getTextura(clave));


        if (aplicar_fuerza && obj.tieneComponente<IGravedad>()) {
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

        auto grav = obj.getComponente<IGravedad>();

        if (grav && grav->en_suelo) {
            if (!aterrizando) {
                aterrizando = true;
                id_frame = 4; // Frame 5 (aterrizando)
            }
            tiempo_aterrizaje -= dt;
        } else if (grav && grav->velocidad_Y >= 0) {
            // Cayendo -> Frame 4 (index 3)
            id_frame = 3;
        } else {
            // Subiendo -> Animar frames 0, 1, 2
            act_tiempo -= dt;
            if (act_tiempo <= 0) {
                if (id_frame < 2) {
                    id_frame++;
                }
                act_tiempo = 0.1f;
            }
        }

        sprite->setTextureRect(
                sf::IntRect{
                    { s_w * id_frame, 0 },
                    { s_w, s_h }
                });
    }
}
