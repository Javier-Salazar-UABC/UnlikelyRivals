#include "PatearJugador.hpp"
#include "IdleJugadores.hpp"
#include "MoverJugadores.hpp"
#include "SaltarJugador.hpp"
#include "../../Componentes/IJComponentes.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "../../Sistemas/Sistemas.hpp"
#include "CorrerJugador.hpp"

namespace IVJ
{
    PatearJugador::PatearJugador(int max_frames, float frame_rate)
        :FSM{},sprite{nullptr},s_w{0},s_h{0},
        max_tiempo{frame_rate},
        act_tiempo{0.f},      // Empezar en 0 para que el primer frame sea instantáneo
        max_frames{max_frames},
        hitbox_activa{false},
        golpe_procesado{false},
        esperando_liberacion{false}
    {
        nombre = "PatearJugador";
    }
    FSM* PatearJugador::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        // Evitar que siga atacando si el botón queda presionado
        if (!control.acc) {
            esperando_liberacion = false;
        }

        if (id_frame >= max_frames) {
            if (control.arr) {
                if (obj.tieneComponente<IGravedad>() && obj.getComponente<IGravedad>()->saltos_restantes > 0) {
                    return new SaltarJugador(true);
                }
            }
            if (obj.tieneComponente<IGravedad>() && !obj.getComponente<IGravedad>()->en_suelo) {
                return new SaltarJugador(false);
            }
            if(control.abj || control.der || control.izq) {
                if (control.run) return new CorrerJugador(6, 0.08f);
                return new MoverJugadores(4, 0.1f);
            }
            return new IdleJugadores(4, 0.15f);
        }
        return nullptr;
    }
    void PatearJugador::onEntrar(const Entidad& obj)
    {
        auto compSprite = obj.getComponente<CE::ISprite>();
        sprite = &compSprite->m_sprite;

        // Leer la configuración del componente IAnimaciones
        if (obj.tieneComponente<IAnimaciones>()) {
            if (auto* data = obj.getComponente<IAnimaciones>()->get("kick")) {
                sprite->setTexture(CE::GestorAssets::Get().getTextura(data->clave_textura));
                max_frames = data->frames;
                max_tiempo = data->frame_rate; // <-- NUEVO
                if (data->width > 0)  compSprite->width = data->width;
                if (data->height > 0) compSprite->height = data->height;
            }
        } else {
            // Fallback
            sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_kick"));
            max_frames = 4;
        }

        s_w = compSprite->width;
        s_h = compSprite->height;
        sprite->setOrigin({s_w / 2.f, s_h / 2.f});
        sprite->setTextureRect(sf::IntRect({0, 0}, {s_w, s_h}));
        id_frame = 0;
        hitbox_activa = false;
        golpe_procesado = false;
        esperando_liberacion = true; // Ignorar el acc actual hasta que se suelte

        // Reproducir sonido de ataque
        ReproducirSonidoAleatorio("swing", 5);
    }
    void PatearJugador::onSalir(const Entidad& obj)
    {
        (void)obj;
        hitbox_activa = false;
        golpe_procesado = false;
    }
    void PatearJugador::onUpdate(const Entidad& obj,float dt)
    {
        act_tiempo = act_tiempo - dt;
        
        // La patada es activa en el frame 3 (indice 2) de 4
        if (id_frame == 2) {
            hitbox_activa = true;
        } else {
            hitbox_activa = false;
            golpe_procesado = false;
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
