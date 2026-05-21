/**
 * @file Goku.hpp
 * @brief Preset del personaje "Goku".
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
    struct Goku
    {
        // ─── Constantes del personaje ───────────────────────────────────────
        static constexpr float VELOCIDAD    = 180.f;  // Goku es muy rápido
        static constexpr float GRAVEDAD     = 1200.f; 
        static constexpr float SALTO        = 700.f;  // Salta muy alto
        static constexpr float ESCALA       = 2.2f;   // Igual que Master Chief
        static constexpr int   FRAME_W      = 16;
        static constexpr int   FRAME_H      = 32;


        // ─── Rutas de sprites ───────────────────────────────────────────────
        static constexpr const char* SPR_WALK   = ASSETS "/sprites/goku/goku_walk.png";
        static constexpr const char* SPR_ATTACK = ASSETS "/sprites/goku/goku_attack.png";

        // ─── Frame counts ───────────────────────────────────────────────────
        static constexpr int FRAMES_WALK   = 4; 
        static constexpr int FRAMES_IDLE   = 1;
        static constexpr int FRAMES_ATTACK = 7;

        /**
         * @brief Registra todas las texturas del personaje.
         */
        static void registrarAssets()
        {
            auto& assets = CE::GestorAssets::Get();
            using V2 = CE::Vector2D;

            assets.agregarTextura("goku_walk",   SPR_WALK,   V2{0,0}, V2{(float)(FRAME_W * FRAMES_WALK),   (float)FRAME_H});
            assets.agregarTextura("goku_attack", SPR_ATTACK, V2{0,0}, V2{(float)(32 * FRAMES_ATTACK), (float)32});
        }

        /**
         * @brief Configura todos los componentes de Goku.
         */
        static void cargar(std::shared_ptr<Entidad>& entidad, float posX = 300.f, float posY = 300.f)
        {
            registrarAssets();

            entidad->setPosicion(posX, posY);
            entidad->getTransformada()->velocidad = CE::Vector2D{VELOCIDAD, VELOCIDAD};
            
            // Goku es el más fuerte de todos
            entidad->getStats()->multiplicador_fuerza = 2.5f; 

            auto sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura("goku_walk"),
                FRAME_W, FRAME_H,
                ESCALA);

            // Mapa de animaciones (ahora con metadatos de frames, velocidad y tamaño)
            auto anim = std::make_shared<IAnimaciones>();
            anim->set("idle",  "goku_walk",   FRAMES_IDLE, 0.15f, FRAME_W, FRAME_H);
            anim->set("walk",  "goku_walk",   FRAMES_WALK, 0.1f,  FRAME_W, FRAME_H);
            anim->set("run",   "goku_walk",   FRAMES_WALK, 0.08f, FRAME_W, FRAME_H);
            anim->set("jump",  "goku_walk",   FRAMES_WALK, 0.1f,  FRAME_W, FRAME_H);
            anim->set("punch", "goku_attack", FRAMES_ATTACK, 0.06f, 32, 32);
            anim->set("kick",  "goku_attack", FRAMES_ATTACK, 0.06f, 32, 32);
            anim->set("hit",   "goku_attack", FRAMES_ATTACK, 0.06f, 32, 32); // Reusar frame de ataque para hit

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
