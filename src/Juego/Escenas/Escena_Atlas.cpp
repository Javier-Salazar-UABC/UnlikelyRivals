#include "Escena_Atlas.hpp"

#include "Motor/Utils/Vector2D.hpp"
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <memory>
#include <cmath>
#include <algorithm>

#include "Juego/Maquinas/Naves/IdleJugadores.hpp"
#include "Juego/Sistemas/Sistemas.hpp"
#include "Motor/Camaras/CamarasGestor.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "Juego/Personajes/Snoopy.hpp"
#include "Juego/Personajes/MasterChief.hpp"
#include "Juego/Personajes/Goku.hpp"
#include "Juego/Globales.hpp"


namespace IVJ
{
    Escena_Atlas::Escena_Atlas(std::shared_ptr<Entidad>& pref)
        :CE::Escena{},player{pref}
    {
    }
    void Escena_Atlas::onInit()
    {
        SistemaAudioInit();
        CE::GestorCamaras::Get().setCamaraActiva(4);
        
        // --- 1. CONFIGURACIÓN ESTÁTICA (SOLO UNA VEZ) ---
        if (inicializar)
        {
            registrarBotones(sf::Keyboard::Scancode::W,"arriba");
            registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
            registrarBotones(sf::Keyboard::Scancode::S,"abajo");
            registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
            registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
            registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
            registrarBotones(sf::Keyboard::Scancode::D,"derecha");
            registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
            registrarBotones(sf::Keyboard::Scancode::LShift,"correr");
            registrarBotones(sf::Keyboard::Scancode::RShift,"correr");
            registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");
            registrarBotones(sf::Keyboard::Scancode::J,"atacar");
            registrarBotones(sf::Keyboard::Scancode::K,"especial");

            // --- Mapeo de Control (Switch Pro / Xbox / PS) ---
            for (unsigned int i = 0; i < 4; ++i)
            {
                // Botones principales (A/B/X/Y o Cross/Circle/Square/Triangle)
                registrarJoystickBoton(i, 0, "atacar");
                registrarJoystickBoton(i, 1, "especial");
                registrarJoystickBoton(i, 2, "correr");
                registrarJoystickBoton(i, 3, "correr");
                
                // Triggers / Bumpers
                registrarJoystickBoton(i, 4, "correr"); // L1 / LB
                registrarJoystickBoton(i, 5, "correr"); // R1 / RB
                registrarJoystickBoton(i, 7, "atacar"); // R2 / ZR
                
                // Ejes para movimiento (Sticks y D-Pad)
                registrarJoystickEje(i, sf::Joystick::Axis::X, "horizontal");
                registrarJoystickEje(i, sf::Joystick::Axis::Y, "vertical");
                registrarJoystickEje(i, sf::Joystick::Axis::PovX, "horizontal");
                registrarJoystickEje(i, sf::Joystick::Axis::PovY, "vertical");
            }

            //cargar mapa de Namek (Fondo y Escenario principal)
            CE::GestorAssets::Get().agregarTextura("fondo_namek", ASSETS "/atlas/fondo_namek.png", {0.f,0.f}, {382.f, 216.f});
            CE::GestorAssets::Get().agregarTextura("namek", ASSETS "/atlas/escenario_namek.png", {0.f,0.f}, {382.f, 216.f});

            // 1. Agregar fondo
            auto fondo_sprite = std::make_shared<CE::ISprite>(CE::GestorAssets::Get().getTextura("fondo_namek"), 382, 216, 3.f);
            auto fondo_ent = std::make_shared<Entidad>();
            fondo_ent->addComponente(fondo_sprite);
            fondo_ent->setPosicion(382.f * 1.5f, 216.f * 1.5f);
            objetos.agregarPool(fondo_ent);

            // 2. Agregar escenario principal
            auto map_sprite = std::make_shared<CE::ISprite>(CE::GestorAssets::Get().getTextura("namek"), 382, 216, 3.f);
            auto map_ent = std::make_shared<Entidad>();
            map_ent->addComponente(map_sprite);
            map_ent->setPosicion(382.f * 1.5f, 216.f * 1.5f);
            objetos.agregarPool(map_ent);

            // --- Configurar límites de la cámara ---
            float worldW = 382.f * 3.f;
            float worldH = 216.f * 3.f;
            
            auto& camActiva = CE::GestorCamaras::Get().getCamaraActiva();
            if (auto* smash = dynamic_cast<CE::CamaraSmash*>(&camActiva)) {
                smash->setWorldBounds(sf::FloatRect({0, 0}, {worldW, worldH}));
            }

            // --- Configurar colisiones irregulares del mapa Namek ---
            auto crearPlataforma = [&](float cx, float cy, float w, float h, const std::string& nombre = "") {
                auto plat = std::make_shared<Entidad>();
                plat->getStats()->hp_max = 100;
                plat->getStats()->hp = 100;
                plat->getStats()->def = 150;
                plat->addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{w, h}));
                if (!nombre.empty()) {
                    plat->addComponente(std::make_shared<CE::INombre>(nombre));
                }
                plat->setPosicion(cx, cy);
                objetos.agregarPool(plat);
            };

