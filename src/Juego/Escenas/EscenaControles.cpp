#include "EscenaControles.hpp"
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <cmath>
#include <iostream>

namespace IVJ
{
    void EscenaControles::createJaggedBox(sf::ConvexShape& shape, sf::Vector2f size, float skew)
    {
        shape.setPointCount(5);
        shape.setPoint(0, sf::Vector2f(0, 0));
        shape.setPoint(1, sf::Vector2f(size.x + skew, 5));
        shape.setPoint(2, sf::Vector2f(size.x + 10, size.y - 5));
        shape.setPoint(3, sf::Vector2f(size.x / 2.0f, size.y + 10));
        shape.setPoint(4, sf::Vector2f(-skew, size.y - 10));
    }

    void EscenaControles::onInit()
    {
        if (!inicializar) return;

        sf::Font& font = CE::GestorAssets::Get().getFont("p5_font");
        
        background.setSize({1080, 720});
        background.setFillColor(P5_BLACK);

        title.setFont(font);
        title.setString("CONTROL SETTINGS");
        title.setCharacterSize(60);
        title.setFillColor(sf::Color::White);
        title.setPosition(50, 30);

        instruction.setFont(font);
        instruction.setString("Press Enter to change Keyboard | Press Y/X to change Joystick");
        instruction.setCharacterSize(24);
        instruction.setFillColor(sf::Color(255, 255, 255, 180));
        instruction.setPosition(50, 650);

        std::vector<std::pair<std::string, std::string>> actions = {
            {"arriba", "UP"},
            {"abajo", "DOWN"},
            {"izquierda", "LEFT"},
            {"derecha", "RIGHT"},
            {"correr", "RUN / DASH"},
            {"atacar", "ATTACK"},
            {"BACK", "RETURN TO MENU"}
        };

        for(size_t i=0; i<actions.size(); ++i) {
            ControlItem item;
            item.actionName = actions[i].first;
            item.displayName = actions[i].second;
            
            item.sfAction.setFont(font);
            item.sfAction.setString(item.displayName);
            item.sfAction.setCharacterSize(35);
            
            item.sfMapping.setFont(font);
            item.sfMapping.setCharacterSize(30);
            item.sfMapping.setFillColor(sf::Color(255, 255, 255, 200));

            createJaggedBox(item.box, {600, 60}, 20);
            item.box.setPosition({-700, 150.0f + i * 80.0f});
            
            controlItems.push_back(std::move(item));
        }

        updateMappingTexts();
        updatePositions();

        // Inputs para navegar
        registrarBotones(sf::Keyboard::Scancode::W, "up");
        registrarBotones(sf::Keyboard::Scancode::S, "down");
        registrarBotones(sf::Keyboard::Scancode::Enter, "select");
        registrarBotones(sf::Keyboard::Scancode::Escape, "back");

        for (int i = 0; i < 4; ++i) {
            registrarJoystickBoton(i, 0, "select");
            registrarJoystickBoton(i, 1, "select");
            registrarJoystickBoton(i, 2, "remap_joy"); // Y en Switch para remapear joy
            registrarJoystickBoton(i, 6, "back");
            registrarJoystickEje(i, sf::Joystick::Axis::Y, "vertical");
            registrarJoystickEje(i, sf::Joystick::Axis::PovY, "vertical");
        }

        inicializar = false;
    }

    void EscenaControles::updateMappingTexts()
    {
        // En una implementacion real, esto leeria de un config global
        // Por ahora, buscaremos en la escena EAtlas para mostrar lo que hay
        auto& atlas = CE::GestorEscenas::Get().getEscena("EAtlas");
        
        for (auto& item : controlItems) {
            if (item.actionName == "BACK") {
                item.sfMapping.setString("");
                continue;
            }

            std::string keys = "";
            for (auto const& [scan, act] : atlas.getBotones()) {
                if (act == item.actionName) {
                    keys += getScancodeName(scan) + " ";
                }
            }
            item.currentKey = keys.empty() ? "None" : keys;

            std::string joys = "";
            for (auto const& [pair, act] : atlas.joyBotones) {
                if (act == item.actionName) {
                    joys += "B" + std::to_string(pair.second) + " ";
                }
            }
            item.currentJoy = joys.empty() ? "None" : joys;

            item.sfMapping.setString("KB: " + item.currentKey + " | JOY: " + item.currentJoy);
        }
    }

