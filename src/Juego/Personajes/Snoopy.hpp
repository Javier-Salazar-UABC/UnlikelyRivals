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
        static constexpr const char* SPR_WALK  = ASSETS "/sprites/snoopy/esnupi_walk.png";
        static constexpr const char* SPR_RUN   = ASSETS "/sprites/snoopy/esnupi_run.png";
        static constexpr const char* SPR_JUMP  = ASSETS "/sprites/snoopy/esnupi_jump.png";
        static constexpr const char* SPR_IDLE  = ASSETS "/sprites/snoopy/esnupi_idle.png";
        static constexpr const char* SPR_PUNCH = ASSETS "/sprites/snoopy/esnupi_punch.png";
        static constexpr const char* SPR_KICK  = ASSETS "/sprites/snoopy/esnupi_kick.png";
        static constexpr const char* SPR_DOWN_B = ASSETS "/sprites/snoopy/esnupi_down-B.png";

        // ─── Rutas de mapas de normales ─────────────────────────────────────
        static constexpr const char* SPR_WALK_N  = ASSETS "/sprites/snoopy/NormalMaps/esnupi_walk_n.png";
        static constexpr const char* SPR_RUN_N   = ASSETS "/sprites/snoopy/NormalMaps/esnupi_run_n.png";
        static constexpr const char* SPR_JUMP_N  = ASSETS "/sprites/snoopy/NormalMaps/esnupi_jump_n.png";
        static constexpr const char* SPR_IDLE_N  = ASSETS "/sprites/snoopy/NormalMaps/esnupi_idle_n.png";
        static constexpr const char* SPR_PUNCH_N = ASSETS "/sprites/snoopy/NormalMaps/esnupi_punch_n.png";
        static constexpr const char* SPR_KICK_N  = ASSETS "/sprites/snoopy/NormalMaps/esnupi_kick_n.png";
        static constexpr const char* SPR_DOWN_B_N = ASSETS "/sprites/snoopy/NormalMaps/esnupi_down-B_n.png";

        // ─── Frame counts por animación ─────────────────────────────────────
        static constexpr int FRAMES_WALK  = 4;
        static constexpr int FRAMES_RUN   = 6;
        static constexpr int FRAMES_JUMP  = 5;
        static constexpr int FRAMES_IDLE  = 4;
        static constexpr int FRAMES_PUNCH = 4;
        static constexpr int FRAMES_KICK  = 4;
        static constexpr int FRAMES_DOWN_B = 6;

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
            assets.agregarTextura("esnupi_down_b", SPR_DOWN_B, V2{0,0}, V2{(float)(40 * FRAMES_DOWN_B), 40.f});

            // Registrar mapas de normales correspondientes
            assets.agregarTextura("esnupi_walk_n",  SPR_WALK_N,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_WALK),  (float)FRAME_H});
            assets.agregarTextura("esnupi_run_n",   SPR_RUN_N,   V2{0,0}, V2{(float)(FRAME_W * FRAMES_RUN),   (float)FRAME_H});
            assets.agregarTextura("esnupi_jump_n",  SPR_JUMP_N,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_JUMP),  (float)FRAME_H});
            assets.agregarTextura("esnupi_idle_n",  SPR_IDLE_N,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_IDLE),  (float)FRAME_H});
            assets.agregarTextura("esnupi_punch_n", SPR_PUNCH_N, V2{0,0}, V2{(float)(FRAME_W * FRAMES_PUNCH), (float)FRAME_H});
            assets.agregarTextura("esnupi_kick_n",  SPR_KICK_N,  V2{0,0}, V2{(float)(FRAME_W * FRAMES_KICK),  (float)FRAME_H});
            assets.agregarTextura("esnupi_down_b_n", SPR_DOWN_B_N, V2{0,0}, V2{(float)(40 * FRAMES_DOWN_B), 40.f});
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
            entidad->getStats()->multiplicador_fuerza = 1.0f; // Fuerza normal


            auto sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura("esnupi_walk"),
                FRAME_W, FRAME_H,
                ESCALA);

            // Mapa de animaciones: los FSMs leerán de aquí en lugar de usar nombres hardcodeados
            auto anim = std::make_shared<IAnimaciones>();
            anim->set("idle",  "esnupi_idle",  FRAMES_IDLE,  0.15f);
            anim->set("walk",  "esnupi_walk",  FRAMES_WALK,  0.1f);
            anim->set("run",   "esnupi_run",   FRAMES_RUN,   0.08f);
            anim->set("jump",  "esnupi_jump",  FRAMES_JUMP,  0.1f);
            anim->set("punch", "esnupi_punch", FRAMES_PUNCH, 0.1f);
            anim->set("kick",  "esnupi_kick",  FRAMES_KICK,  0.1f);
            anim->set("hit",   "esnupi_jump",  FRAMES_JUMP,  0.1f); // Reusar salto para hit
            anim->set("down_b", "esnupi_down_b", FRAMES_DOWN_B, 0.1f, 40, 40);

            entidad->addComponente(std::make_shared<CE::IControl>());
            entidad->addComponente(std::make_shared<CE::IBoundingBox>(
                CE::Vector2D{(FRAME_W - 2) * ESCALA, (FRAME_H - 2) * ESCALA},
                CE::CollisionLayer::PLAYER));
            entidad->addComponente(std::make_shared<IGravedad>(GRAVEDAD, SALTO));
            entidad->addComponente(sprite);
            entidad->addComponente(anim);

            // Crear y configurar el mapa de normales para Snoopy
            auto normalMapComp = std::make_shared<CE::INormalMap>();
            auto& assets = CE::GestorAssets::Get();
            normalMapComp->registrarNormalMap(&assets.getTextura("esnupi_walk"),  &assets.getTextura("esnupi_walk_n"));
            normalMapComp->registrarNormalMap(&assets.getTextura("esnupi_run"),   &assets.getTextura("esnupi_run_n"));
            normalMapComp->registrarNormalMap(&assets.getTextura("esnupi_jump"),  &assets.getTextura("esnupi_jump_n"));
            normalMapComp->registrarNormalMap(&assets.getTextura("esnupi_idle"),  &assets.getTextura("esnupi_idle_n"));
            normalMapComp->registrarNormalMap(&assets.getTextura("esnupi_punch"), &assets.getTextura("esnupi_punch_n"));
            normalMapComp->registrarNormalMap(&assets.getTextura("esnupi_kick"),  &assets.getTextura("esnupi_kick_n"));
            normalMapComp->registrarNormalMap(&assets.getTextura("esnupi_down_b"), &assets.getTextura("esnupi_down_b_n"));

            
            entidad->addComponente(normalMapComp);

            auto me = std::make_shared<IMaquinaEstado>();
            me->fsm = std::make_shared<IdleJugadores>(FRAMES_IDLE, 0.15f);
            entidad->addComponente(me);
            entidad->setFSM(me->fsm);
        }
    };
}