            // --- SUELO (Pixel-Perfect) ---
            crearPlataforma(192.0f, 535.5f, 3.0f, 225.0f);
            crearPlataforma(195.0f, 537.0f, 3.0f, 222.0f);
            crearPlataforma(201.0f, 534.0f, 9.0f, 228.0f);
            crearPlataforma(210.0f, 523.5f, 9.0f, 249.0f);
            crearPlataforma(216.0f, 522.0f, 3.0f, 252.0f);
            crearPlataforma(222.0f, 519.0f, 9.0f, 258.0f);
            crearPlataforma(228.0f, 517.5f, 3.0f, 261.0f);
            crearPlataforma(232.5f, 519.0f, 6.0f, 258.0f);
            crearPlataforma(238.5f, 532.5f, 6.0f, 231.0f);
            crearPlataforma(246.0f, 534.0f, 9.0f, 228.0f);
            crearPlataforma(252.0f, 535.5f, 3.0f, 225.0f);
            crearPlataforma(256.5f, 538.5f, 6.0f, 219.0f);
            crearPlataforma(261.0f, 537.0f, 3.0f, 222.0f);
            crearPlataforma(267.0f, 538.5f, 9.0f, 219.0f);
            crearPlataforma(277.5f, 540.0f, 12.0f, 216.0f);
            crearPlataforma(291.0f, 540.0f, 3.0f, 216.0f);
            crearPlataforma(304.5f, 540.0f, 18.0f, 216.0f);
            crearPlataforma(315.0f, 541.5f, 3.0f, 213.0f);
            crearPlataforma(328.5f, 540.0f, 24.0f, 216.0f);
            crearPlataforma(345.0f, 540.0f, 3.0f, 216.0f);
            crearPlataforma(354.0f, 541.5f, 3.0f, 213.0f);
            crearPlataforma(357.0f, 540.0f, 3.0f, 216.0f);
            crearPlataforma(366.0f, 538.5f, 9.0f, 219.0f);
            crearPlataforma(382.5f, 540.0f, 6.0f, 216.0f);
            crearPlataforma(387.0f, 541.5f, 3.0f, 213.0f);
            crearPlataforma(391.5f, 538.5f, 6.0f, 219.0f);
            crearPlataforma(396.0f, 535.5f, 3.0f, 225.0f);
            crearPlataforma(400.5f, 534.0f, 6.0f, 228.0f);
            crearPlataforma(405.0f, 532.5f, 3.0f, 231.0f);
            crearPlataforma(409.5f, 531.0f, 6.0f, 234.0f);
            crearPlataforma(414.0f, 528.0f, 3.0f, 240.0f);
            crearPlataforma(417.0f, 529.5f, 3.0f, 237.0f);
            crearPlataforma(423.0f, 514.5f, 3.0f, 267.0f);
            crearPlataforma(426.0f, 513.0f, 3.0f, 270.0f);
            crearPlataforma(430.5f, 514.5f, 6.0f, 267.0f);
            crearPlataforma(435.0f, 513.0f, 3.0f, 270.0f);
            crearPlataforma(444.0f, 511.5f, 15.0f, 273.0f);
            crearPlataforma(453.0f, 513.0f, 3.0f, 270.0f);
            crearPlataforma(456.0f, 511.5f, 3.0f, 273.0f);
            crearPlataforma(462.0f, 513.0f, 9.0f, 270.0f);
            crearPlataforma(474.0f, 514.5f, 15.0f, 267.0f);
            crearPlataforma(496.5f, 511.5f, 30.0f, 273.0f);
            crearPlataforma(513.0f, 510.0f, 3.0f, 276.0f);
            crearPlataforma(519.0f, 507.0f, 9.0f, 282.0f);
            crearPlataforma(525.0f, 510.0f, 3.0f, 276.0f);
            crearPlataforma(538.5f, 505.5f, 24.0f, 285.0f);
            crearPlataforma(559.5f, 502.5f, 18.0f, 291.0f);
            crearPlataforma(571.5f, 501.0f, 6.0f, 294.0f);
            crearPlataforma(579.0f, 498.0f, 9.0f, 300.0f);
            crearPlataforma(588.0f, 501.0f, 9.0f, 294.0f);
            crearPlataforma(597.0f, 502.5f, 9.0f, 291.0f);
            crearPlataforma(603.0f, 507.0f, 3.0f, 282.0f);
            crearPlataforma(607.5f, 484.5f, 6.0f, 327.0f);
            crearPlataforma(615.0f, 483.0f, 9.0f, 330.0f);
            crearPlataforma(627.0f, 481.5f, 15.0f, 333.0f);
            crearPlataforma(643.5f, 480.0f, 18.0f, 336.0f);
            crearPlataforma(663.0f, 478.5f, 21.0f, 339.0f);
            crearPlataforma(675.0f, 489.0f, 3.0f, 318.0f);
            crearPlataforma(678.0f, 493.5f, 3.0f, 309.0f);
            crearPlataforma(685.5f, 480.0f, 12.0f, 336.0f);
            crearPlataforma(696.0f, 478.5f, 9.0f, 339.0f);
            crearPlataforma(702.0f, 477.0f, 3.0f, 342.0f);
            crearPlataforma(714.0f, 474.0f, 3.0f, 348.0f);
            crearPlataforma(717.0f, 472.5f, 3.0f, 351.0f);
            crearPlataforma(727.5f, 462.0f, 18.0f, 372.0f);
            crearPlataforma(739.5f, 460.5f, 6.0f, 375.0f);
            crearPlataforma(744.0f, 462.0f, 3.0f, 372.0f);
            crearPlataforma(748.5f, 460.5f, 6.0f, 375.0f);
            crearPlataforma(753.0f, 463.5f, 3.0f, 369.0f);
            crearPlataforma(756.0f, 465.0f, 3.0f, 366.0f);
            crearPlataforma(759.0f, 466.5f, 3.0f, 363.0f);
            crearPlataforma(762.0f, 468.0f, 3.0f, 360.0f);
            crearPlataforma(765.0f, 466.5f, 3.0f, 363.0f);
            crearPlataforma(768.0f, 454.5f, 3.0f, 387.0f);
            crearPlataforma(775.5f, 453.0f, 12.0f, 390.0f);
            crearPlataforma(795.0f, 442.5f, 3.0f, 411.0f);
            crearPlataforma(799.5f, 444.0f, 6.0f, 408.0f);
            crearPlataforma(813.0f, 441.0f, 9.0f, 414.0f);
            crearPlataforma(823.5f, 442.5f, 12.0f, 411.0f);
            crearPlataforma(831.0f, 441.0f, 3.0f, 414.0f);
            crearPlataforma(835.5f, 448.5f, 6.0f, 399.0f);
            crearPlataforma(840.0f, 451.5f, 3.0f, 393.0f);
            crearPlataforma(844.5f, 442.5f, 6.0f, 411.0f);
            crearPlataforma(852.0f, 444.0f, 9.0f, 408.0f);
            crearPlataforma(859.5f, 456.0f, 6.0f, 384.0f);
            crearPlataforma(868.5f, 444.0f, 12.0f, 408.0f);
            crearPlataforma(877.5f, 451.5f, 6.0f, 393.0f);
            crearPlataforma(882.0f, 450.0f, 3.0f, 396.0f);
            crearPlataforma(886.5f, 448.5f, 6.0f, 399.0f);
            crearPlataforma(891.0f, 450.0f, 3.0f, 396.0f);
            crearPlataforma(895.5f, 447.0f, 6.0f, 402.0f);
            crearPlataforma(909.0f, 448.5f, 21.0f, 399.0f);
            crearPlataforma(921.0f, 451.5f, 3.0f, 393.0f);
            crearPlataforma(924.0f, 448.5f, 3.0f, 399.0f);
            crearPlataforma(927.0f, 453.0f, 3.0f, 390.0f);
            crearPlataforma(936.0f, 451.5f, 15.0f, 393.0f);

