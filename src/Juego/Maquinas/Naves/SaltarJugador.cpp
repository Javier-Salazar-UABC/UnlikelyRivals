#include "SaltarJugador.hpp"
#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "DownBJugador.hpp"
#include "../../Componentes/IJComponentes.hpp"
#include "../../Sistemas/Sistemas.hpp"
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
        if (control.abj && control.sacc) {
            if (obj.tieneComponente<IAnimaciones>() && obj.getComponente<IAnimaciones>()->get("down_b")) {
                return new DownBJugador();
            }
        }
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
        auto compSprite = obj.getComponente<CE::ISprite>();
        sprite = &compSprite->m_sprite;

        // Leer la configuración del componente IAnimaciones
        if (obj.tieneComponente<IAnimaciones>()) {
            if (auto* data = obj.getComponente<IAnimaciones>()->get("jump")) {
                sprite->setTexture(CE::GestorAssets::Get().getTextura(data->clave_textura));
                // Nota: SaltarJugador tiene lógica interna para frames, pero actualizamos el tamaño y velocidad
                act_tiempo = data->frame_rate; // <-- NUEVO
                if (data->width > 0)  compSprite->width = data->width;
                if (data->height > 0) compSprite->height = data->height;
            }
        } else {
            // Fallback
            sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_jump"));
        }

        s_w = compSprite->width;
        s_h = compSprite->height;
        sprite->setOrigin({s_w / 2.f, s_h / 2.f});
        sprite->setTextureRect(sf::IntRect({0, 0}, {s_w, s_h}));


        if (aplicar_fuerza && obj.tieneComponente<IGravedad>()) {
            auto grav = obj.getComponente<IGravedad>();
            if (grav->saltos_restantes > 0) {
                grav->velocidad_Y = -350.f; // Fuerza del salto
                grav->saltos_restantes--;
                grav->en_suelo = false; 

                // Reproducir sonido de salto
                ReproducirSonidoAleatorio("jump", 5, false);
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
