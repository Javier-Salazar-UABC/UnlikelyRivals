#include "DownBJugador.hpp"
#include "IdleJugadores.hpp"
#include "SaltarJugador.hpp"
#include "../../Componentes/IJComponentes.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "../../Sistemas/Sistemas.hpp"

namespace IVJ
{
    DownBJugador::DownBJugador()
        : FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
          max_tiempo{0.1f}, act_tiempo{0.0f}, max_frames{6}, id_frame{0},
          en_aire{false}, descenso_rapido{false}, impacto_suelo_reproducido{false},
          hitbox_activa{false}, golpe_procesado{false}
    {
        nombre = "DownBJugador";
    }

    void DownBJugador::onEntrar(const Entidad& obj)
    {
        auto compSprite = obj.getComponente<CE::ISprite>();
        sprite = &compSprite->m_sprite;

        // Leer la configuración del componente IAnimaciones
        if (obj.tieneComponente<IAnimaciones>()) {
            if (auto* data = obj.getComponente<IAnimaciones>()->get("down_b")) {
                sprite->setTexture(CE::GestorAssets::Get().getTextura(data->clave_textura));
                max_frames = data->frames;
                max_tiempo = data->frame_rate;
                if (data->width > 0)  compSprite->width = data->width;
                if (data->height > 0) compSprite->height = data->height;
            }
        } else {
            // Fallback
            sprite->setTexture(CE::GestorAssets::Get().getTextura("esnupi_down_b"));
            max_frames = 6;
            compSprite->width = 40;
            compSprite->height = 40;
        }

        s_w = compSprite->width;
        s_h = compSprite->height;
        sprite->setOrigin({s_w / 2.f, s_h / 2.f});
        sprite->setTextureRect(sf::IntRect({0, 0}, {s_w, s_h}));
        id_frame = 0;
        act_tiempo = 0.0f; // Primer frame instantáneo
        hitbox_activa = false;
        golpe_procesado = false;
        impacto_suelo_reproducido = false;
        en_aire = false;
        descenso_rapido = false;

        if (obj.tieneComponente<IGravedad>()) {
            auto grav = obj.getComponente<IGravedad>();
            if (!grav->en_suelo) {
                en_aire = true;
                descenso_rapido = true;
                grav->velocidad_Y = 1000.f; // Descend rápido instantáneo
            }
        }

        // Desactivar temporalmente el control para evitar movimiento horizontal
        if (auto control = obj.getComponente<CE::IControl>()) {
            control->setActivo(false);
        }

        // Sonido inicial
        ReproducirSonidoAleatorio("swing", 5);
    }

    void DownBJugador::onSalir(const Entidad& obj)
    {
        // Reactivar control al salir
        if (auto control = obj.getComponente<CE::IControl>()) {
            control->setActivo(true);
        }
        hitbox_activa = false;
        golpe_procesado = false;
    }

    void DownBJugador::onUpdate(const Entidad& obj, float dt)
    {
        auto transform = obj.getTransformada();
        auto grav = obj.getComponente<IGravedad>();

        if (descenso_rapido) {
            // Congelar velocidad horizontal y forzar descenso rápido
            if (transform) {
                transform->velocidad.x = 0.0f;
            }
            if (grav) {
                grav->velocidad_Y = 1000.f;
                grav->en_suelo = false;
            }

            // Animar de 0 a 3 y congelar en 3
            act_tiempo -= dt;
            if (act_tiempo <= 0.0f) {
                if (id_frame < 3) {
                    id_frame++;
                }
                act_tiempo = max_tiempo;
            }

            // Si toca el suelo
            if (grav && grav->en_suelo) {
                descenso_rapido = false;
                id_frame = 4; // Impacto
                hitbox_activa = true;
                ReproducirSonidoAleatorio("melee", 11, false); // Sonido de impacto
                impacto_suelo_reproducido = true;
                act_tiempo = max_tiempo;
            }
        } else {
            // Animación normal en tierra (o post-impacto)
            if (transform) {
                transform->velocidad.x = 0.0f;
            }
            if (grav && grav->en_suelo) {
                grav->velocidad_Y = 0.0f; // Mantener pegado al suelo
            }

            act_tiempo -= dt;
            if (act_tiempo <= 0.0f) {
                id_frame++;
                act_tiempo = max_tiempo;
            }

            // Hitbox activa en el frame 4
            if (id_frame == 4) {
                hitbox_activa = true;
                if (!impacto_suelo_reproducido) {
                    ReproducirSonidoAleatorio("melee", 11, false);
                    impacto_suelo_reproducido = true;
                }
            } else {
                hitbox_activa = false;
            }
        }

        if (id_frame < max_frames) {
            sprite->setTextureRect(sf::IntRect({ s_w * id_frame, 0 }, { s_w, s_h }));
        }
    }

    FSM* DownBJugador::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        (void)control;
        if (id_frame >= max_frames) {
            if (obj.tieneComponente<IGravedad>() && !obj.getComponente<IGravedad>()->en_suelo) {
                return new SaltarJugador(false);
            }
            return new IdleJugadores(4, 0.15f);
        }
        return nullptr;
    }
}