            // --- NAVE CAPSULA ---
            crearPlataforma(240.0f, 403.5f, 3.0f, 57.0f);
            crearPlataforma(243.0f, 399.0f, 3.0f, 66.0f);
            crearPlataforma(246.0f, 394.5f, 3.0f, 75.0f);
            crearPlataforma(249.0f, 393.0f, 3.0f, 78.0f);
            crearPlataforma(252.0f, 390.0f, 3.0f, 84.0f);
            crearPlataforma(255.0f, 388.5f, 3.0f, 87.0f);
            crearPlataforma(258.0f, 387.0f, 3.0f, 90.0f);
            crearPlataforma(261.0f, 385.5f, 3.0f, 93.0f);
            crearPlataforma(264.0f, 384.0f, 3.0f, 96.0f);
            crearPlataforma(267.0f, 382.5f, 3.0f, 99.0f);
            crearPlataforma(271.5f, 381.0f, 6.0f, 102.0f);
            crearPlataforma(276.0f, 379.5f, 3.0f, 105.0f);
            crearPlataforma(279.0f, 378.0f, 3.0f, 108.0f);
            crearPlataforma(282.0f, 375.0f, 3.0f, 114.0f);
            crearPlataforma(285.0f, 373.5f, 3.0f, 117.0f);
            crearPlataforma(291.0f, 375.0f, 9.0f, 114.0f);
            crearPlataforma(298.5f, 373.5f, 6.0f, 117.0f);
            crearPlataforma(310.5f, 372.0f, 18.0f, 120.0f);
            crearPlataforma(330.0f, 370.5f, 21.0f, 123.0f);
            crearPlataforma(349.5f, 372.0f, 18.0f, 120.0f);
            crearPlataforma(361.5f, 373.5f, 6.0f, 117.0f);
            crearPlataforma(367.5f, 375.0f, 6.0f, 114.0f);
            crearPlataforma(375.0f, 376.5f, 9.0f, 111.0f);
            crearPlataforma(382.5f, 378.0f, 6.0f, 108.0f);
            crearPlataforma(387.0f, 376.5f, 3.0f, 111.0f);
            crearPlataforma(391.5f, 381.0f, 6.0f, 102.0f);
            crearPlataforma(396.0f, 384.0f, 3.0f, 96.0f);
            crearPlataforma(399.0f, 385.5f, 3.0f, 93.0f);
            crearPlataforma(402.0f, 387.0f, 3.0f, 90.0f);
            crearPlataforma(405.0f, 388.5f, 3.0f, 87.0f);
            crearPlataforma(408.0f, 390.0f, 3.0f, 84.0f);
            crearPlataforma(411.0f, 393.0f, 3.0f, 78.0f);

