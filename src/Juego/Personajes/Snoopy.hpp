/**
 * @file Snoopy.hpp
 * @brief Preset del personaje "Snoopy" (Esnupi).
 *
 * Para cargar al personaje en la escena solo hace falta llamar:
 *
 *   IVJ::Snoopy::cargar(player, objetos);
 *
 * Eso registra todas las texturas, adjunta todos los componentes
 * y establece la FSM inicial del jugador.
 */
#pragma once

#include "Juego/objetos/Entidad.hpp"
#include "Juego/Figuras/Figuras.hpp"
#include "Juego/Componentes/IJComponentes.hpp"
#include "Juego/Maquinas/Naves/IdleJugadores.hpp"
#include "Motor/Componentes/IComponentes.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"

namespace IVJ
{
    /**
     * @brief Preset del personaje Snoopy / Esnupi.
     *
     * Centraliza toda la configuración del personaje para que la escena
     * no tenga que conocer los detalles (rutas de sprites, tamaños de frame,
     * velocidades, etc.).
     */
    struct Snoopy
    {
        // ─── Constantes del personaje ───────────────────────────────────────
        static constexpr float VELOCIDAD    = 120.f;  ///< Velocidad base (px/s)
        static constexpr float GRAVEDAD     = 1200.f; ///< Aceleración gravitatoria
        static constexpr float SALTO        = 600.f;  ///< Impulso de salto
        static constexpr float ESCALA       = 2.f;    ///< Escala visual del sprite
        static constexpr int   FRAME_W      = 20;     ///< Ancho de un frame (px)
        static constexpr int   FRAME_H      = 20;     ///< Alto de un frame (px)

        // ─── Rutas de sprites ───────────────────────────────────────────────
        static constexpr const char* SPR_WALK  = ASSETS "/sprites/esnupi_walk.png";
        static constexpr const char* SPR_RUN   = ASSETS "/sprites/esnupi_run.png";
        static constexpr const char* SPR_JUMP  = ASSETS "/sprites/esnupi_jump.png";
        static constexpr const char* SPR_IDLE  = ASSETS "/sprites/esnupi_idle.png";
        static constexpr const char* SPR_PUNCH = ASSETS "/sprites/esnupi_punch.png";
        static constexpr const char* SPR_KICK  = ASSETS "/sprites/esnupi_kick.png";

        // ─── Frame counts por animación ─────────────────────────────────────
        static constexpr int FRAMES_WALK  = 4;
        static constexpr int FRAMES_RUN   = 6;
        static constexpr int FRAMES_JUMP  = 5;
        static constexpr int FRAMES_IDLE  = 4;
        static constexpr int FRAMES_PUNCH = 4;
        static constexpr int FRAMES_KICK  = 4;

        /**
         * @brief Registra todas las texturas del personaje en el GestorAssets.
         *
         * Llama esta función una sola vez al inicializar la escena.
         * Si las texturas ya existen, el gestor las ignora sin problema.
         */
        static void registrarAssets()
        {
            auto& assets = CE::GestorAssets::Get();
            using V2 = CE::Vector2D;

            assets.agregarTextura("esnupi_walk",  SPR_WALK,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_WALK),  (float)FRAME_H});
            assets.agregarTextura("esnupi_run",   SPR_RUN,   V2{0,0}, V2{(float)(FRAME_W * FRAMES_RUN),   (float)FRAME_H});
            assets.agregarTextura("esnupi_jump",  SPR_JUMP,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_JUMP),  (float)FRAME_H});
            assets.agregarTextura("esnupi_idle",  SPR_IDLE,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_IDLE),  (float)FRAME_H});
            assets.agregarTextura("esnupi_punch", SPR_PUNCH, V2{0,0}, V2{(float)(FRAME_W * FRAMES_PUNCH), (float)FRAME_H});
            assets.agregarTextura("esnupi_kick",  SPR_KICK,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_KICK),  (float)FRAME_H});
        }

        /**
         * @brief Configura todos los componentes del jugador y su FSM inicial.
         *
         * @param entidad  La entidad que representará al personaje.
         * @param posX     Posición inicial en X.
         * @param posY     Posición inicial en Y.
         */
        static void cargar(std::shared_ptr<Entidad>& entidad, float posX = 300.f, float posY = 300.f)
        {
            registrarAssets();

            entidad->setPosicion(posX, posY);
            entidad->getTransformada()->velocidad = CE::Vector2D{VELOCIDAD, VELOCIDAD};

            auto sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura("esnupi_walk"),
                FRAME_W, FRAME_H,
                ESCALA);

            // Mapa de animaciones: los FSMs leerán de aquí en lugar de usar nombres hardcodeados
            auto anim = std::make_shared<IAnimaciones>();
            anim->set("idle",  "esnupi_idle");
            anim->set("walk",  "esnupi_walk");
            anim->set("run",   "esnupi_run");
            anim->set("jump",  "esnupi_jump");
            anim->set("punch", "esnupi_punch");
            anim->set("kick",  "esnupi_kick");

            entidad->addComponente(std::make_shared<CE::IControl>());
            entidad->addComponente(std::make_shared<CE::IBoundingBox>(
                CE::Vector2D{(FRAME_W - 2) * ESCALA, (FRAME_H - 2) * ESCALA},
                CE::CollisionLayer::PLAYER));
            entidad->addComponente(std::make_shared<IGravedad>(GRAVEDAD, SALTO));
            entidad->addComponente(sprite);
            entidad->addComponente(anim);

            auto me = std::make_shared<IMaquinaEstado>();
            me->fsm = std::make_shared<IdleJugadores>(FRAMES_IDLE, 0.15f);
            entidad->addComponente(me);
            entidad->setFSM(me->fsm);
        }
    };
}