    std::string EscenaControles::getScancodeName(sf::Keyboard::Scancode code)
    {
        // Simplificacion para el ejemplo
        if (code >= sf::Keyboard::Scancode::A && code <= sf::Keyboard::Scancode::Z)
            return std::string(1, 'A' + (int)code - (int)sf::Keyboard::Scancode::A);
        if (code == sf::Keyboard::Scancode::W) return "W";
        if (code == sf::Keyboard::Scancode::S) return "S";
        if (code == sf::Keyboard::Scancode::A) return "A";
        if (code == sf::Keyboard::Scancode::D) return "D";
        if (code == sf::Keyboard::Scancode::Up) return "UP";
        if (code == sf::Keyboard::Scancode::Down) return "DOWN";
        if (code == sf::Keyboard::Scancode::Left) return "LEFT";
        if (code == sf::Keyboard::Scancode::Right) return "RIGHT";
        if (code == sf::Keyboard::Scancode::LShift) return "LSHIFT";
        if (code == sf::Keyboard::Scancode::J) return "J";
        return "Key " + std::to_string((int)code);
    }

    void EscenaControles::updatePositions()
    {
        float startY = 130.0f;
        float spacing = 75.0f;

        for(size_t i=0; i<controlItems.size(); ++i) {
            float targetX = (i == (size_t)selectedIndex) ? 100.0f : 60.0f;
            controlItems[i].targetPos = {targetX, startY + i * spacing};
            
            controlItems[i].box.setFillColor((i == (size_t)selectedIndex) ? sf::Color::White : P5_RED);
            controlItems[i].sfAction.setFillColor((i == (size_t)selectedIndex) ? P5_BLACK : sf::Color::White);
            
            if (listeningForKey && i == (size_t)selectedIndex) {
                controlItems[i].box.setFillColor(sf::Color::Yellow);
            }
            if (listeningForJoy && i == (size_t)selectedIndex) {
                controlItems[i].box.setFillColor(sf::Color::Cyan);
            }
        }
    }

    void EscenaControles::onUpdate(float dt)
    {
        timer += dt;
        for(size_t i=0; i<controlItems.size(); ++i) {
            sf::Vector2f currentPos = controlItems[i].box.getPosition();
            sf::Vector2f diff = controlItems[i].targetPos - currentPos;
            controlItems[i].box.setPosition(currentPos + diff * 10.0f * dt);
            
            float wobble = std::sin(timer * 4.0f + i) * 1.5f;
            controlItems[i].box.setRotation(sf::degrees(wobble));
            
            controlItems[i].sfAction.setPosition(controlItems[i].box.getPosition() + sf::Vector2f(20, 5));
            controlItems[i].sfMapping.setPosition(controlItems[i].box.getPosition() + sf::Vector2f(250, 10));
        }
    }

