#include "GolpearJugador.hpp"
#include "PatearJugador.hpp"
#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "SaltarJugador.hpp"
#include "../../Componentes/IJComponentes.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "CorrerJugador.hpp"

namespace IVJ
{
    GolpearJugador::GolpearJugador(int max_frames, float frame_rate)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},
        max_tiempo{frame_rate},
        act_tiempo{0.f},      // Empezar en 0 para que el primer frame sea instantáneo
        max_frames{max_frames},
        hitbox_activa{false},
        golpe_procesado{false}
    {
        nombre = "GolpearJugador";
    }
    FSM* GolpearJugador::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        // COMBO: Solo permitimos el combo si el botón fue soltado y presionado de nuevo
        if (!control.acc) {
            esperando_liberacion = false;
        }

        if (id_frame >= 3 && control.acc && !esperando_liberacion) {
            return new PatearJugador(4, 0.1f);
        }

        // Solo podemos salir del estado si la animacion terminó
        if (id_frame >= max_frames) {
            // Primero checamos si saltó (tecla Arriba) para iniciar un nuevo salto con fuerza
            if (control.arr) {
                if (obj.tieneComponente<IGravedad>() && obj.getComponente<IGravedad>()->saltos_restantes > 0) {
                    return new SaltarJugador(true);
                }
            }
            // Luego verificamos si está en el aire (cayendo) para volver a la animación de salto
            if (obj.tieneComponente<IGravedad>() && !obj.getComponente<IGravedad>()->en_suelo) {
                return new SaltarJugador(false);
            }
            // Si está en el suelo:
            if(control.abj || control.der || control.izq) {
                if (control.run) return new CorrerJugador(6, 0.08f);
                return new MoverJugadores(4, 0.1f);
            }
            return new IdleJugadores(4, 0.15f);
        }
        return nullptr;
    }
    void GolpearJugador::onEntrar(const Entidad& obj)
    {
        sprite = &obj.getComponente<CE::ISprite>()->m_sprite;
        s_w = obj.getComponente<CE::ISprite>()->width;
        s_h = obj.getComponente<CE::ISprite>()->height;
        sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_punch"));
        id_frame = 0;
        hitbox_activa = false;
        golpe_procesado = false;
        esperando_liberacion = true; // Ignorar el primer acc hasta que se suelte
    }
    void GolpearJugador::onSalir(const Entidad& obj)
    {
        (void)obj;
        hitbox_activa = false;
        golpe_procesado = false;
    }
    void GolpearJugador::onUpdate(const Entidad& obj,float dt)
    {
        act_tiempo = act_tiempo - dt;
        
        // El golpe se hace activo en el frame 3 (indice 2)
        if (id_frame == 2) {
            hitbox_activa = true;
        } else {
            hitbox_activa = false;
            golpe_procesado = false; // Reset if frame changes
        }

        if (act_tiempo <= 0)
        {
            if (id_frame < max_frames) {
                sprite->setTextureRect(sf::IntRect({ s_w * id_frame, 0 }, { s_w, s_h }));
                id_frame++;
            }
            act_tiempo = max_tiempo;
        }
    }
}
