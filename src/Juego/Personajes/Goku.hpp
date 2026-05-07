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
        static constexpr const char* SPR_WALK  = ASSETS "/sprites/goku_walk.png";

        // ─── Frame counts ───────────────────────────────────────────────────
        static constexpr int FRAMES_WALK  = 4; // Ajustar según el PNG real
        static constexpr int FRAMES_IDLE  = 1;

        /**
         * @brief Registra todas las texturas del personaje.
         */
        static void registrarAssets()
        {
            auto& assets = CE::GestorAssets::Get();
            using V2 = CE::Vector2D;

            // Por ahora solo tenemos goku_walk, lo usamos para todo
            assets.agregarTextura("goku_walk", SPR_WALK, V2{0,0}, V2{(float)(FRAME_W * FRAMES_WALK), (float)FRAME_H});
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

            // Mapa de animaciones (usamos goku_walk para todo por ahora)
            auto anim = std::make_shared<IAnimaciones>();
            anim->set("idle",  "goku_walk");
            anim->set("walk",  "goku_walk");
            anim->set("run",   "goku_walk");
            anim->set("jump",  "goku_walk");
            anim->set("punch", "goku_walk");
            anim->set("kick",  "goku_walk");

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