            // --- PLATAFORMA FLOTANTE 1 ---
            crearPlataforma(489.0f, 294.0f, 21.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(502.5f, 291.0f, 6.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(507.0f, 297.0f, 3.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(526.5f, 291.0f, 36.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(549.0f, 297.0f, 9.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(558.0f, 291.0f, 9.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(571.5f, 294.0f, 18.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(582.0f, 297.0f, 3.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(585.0f, 294.0f, 3.0f, 15.0f, "plataforma_flotante");

            // --- PLATAFORMA FLOTANTE 2 ---
            crearPlataforma(630.0f, 213.0f, 3.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(637.5f, 210.0f, 12.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(694.5f, 207.0f, 102.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(753.0f, 210.0f, 15.0f, 15.0f, "plataforma_flotante");
            crearPlataforma(762.0f, 213.0f, 3.0f, 15.0f, "plataforma_flotante");

            inicializar = false;
        }

        // --- 2. CONFIGURACIÓN DINÁMICA (CADA VEZ QUE SE ENTRA AL NIVEL) ---
        
        // A. Eliminar jugadores antiguos de la pool de objetos
        auto& poolVec = objetos.getPool();
        if (player) {
            poolVec.erase(std::remove(poolVec.begin(), poolVec.end(), player), poolVec.end());
        }
        if (p2) {
            poolVec.erase(std::remove(poolVec.begin(), poolVec.end(), p2), poolVec.end());
        }

        // B. Cargar/Re-crear jugador 1 según selección
        auto loadPlayer = [&](std::shared_ptr<Entidad>& ent, PersonajeID id, float x, float y) {
            switch(id) {
                case PersonajeID::SNOOPY:       Snoopy::cargar(ent, x, y); break;
                case PersonajeID::MASTER_CHIEF: MasterChief::cargar(ent, x, y); break;
                case PersonajeID::GOKU:         Goku::cargar(ent, x, y); break;
            }
        };

        player = std::make_shared<Entidad>();
        loadPlayer(player, Globales::p1_seleccionado, m_p1StartX, m_p1StartY);
        player->getStats()->porcentaje_danio = 0.f;

        // C. Cargar/Re-crear jugador 2 según selección
        p2 = std::make_shared<Entidad>();
        loadPlayer(p2, Globales::p2_seleccionado, m_p2StartX, m_p2StartY);
        p2->getStats()->porcentaje_danio = 0.f;

        objetos.agregarPool(player);
        objetos.agregarPool(p2);

        // D. Sincronizar objetivos de la cámara Smash con los nuevos jugadores
        auto& camActive = CE::GestorCamaras::Get().getCamaraActiva();
        if (auto* s = dynamic_cast<CE::CamaraSmash*>(&camActive)) {
            s->limpiarTargets();
            s->agregarTarget(player);
            s->agregarTarget(p2);
        }

        // E. Inicializar/Resetear cuenta atrás y estado flotante
        m_countdownState = 3;
        m_countdownTimeRemaining = 1.0f;
        m_elapsedTime = 0.0f;

        m_countdownText = std::make_unique<sf::Text>(CE::GestorAssets::Get().getFont("nova"));
        m_countdownText->setCharacterSize(140);
        m_countdownText->setStyle(sf::Text::Bold);

        m_countdownText->setString("3");
        m_countdownText->setFillColor(sf::Color(230, 50, 50));
        {
            sf::FloatRect bounds = m_countdownText->getLocalBounds();
            m_countdownText->setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
            m_countdownText->setPosition({540.f, 360.f});
        }

        // F. Reproducir el audio 321GO.mp3
        sf::Sound& goSound = CE::GestorAssets::Get().getSonido("321go");
        goSound.setPlayingOffset(sf::seconds(0));
        goSound.play();

        // Detener la música de fondo si estaba reproduciéndose (al reiniciar el nivel)
        CE::GestorAssets::Get().getMusica("dragonball_ost").stop();
    }
    void Escena_Atlas::onFinal()
    {
        CE::GestorCamaras::Get().setCamaraActiva(0);
        CE::GestorAssets::Get().getMusica("dragonball_ost").stop();
    }
    void Escena_Atlas::onUpdate(float dt)
    {
        SistemaUpdateAudio(dt);

        // --- SISTEMA COUNTDOWN & FLOTANTE ---
        if (m_countdownState >= 0)
        {
            // Capamos el dt para evitar que los tirones de carga al entrar al nivel
            // se coman el primer segundo de la cuenta atrás (mostrando el 3)
            float countdownDt = std::min(dt, 0.1f);
            m_countdownTimeRemaining -= countdownDt;
            if (m_countdownTimeRemaining <= 0.0f)
            {
                m_countdownState--;
                if (m_countdownState == 0) // Transicion a GO!
                {
                    m_countdownTimeRemaining = 0.8f;

                    m_countdownText->setString("GO!");
                    m_countdownText->setFillColor(sf::Color(255, 100, 0));
                    sf::FloatRect bounds = m_countdownText->getLocalBounds();
                    m_countdownText->setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
                    m_countdownText->setPosition({540.f, 360.f});

                    // Reproducir la música de Dragon Ball al empezar la pelea
                    CE::GestorAssets::Get().getMusica("dragonball_ost").play();
                }
                else if (m_countdownState > 0) // Transicion a 2 o 1
                {
                    m_countdownTimeRemaining = 1.0f;

                    std::string countStr = std::to_string(m_countdownState);
                    sf::Color countColor = (m_countdownState == 2) ? sf::Color(50, 150, 230) : sf::Color(240, 210, 40);
                    m_countdownText->setString(countStr);
                    m_countdownText->setFillColor(countColor);
                    sf::FloatRect bounds = m_countdownText->getLocalBounds();
                    m_countdownText->setOrigin({bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f});
                    m_countdownText->setPosition({540.f, 360.f});
                }
                else // La pelea empezo por completo
                {
                    m_countdownState = -1;
                }
            }
        }

        // Bobbing y bloqueo de personajes en el aire durante la cuenta atras
        if (m_countdownState > 0)
        {
            m_elapsedTime += dt;
            float p1_bob = std::sin(m_elapsedTime * 6.0f) * 12.0f;
            float p2_bob = std::cos(m_elapsedTime * 6.0f) * 12.0f;

            player->setPosicion(m_p1StartX, m_p1StartY + p1_bob);
            if (p2) {
                p2->setPosicion(m_p2StartX, m_p2StartY + p2_bob);
            }

            // Forzar velocidad a cero para evitar aceleracion por gravedad
            if (auto grav1 = player->getComponente<IGravedad>()) {
                grav1->velocidad_Y = 0.0f;
                grav1->en_suelo = false;
            }
            if (auto trans1 = player->getTransformada()) {
                trans1->velocidad = {0.f, 0.f};
            }

            if (p2) {
                if (auto grav2 = p2->getComponente<IGravedad>()) {
                    grav2->velocidad_Y = 0.0f;
                    grav2->en_suelo = false;
                }
                if (auto trans2 = p2->getTransformada()) {
                    trans2->velocidad = {0.f, 0.f};
                }
            }

            // Bloquear todos los inputs
            if (auto ctrl1 = player->getComponente<CE::IControl>()) {
                ctrl1->izq = ctrl1->der = ctrl1->arr = ctrl1->abj = ctrl1->run = ctrl1->jmp = ctrl1->acc = ctrl1->sacc = false;
            }
            if (p2) {
                if (auto ctrl2 = p2->getComponente<CE::IControl>()) {
                    ctrl2->izq = ctrl2->der = ctrl2->arr = ctrl2->abj = ctrl2->run = ctrl2->jmp = ctrl2->acc = ctrl2->sacc = false;
                }
            }
        }

        // --- SISTEMA IA (PVC MODE) ---
        if (Globales::modo_juego == GameMode::PVC && p2 && m_countdownState <= 0) {
            auto ctrl2 = p2->getComponente<CE::IControl>();
            if (ctrl2) {
                // Reset inputs
                ctrl2->izq = ctrl2->der = ctrl2->arr = ctrl2->abj = ctrl2->acc = false;

                auto pos1 = player->getTransformada()->posicion;
                auto pos2 = p2->getTransformada()->posicion;
                float dist = std::abs(pos1.x - pos2.x);
                float distY = std::abs(pos1.y - pos2.y);

                // Movimiento horizontal
                if (dist > 40.0f) {
                    if (pos1.x < pos2.x) ctrl2->izq = true;
                    else ctrl2->der = true;
                }

                // Ataque
                if (dist < 60.0f && distY < 40.0f) {
                    ctrl2->acc = true;
                }

                // Recuperación si está en el aire
                auto grav = p2->getComponente<IGravedad>();
                if (grav && !grav->en_suelo) {
                    if (grav->velocidad_Y > 0) ctrl2->arr = true; // Saltar si está cayendo
                }
            }
        }

        // Primera pasada: actualizar todos los objetos
        for(auto& obj: objetos.getPool())
        {
            obj->inputFSM();
            obj->onUpdate(dt);
            if(obj->tieneComponente<ITarget>())
            {
                auto& tpos = *obj->getComponente<ITarget>()->pos;
                SistemaNPCLookTarget(*obj,tpos);
            }
        }

        // Sistema de partículas fuera de pantalla (Smash Bros style)
        std::vector<std::shared_ptr<CE::Objeto>> targets;
        targets.push_back(player);
        if (p2) targets.push_back(p2);
        SistemaLimitesPantalla(targets);

        // --- MUERTE POR BLAST ZONE (Círculo de muerte estilo Smash) ---
        SistemaMuerteBlastZone(targets, {573, 324}, 750.0f, dt);

        // Segunda pasada: física de objetos dinámicos (los que tienen IGravedad)
        // y su colisión vs todos los tiles/objetos estáticos del pool
        for(auto& obj: objetos.getPool())
        {
            if(!obj->tieneComponente<IGravedad>()) continue;

            SistemaMover(obj, dt);
            SistemaGravedad(obj, dt);

            // Resolver colisión de este objeto dinámico contra todos los demás del pool
            for(auto& tile: objetos.getPool())
            {
                if(tile == obj) continue;           // no contra sí mismo
                
                // Colisión vs Tiles (objetos estáticos)
                if(!tile->tieneComponente<IGravedad>()) {
                    SistemaColAABBMid(*obj, *tile, true);
                }
                // Colisión vs Otros Jugadores / Dinámicos (opcional, estilo Smash)
                else {
                    // Solo resolvemos si son diferentes para evitar que player choque consigo mismo
                    // (Aunque ya tenemos el 'tile == obj' arriba)
                    SistemaColAABBMid(*obj, *tile, true);
                }
            }
        }

        // --- LEDGE TRUMPING (Estilo Super Smash Bros) ---
        // Si dos personajes están colgados de la misma repisa, el que lleva más tiempo (menor cooldown)
        // es "desalojado" (trumped) para que el nuevo tome el control de la repisa.
        std::vector<std::shared_ptr<CE::Objeto>> colgados_izq;
        std::vector<std::shared_ptr<CE::Objeto>> colgados_der;

        for (auto& obj : objetos.getPool()) {
            if (!obj->tieneComponente<IGravedad>()) continue;
            auto grav = obj->getComponente<IGravedad>();
            if (grav->colgado_borde) {
                auto trans = obj->getTransformada();
                if (trans) {
                    if (std::abs(trans->posicion.x - 183.0f) < 5.0f) {
                        colgados_izq.push_back(obj);
                    } else if (std::abs(trans->posicion.x - 942.0f) < 5.0f) {
                        colgados_der.push_back(obj);
                    }
                }
            }
        }

        if (colgados_izq.size() > 1) {
            std::shared_ptr<CE::Objeto> nuevo = colgados_izq[0];
            std::shared_ptr<CE::Objeto> antiguo = colgados_izq[1];
            if (nuevo->getComponente<IGravedad>()->ledge_grab_cooldown < antiguo->getComponente<IGravedad>()->ledge_grab_cooldown) {
                std::swap(nuevo, antiguo);
            }
            auto grav_ant = antiguo->getComponente<IGravedad>();
            auto trans_ant = antiguo->getTransformada();
            grav_ant->colgado_borde = false;
            grav_ant->ledge_grab_cooldown = 0.75f; // Evitar re-agarre inmediato
            grav_ant->en_suelo = false;
            trans_ant->velocidad.x = -150.0f; // Salir volando hacia la izquierda
            grav_ant->velocidad_Y = -300.0f; // Impulso hacia arriba
            ReproducirSonidoAleatorio("jump", 5, false); // Sonido de pop-off (trump)
        }

        if (colgados_der.size() > 1) {
            std::shared_ptr<CE::Objeto> nuevo = colgados_der[0];
            std::shared_ptr<CE::Objeto> antiguo = colgados_der[1];
            if (nuevo->getComponente<IGravedad>()->ledge_grab_cooldown < antiguo->getComponente<IGravedad>()->ledge_grab_cooldown) {
                std::swap(nuevo, antiguo);
            }
            auto grav_ant = antiguo->getComponente<IGravedad>();
            auto trans_ant = antiguo->getTransformada();
            grav_ant->colgado_borde = false;
            grav_ant->ledge_grab_cooldown = 0.75f; // Evitar re-agarre inmediato
            grav_ant->en_suelo = false;
            trans_ant->velocidad.x = 150.0f; // Salir volando hacia la derecha
            grav_ant->velocidad_Y = -300.0f; // Impulso hacia arriba
            ReproducirSonidoAleatorio("jump", 5, false); // Sonido de pop-off (trump)
        }

        // Procesar golpes de ambos jugadores
        SistemaGolpe(player, objetos);
        if (p2) SistemaGolpe(p2, objetos);
    }
    void Escena_Atlas::onInputs(const CE::Botones& accion)
    {
        if (m_countdownState > 0) return;

        // Ruteo de entrada: Gamepad controla a P2, Teclado controla a P1
        std::shared_ptr<Entidad> target = player;
        if (accion.getSource() == CE::Botones::InputSource::JoystickButton || 
            accion.getSource() == CE::Botones::InputSource::JoystickAxis) {
            // En modo PVC, el joystick NO controla a P2 (la IA lo hace)
            if (Globales::modo_juego == GameMode::PVC) return;
            target = p2;
        }

        if (!target) return;
        auto control = target->getComponente<CE::IControl>();
        if (!control) return;

        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {
                if(accion.getNombre() == "arriba")    control->arr = true;
                if(accion.getNombre() == "abajo")     control->abj = true;
                if(accion.getNombre() == "derecha")   control->der = true;
                if(accion.getNombre() == "izquierda") control->izq = true;
                if(accion.getNombre() == "correr")    control->run = true;
                if(accion.getNombre() == "atacar")    control->acc = true;
                if(accion.getNombre() == "especial")  control->sacc = true;
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                if(accion.getNombre() == "arriba")    control->arr = false;
                if(accion.getNombre() == "abajo")     control->abj = false;
                if(accion.getNombre() == "derecha")   control->der = false;
                if(accion.getNombre() == "izquierda") control->izq = false;
                if(accion.getNombre() == "correr")    control->run = false;
                if(accion.getNombre() == "atacar")    control->acc = false;
                if(accion.getNombre() == "especial")  control->sacc = false;
                break;
            }
            case CE::Botones::TipoAccion::Moved:
            {
                float pos = accion.getAxisPos();
                if (accion.getNombre() == "horizontal")
                {
                    control->der = (pos > 30.f);
                    control->izq = (pos < -30.f);
                }
                if (accion.getNombre() == "vertical")
                {
                    control->abj = (pos > 30.f);
                    control->arr = (pos < -30.f);
                }
                break;
            }
            case CE::Botones::TipoAccion::None: break;
        }
    }
    void Escena_Atlas::onRender()
    {
        //renderizamos los layers primero para
        //que se pinten atrás de todo
        for(auto& al: tiles_layers)
            CE::Render::Get().AddToDraw(al);

        for(auto& obj: objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);


#if DEBUG
        SistemaDibujarGolpe(player);
        if (p2) SistemaDibujarGolpe(p2);
#endif
        // --- DIBUJAR PARTÍCULAS DE MUERTE (Al final para que estén arriba) ---
        SistemaDibujarParticulasMuerte();

        // --- DIBUJAR CUENTA ATRÁS (UI OVERLAY) ---
        if (m_countdownState >= 0 && m_countdownText)
        {
            auto& tex = CE::Render::Get().GetTextura();
            sf::View oldView = tex.getView();
            sf::View uiView(sf::FloatRect({0.f, 0.f}, {1080.f, 720.f}));
            tex.setView(uiView);

            // Calcular escala (ease-out cubic)
            float totalDuration = (m_countdownState == 0) ? 0.8f : 1.0f;
            float t = 1.0f - (m_countdownTimeRemaining / totalDuration);
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;
            float scale = 2.5f - 1.5f * (1.0f - std::pow(1.0f - t, 3.0f));
            m_countdownText->setScale({scale, scale});

            // Calcular opacidad para el fade-out al final
            std::uint8_t alpha = 255;
            float fadeThreshold = (m_countdownState == 0) ? 0.3f : 0.2f;
            if (m_countdownTimeRemaining < fadeThreshold) {
                alpha = static_cast<std::uint8_t>((m_countdownTimeRemaining / fadeThreshold) * 255.f);
            }
            sf::Color color = m_countdownText->getFillColor();
            color.a = alpha;
            m_countdownText->setFillColor(color);

            // Sombra del texto para mejorar legibilidad
            sf::Text shadow = *m_countdownText;
            shadow.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(alpha * 0.5f)));
            shadow.move({6.f, 6.f});

            tex.draw(shadow);
            tex.draw(*m_countdownText);

            tex.setView(oldView);
        }
    }
}
