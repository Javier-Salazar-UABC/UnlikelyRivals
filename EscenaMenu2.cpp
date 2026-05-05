#include "EscenaMenu2.hpp"
#include "../../Motor/Primitivos/GestorEscenas.hpp"
#include "../../Motor/Render/Render.hpp"
#include "../../Motor/Primitivos/GestorAssets.hpp"
#include <cmath>

namespace IVJ
{
    void EscenaMenu2::createJaggedBox(sf::ConvexShape& shape, sf::Vector2f size, float skew)
    {
        shape.setPointCount(5);
        shape.setPoint(0, sf::Vector2f(0, 0));
        shape.setPoint(1, sf::Vector2f(size.x + skew, 5));
        shape.setPoint(2, sf::Vector2f(size.x + 10, size.y - 5));
        shape.setPoint(3, sf::Vector2f(size.x / 2.0f, size.y + 10)); // Extra point for jaggedness
        shape.setPoint(4, sf::Vector2f(-skew, size.y - 10));
    }

    void EscenaMenu2::onInit()
    {
        if (!inicializar) return;

        const std::string ASSETS_PATH = "../../assets/";
        
        // Register required assets before setup
        CE::GestorAssets::Get().agregarFont("p5_font", ASSETS_PATH + "Fonts/Arial.otf");
        CE::GestorAssets::Get().agregarTextura("p5_player", ASSETS_PATH + "sprites/Characters/Player.png", {100, 100}, {100, 100});

        setupBackground();
        setupDecorations();
        setupMenu();

        currentItems = &mainItems;

        registrarBotones(sf::Keyboard::Scancode::W, "up");
        registrarBotones(sf::Keyboard::Scancode::S, "down");
        registrarBotones(sf::Keyboard::Scancode::Enter, "select");
        registrarBotones(sf::Keyboard::Scancode::Escape, "exit");

        updatePositions();
        inicializar = false;
    }

    void EscenaMenu2::setupBackground()
    {
        background.setSize({1080, 720});
        background.setFillColor(P5_BLACK);

        // --- Clean Ordered Dithering System ---
        for(int x=0; x<1080; x+=15) {
            for(int y=0; y<720; y+=15) {
                float threshold = (float)(x + y) / (1080 + 720);
                bool draw = false;
                float size = 3.0f;

                if (threshold > 0.8f) { draw = true; size = 6.0f; }
                else if (threshold > 0.6f && (x%30==0 && y%30==0)) { draw = true; size = 4.0f; }
                else if (threshold > 0.4f && (x%45==0 && y%45==0)) { draw = true; size = 2.0f; }

                if (draw) {
                    sf::ConvexShape dot;
                    dot.setPointCount(4);
                    dot.setPoint(0, {0, 0});
                    dot.setPoint(1, {size, 0});
                    dot.setPoint(2, {size, size});
                    dot.setPoint(3, {0, size});
                    dot.setPosition({(float)x, (float)y});
                    dot.setFillColor(sf::Color(255, 0, 0, 40)); // Brighter Red dots
                    bgDecorations.push_back(dot);
                }
            }
        }
    }

    void EscenaMenu2::setupDecorations()
    {
        sf::Font& font = CE::GestorAssets::Get().getFont("p5_font");

        // Selection Stripe
        selectionStripe.setSize({2000.0f, 80.0f});
        selectionStripe.setFillColor(sf::Color(211, 0, 0, 100));
        selectionStripe.setRotation(sf::degrees(-10.0f));
        selectionStripe.setOrigin({1000.0f, 40.0f});

        // Floating Background Text
        std::vector<std::string> bgWords = {"1", "2", "3", "PLAY"};
        for(int i=0; i<6; ++i) {
            auto txt = std::make_unique<sf::Text>(font);
            txt->setString(bgWords[rand()%bgWords.size()]);
            txt->setCharacterSize(60);
            txt->setFillColor(sf::Color(255, 255, 255, 15)); 
            txt->setRotation(sf::degrees(-15.0f));
            txt->setPosition({(float)(rand()%1000), (float)(rand()%700)});
            bgFloatingTexts.push_back(std::move(txt));
        }

        // Glitch Lines
        for(int i=0; i<15; ++i) {
            sf::ConvexShape line;
            createJaggedBox(line, {2000, 3}, 0);
            line.setFillColor(sf::Color(255, 255, 255, 25));
            line.setPosition({-500, (float)(rand()%720)});
            line.setRotation(sf::degrees(-25.0f));
            bgDecorations.push_back(line);
        }

        // Character Silhouette
        {
            sf::Texture& silTexture = CE::GestorAssets::Get().getTextura("p5_player");
            silhouette = std::make_unique<sf::Sprite>(silTexture);
            auto bounds = silhouette->getLocalBounds();
            silhouette->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
            silhouette->setPosition({850, 400});
            silhouette->setScale({30.0f, 30.0f}); 
            silhouette->setColor(sf::Color(255, 255, 255, 120)); 
        }
    }