    void EscenaControles::onInputs(const CE::Botones& accion)
    {
        if (listeningForKey || listeningForJoy) return; // Se maneja en el loop de eventos crudos si fuera necesario, o esperamos un input especifico

        if (accion.getTipo() == CE::Botones::TipoAccion::OnPress) {
            if (accion.getNombre() == "up") {
                selectedIndex = (selectedIndex - 1 + controlItems.size()) % controlItems.size();
                updatePositions();
            } else if (accion.getNombre() == "down") {
                selectedIndex = (selectedIndex + 1) % controlItems.size();
                updatePositions();
            } else if (accion.getNombre() == "select") {
                if (controlItems[selectedIndex].actionName == "BACK") {
                    CE::GestorEscenas::Get().cambiarEscena("Menu3");
                } else {
                    // Aqui empezaria la logica de remapeo (escuchar siguiente tecla)
                    // Por ahora solo mostraremos visualmente el feedback
                    listeningForKey = true;
                    instruction.setString("Press any KEY to assign to " + controlItems[selectedIndex].displayName);
                    updatePositions();
                }
            } else if (accion.getNombre() == "remap_joy") {
                listeningForJoy = true;
                instruction.setString("Press any BUTTON on Controller to assign to " + controlItems[selectedIndex].displayName);
                updatePositions();
            } else if (accion.getNombre() == "back") {
                CE::GestorEscenas::Get().cambiarEscena("Menu3");
            }
        } else if (accion.getTipo() == CE::Botones::TipoAccion::Moved) {
            float pos = accion.getAxisPos();
            if (accion.getNombre() == "vertical") {
                if (std::abs(pos) > 50.f) {
                    if (!axisInUse) {
                        if (pos > 0.f) selectedIndex = (selectedIndex + 1) % controlItems.size();
                        else selectedIndex = (selectedIndex - 1 + controlItems.size()) % controlItems.size();
                        updatePositions();
                        axisInUse = true;
                    }
                } else {
                    axisInUse = false;
                }
            }
        }
    }

    void EscenaControles::onRender()
    {
        CE::Render::Get().AddToDraw(background);
        CE::Render::Get().AddToDraw(title);
        CE::Render::Get().AddToDraw(instruction);

        for(auto& item : controlItems) {
            CE::Render::Get().AddToDraw(item.box);
            CE::Render::Get().AddToDraw(item.sfAction);
            CE::Render::Get().AddToDraw(item.sfMapping);
        }
        
        // --- LOGICA DE REMAPEO (SIMULADA PARA DEMO) ---
        // En una implementacion real, el InputBuffer deberia filtrar o el motor avisar
        // Para este prototipo, si estamos escuchando, capturamos el proximo evento de tecla en el proximo frame
        if (listeningForKey) {
            for (int k = 0; k < (int)sf::Keyboard::Scancode::ScancodeCount; ++k) {
                if (sf::Keyboard::isKeyPressed((sf::Keyboard::Scancode)k)) {
                    // Evitar capturar Enter/Select de inmediato
                    if (k == (int)sf::Keyboard::Scancode::Enter) continue;
                    
                    std::string act = controlItems[selectedIndex].actionName;
                    auto& atlas = CE::GestorEscenas::Get().getEscena("EAtlas");
                    
                    // Limpiar mapeos anteriores para esta accion (opcional)
                    // atlas.getBotones().clear(); // No, eso borra todo
                    atlas.registrarBotones((sf::Keyboard::Scancode)k, act);
                    
                    listeningForKey = false;
                    instruction.setString("Press Enter to change Keyboard | Press Y/X to change Joystick");
                    updateMappingTexts();
                    updatePositions();
                    break;
                }
            }
        }
        
        if (listeningForJoy) {
            for (unsigned int b = 0; b < 16; ++b) {
                if (sf::Joystick::isButtonPressed(0, b)) {
                    // Evitar capturar el mismo boton que inicio el remap
                    if (b == 2) continue; 

                    std::string act = controlItems[selectedIndex].actionName;
                    auto& atlas = CE::GestorEscenas::Get().getEscena("EAtlas");
                    
                    for(int i=0; i<4; ++i)
                        atlas.registrarJoystickBoton(i, b, act);
                    
                    listeningForJoy = false;
                    instruction.setString("Press Enter to change Keyboard | Press Y/X to change Joystick");
                    updateMappingTexts();
                    updatePositions();
                    break;
                }
            }
        }
    }

    void EscenaControles::onFinal() {}
}
