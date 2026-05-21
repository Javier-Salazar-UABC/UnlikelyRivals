#include "RecibirGolpe.hpp"
#include "IdleJugadores.hpp"
#include "SaltarJugador.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "../../Componentes/IJComponentes.hpp"
#include <cmath>

namespace IVJ
{
    RecibirGolpe::RecibirGolpe()
        : FSM{}, sprite{nullptr}, s_w{0}, s_h{0},
          max_tiempo{0.1f}, act_tiempo{0.1f}, max_frames{1}
    {
        nombre = "RecibirGolpe";
    }

    FSM* RecibirGolpe::onInputs(const Entidad& obj, const CE::IControl& control)
    {
        (void)control;
        auto stats = obj.getStats();
        
        // Mientras haya hitstun, no se puede hacer nada
        if (stats->hitstun_timer > 0) {
            return nullptr;
        }

        // Una vez terminado el hitstun, volver a idle o salto (sin aplicar un nuevo impulso de salto)
        if (obj.tieneComponente<IGravedad>() && !obj.getComponente<IGravedad>()->en_suelo) {
            return new SaltarJugador(false);
        }
        return new IdleJugadores(4, 0.1f);
    }

    void RecibirGolpe::onEntrar(const Entidad& obj)
    {
        auto compSprite = obj.getComponente<CE::ISprite>();
        sprite = &compSprite->m_sprite;

        if (obj.tieneComponente<IAnimaciones>()) {
            if (auto* data = obj.getComponente<IAnimaciones>()->get("hit")) {
                sprite->setTexture(CE::GestorAssets::Get().getTextura(data->clave_textura));
                max_frames = data->frames;
                max_tiempo = data->frame_rate;
                if (data->width > 0)  compSprite->width = data->width;
                if (data->height > 0) compSprite->height = data->height;
            }
        }

        s_w = compSprite->width;
        s_h = compSprite->height;
        sprite->setOrigin({s_w / 2.f, s_h / 2.f});
        sprite->setTextureRect(sf::IntRect({0, 0}, {s_w, s_h}));
        id_frame = 0;
    }

    void RecibirGolpe::onSalir(const Entidad& obj)
    {
        (void)obj;
    }

    void RecibirGolpe::onUpdate(const Entidad& obj, float dt)
    {
        auto stats = obj.getStats();
        if (stats->hitstun_timer > 0) {
            stats->hitstun_timer -= dt;
        }

        // La velocidad decae para que el vuelo no sea infinito a velocidad constante
        auto& entity_mutable = const_cast<Entidad&>(obj);
        auto trans = entity_mutable.getTransformada();
        if (trans) {
            // Directional Influence (DI estilo Smash Bros): 
            // Permite influir levemente la trayectoria de vuelo al mantener izquierda/derecha durante el hitstun
            auto control = obj.getComponente<CE::IControl>();
            if (control && control->isActivo() && stats->hitstun_timer > 0.0f) {
                if (control->izq) {
                    trans->velocidad.x -= 250.0f * dt; // Deriva hacia la izquierda
                } else if (control->der) {
                    trans->velocidad.x += 250.0f * dt; // Deriva hacia la derecha
                }
            }

            // Aplicar fricción al movimiento horizontal (0.99 es más suave que 0.96)
            trans->velocidad.x *= std::pow(0.99f, dt * 60.0f);
        }

        // --- RECUPERACIÓN EN SUELO ---
        // Si toca el suelo, el hitstun se reduce un poco más rápido
        if (obj.tieneComponente<IGravedad>()) {
            auto grav = obj.getComponente<IGravedad>();
            if (grav->en_suelo && stats->hitstun_timer > 0) {
                stats->hitstun_timer -= dt * 2.0f; // Se recupera 2 veces más rápido (antes 4x, era mucho)
            }
        }

        act_tiempo -= dt;
        if (act_tiempo <= 0) {
            sprite->setTextureRect(sf::IntRect({ s_w * (id_frame % max_frames), 0 }, { s_w, s_h }));
            id_frame++;
            act_tiempo = max_tiempo;
        }
    }
}