    void EscenaMenu2::setupMenu()
    {
        sf::Font& font = CE::GestorAssets::Get().getFont("p5_font");

        std::vector<std::string> options = {"PLAY", "LOAD GAME", "CONFIG", "LEAVE"};
        for(size_t i=0; i<options.size(); ++i) {
            MenuItem item;
            item.text = options[i];
            item.sfText = std::make_unique<sf::Text>(font);
            item.sfText->setString(item.text);
            item.sfText->setCharacterSize(45);
            item.sfText->setStyle(sf::Text::Bold);
            createJaggedBox(item.box, {350, 70}, 30);
            item.rotation = -8.0f + (static_cast<float>(rand() % 16));
            item.box.setPosition({-500, 200}); 
            mainItems.push_back(std::move(item));
        }

        std::vector<std::string> sOptions = {"VIDEO", "AUDIO", "CONTROLS", "BACK"};
        for(size_t i=0; i<sOptions.size(); ++i) {
            MenuItem item;
            item.text = sOptions[i];
            item.sfText = std::make_unique<sf::Text>(font);
            item.sfText->setString(item.text);
            item.sfText->setCharacterSize(45);
            item.sfText->setStyle(sf::Text::Bold);
            createJaggedBox(item.box, {350, 70}, 30);
            item.rotation = -8.0f + (static_cast<float>(rand() % 16));
            item.box.setPosition({1500, 200}); 
            settingsItems.push_back(std::move(item));
        }
    }

    void EscenaMenu2::updatePositions()
    {
        float startY = 150.0f;
        float spacing = 100.0f;

        // Position Main Items
        for(size_t i=0; i<mainItems.size(); ++i) {
            if (currentState == MenuState::MAIN) {
                float targetX = (i == (size_t)selectedIndex) ? 150.0f : 100.0f;
                mainItems[i].targetPos = {targetX, startY + i * spacing};
                mainItems[i].box.setFillColor((i == (size_t)selectedIndex) ? sf::Color::White : P5_RED);
                if (mainItems[i].sfText) mainItems[i].sfText->setFillColor((i == (size_t)selectedIndex) ? P5_BLACK : sf::Color::White);
            } else {
                mainItems[i].targetPos = {-500.0f, startY + i * spacing}; // Slide out left
            }
        }

        // Position Settings Items
        for(size_t i=0; i<settingsItems.size(); ++i) {
            if (currentState == MenuState::SETTINGS) {
                float targetX = (i == (size_t)selectedIndex) ? 150.0f : 100.0f;
                settingsItems[i].targetPos = {targetX, startY + i * spacing};
                settingsItems[i].box.setFillColor((i == (size_t)selectedIndex) ? sf::Color::White : P5_RED);
                if (settingsItems[i].sfText) settingsItems[i].sfText->setFillColor((i == (size_t)selectedIndex) ? P5_BLACK : sf::Color::White);
            } else {
                settingsItems[i].targetPos = {1500.0f, startY + i * spacing}; // Slide out right
            }
        }
    }

