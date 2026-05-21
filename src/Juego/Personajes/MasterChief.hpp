/**
 * @file MasterChief.hpp
 * @brief Preset del personaje "Master Chief" (Jeilo).
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
    struct MasterChief
    {
        // ─── Constantes del personaje ───────────────────────────────────────
        static constexpr float VELOCIDAD    = 100.f;  
        static constexpr float GRAVEDAD     = 1200.f; 
        static constexpr float SALTO        = 550.f;  
        static constexpr float ESCALA       = 2.2f;   
        static constexpr int   FRAME_W      = 16;     
        static constexpr int   FRAME_H      = 32;     

        // ─── Rutas de sprites ───────────────────────────────────────────────
        static constexpr const char* SPR_WALK  = ASSETS "/sprites/master_chief/jeilo_walk.png";
        // Por ahora reutilizamos walk para otras animaciones si no hay más
        static constexpr const char* SPR_IDLE  = ASSETS "/sprites/master_chief/jeilo_walk.png";

        // ─── Frame counts ───────────────────────────────────────────────────
        static constexpr int FRAMES_WALK  = 4;
        static constexpr int FRAMES_IDLE  = 1;

        /**
         * @brief Registra todas las texturas del personaje.
         */
        static void registrarAssets()
        {
            auto& assets = CE::GestorAssets::Get();
            using V2 = CE::Vector2D;

            assets.agregarTextura("jeilo_walk", SPR_WALK, V2{0,0}, V2{(float)(FRAME_W * FRAMES_WALK), (float)FRAME_H});
            assets.agregarTextura("jeilo_idle", SPR_IDLE, V2{0,0}, V2{(float)(FRAME_W * FRAMES_IDLE), (float)FRAME_H});
        }

        /**
         * @brief Configura todos los componentes del Master Chief.
         */
        static void cargar(std::shared_ptr<Entidad>& entidad, float posX = 420.f, float posY = 300.f)
        {
            registrarAssets();

            entidad->setPosicion(posX, posY);
            entidad->getTransformada()->velocidad = CE::Vector2D{VELOCIDAD, VELOCIDAD};
            entidad->getStats()->multiplicador_fuerza = 1.5f; // ¡Es un Spartan!


            auto sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura("jeilo_walk"),
                FRAME_W, FRAME_H,
                ESCALA);

            // Mapa de animaciones: claves propias de Jeilo (reutiliza walk donde no hay sprite propio)
            auto anim = std::make_shared<IAnimaciones>();
            anim->set("idle",  "jeilo_walk", FRAMES_IDLE, 0.15f);
            anim->set("walk",  "jeilo_walk", FRAMES_WALK, 0.1f);
            anim->set("run",   "jeilo_walk", FRAMES_WALK, 0.08f);  // Sin sprite de correr: usa walk
            anim->set("jump",  "jeilo_walk", FRAMES_WALK, 0.1f);   // Sin sprite de salto: usa walk
            anim->set("punch", "jeilo_walk", FRAMES_WALK, 0.1f);   // Sin sprite de golpe: usa walk
            anim->set("kick",  "jeilo_walk", FRAMES_WALK, 0.1f);   // Sin sprite de patada: usa walk
            anim->set("hit",   "jeilo_walk", FRAMES_WALK, 0.1f);   // Hitstun

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