    void EscenaMenu2::onUpdate(float dt)
    {
        timer += dt;

        auto animateItems = [&](std::vector<MenuItem>& items) {
            for(size_t i=0; i<items.size(); ++i) {
                sf::Vector2f currentPos = items[i].box.getPosition();
                sf::Vector2f diff = items[i].targetPos - currentPos;
                items[i].box.setPosition(currentPos + diff * 12.0f * dt);
                
                float wobble = std::sin(timer * 5.0f + i) * 2.0f;
                items[i].box.setRotation(sf::degrees(items[i].rotation + wobble));
                
                if (items[i].sfText) {
                    items[i].sfText->setPosition(items[i].box.getPosition() + sf::Vector2f(20, 5));
                    items[i].sfText->setRotation(items[i].box.getRotation());
                }
            }
        };

        animateItems(mainItems);
        animateItems(settingsItems);

        // --- DYNAMIC BACKGROUND SYSTEM ---
        
        // Update Stripe position to follow selected item
        if (currentItems && selectedIndex < (int)currentItems->size()) {
            sf::Vector2f targetPos = (*currentItems)[selectedIndex].box.getPosition();
            sf::Vector2f currentStripePos = selectionStripe.getPosition();
            selectionStripe.setPosition({currentStripePos.x + (targetPos.x + 100 - currentStripePos.x) * 8.0f * dt, 
                                        currentStripePos.y + (targetPos.y + 35 - currentStripePos.y) * 8.0f * dt});
        }

        // Update Floating Texts
        for(auto& txt : bgFloatingTexts) {
            txt->move({-20.0f * dt, 0}); // Drift left
            if (txt->getPosition().x < -200) txt->setPosition({1200, (float)(rand()%700)});
        }

        // Update Particles (More glitchy)
        for(auto& p : particles) {
            p.shape.move(p.velocity * dt);
            if (rand()%100 == 0) p.shape.move({(float)(rand()%20-10), 0}); // Jitter
            p.shape.rotate(sf::degrees(p.rotationSpeed * dt));
            
            sf::Vector2f pos = p.shape.getPosition();
            if (pos.x < -50) p.shape.setPosition({1130, pos.y});
            if (pos.x > 1130) p.shape.setPosition({-50, pos.y});
            if (pos.y < -50) p.shape.setPosition({pos.x, 770});
            if (pos.y > 770) p.shape.setPosition({pos.x, -50});
        }

        // Animate Silhouette (Big Portrait)
        if (silhouette) {
            float hover = std::sin(timer * 1.0f) * 15.0f;
            float shiftX = (currentState == MenuState::MAIN) ? 0.0f : -200.0f;
            sf::Vector2f currentPos = silhouette->getPosition();
            silhouette->setPosition({currentPos.x + (900.0f + shiftX - currentPos.x) * 4.0f * dt, 500.0f + hover});
        }
    }

    void EscenaMenu2::onInputs(const CE::Botones& accion)
    {
        if (accion.getTipo() == CE::Botones::TipoAccion::OnPress) {
            if (accion.getNombre() == "up") {
                selectedIndex = (selectedIndex - 1 + currentItems->size()) % currentItems->size();
                updatePositions();
            } else if (accion.getNombre() == "down") {
                selectedIndex = (selectedIndex + 1) % currentItems->size();
                updatePositions();
            } else if (accion.getNombre() == "select") {
                MenuItem& selected = (*currentItems)[selectedIndex];
                
                if (currentState == MenuState::MAIN) {
                    if (selected.text == "PLAY") {
                        CE::GestorEscenas::Get().cambiarEscena("House");
                    } else if (selected.text == "LOAD GAME") {
                        CE::GestorEscenas::Get().cambiarEscena("Juego");
                    } else if (selected.text == "CONFIG") {
                        currentState = MenuState::SETTINGS;
                        currentItems = &settingsItems;
                        selectedIndex = 0;
                        updatePositions();
                    } else if (selected.text == "LEAVE") {
                        CE::Render::Get().GetVentana().close();
                    }
                } else if (currentState == MenuState::SETTINGS) {
                    if (selected.text == "BACK") {
                        currentState = MenuState::MAIN;
                        currentItems = &mainItems;
                        selectedIndex = 2; // Return to 'SYSTEM' option
                        updatePositions();
                    }
                    // Add other settings logic here
                }
            } else if (accion.getNombre() == "exit") {
                if (currentState != MenuState::MAIN) {
                    currentState = MenuState::MAIN;
                    currentItems = &mainItems;
                    selectedIndex = 0;
                    updatePositions();
                } else {
                    CE::Render::Get().GetVentana().close();
                }
            }
        }
    }

    void EscenaMenu2::onRender()
    {
        CE::Render::Get().AddToDraw(background);
        for(auto& shape : bgDecorations) {
            CE::Render::Get().AddToDraw(shape);
        }

        for(auto& txt : bgFloatingTexts) {
            if (txt) CE::Render::Get().AddToDraw(*txt);
        }

        CE::Render::Get().AddToDraw(selectionStripe);

        if (silhouette) CE::Render::Get().AddToDraw(*silhouette);

        for(auto& p : particles) {
            CE::Render::Get().AddToDraw(p.shape);
        }
        
        for(auto& item : mainItems) {
            CE::Render::Get().AddToDraw(item.box);
            if (item.sfText) CE::Render::Get().AddToDraw(*item.sfText);
        }
        for(auto& item : settingsItems) {
            CE::Render::Get().AddToDraw(item.box);
            if (item.sfText) CE::Render::Get().AddToDraw(*item.sfText);
        }
    }

    void EscenaMenu2::onFinal() {}
}
