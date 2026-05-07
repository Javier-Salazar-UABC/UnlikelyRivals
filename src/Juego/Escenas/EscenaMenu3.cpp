#include "EscenaMenu3.hpp"
#include <Motor/Primitivos/GestorEscenas.hpp>
#include <Motor/Render/Render.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <cmath>
#include "../Globales.hpp"


namespace IVJ
{
    void EscenaMenu3::createJaggedBox(sf::ConvexShape& shape, sf::Vector2f size, float skew)
    {
        shape.setPointCount(5);
        shape.setPoint(0, sf::Vector2f(0, 0));
        shape.setPoint(1, sf::Vector2f(size.x + skew, 5));
        shape.setPoint(2, sf::Vector2f(size.x + 10, size.y - 5));
        shape.setPoint(3, sf::Vector2f(size.x / 2.0f, size.y + 10));
        shape.setPoint(4, sf::Vector2f(-skew, size.y - 10));
    }

    void EscenaMenu3::onInit()
    {
        if (!inicializar) return;

        CE::GestorAssets::Get().agregarFont("p5_font", ASSETS "/fonts/Electrolize.ttf");
        CE::GestorAssets::Get().agregarTextura("p5_player", ASSETS "/sprites/esnupi_walk.png", {0, 0}, {0, 0});
        CE::GestorAssets::Get().agregarTextura("p5_jeilo",  ASSETS "/sprites/jeilo_walk.png", {0, 0}, {0, 0});
        CE::GestorAssets::Get().agregarTextura("p5_goku",   ASSETS "/sprites/goku_walk.png", {0, 0}, {0, 0});


        setupBackground();
        setupDecorations();
        setupMenu();
        setupCharacterSelect();


        currentItems = &mainItems;

        registrarBotones(sf::Keyboard::Scancode::W, "up");
        registrarBotones(sf::Keyboard::Scancode::S, "down");
        registrarBotones(sf::Keyboard::Scancode::Enter, "select");
        registrarBotones(sf::Keyboard::Scancode::Escape, "exit");

        // --- Mapeo de Control ---
        for (int i = 0; i < 4; ++i)
        {
            registrarJoystickBoton(i, 0, "select");
            registrarJoystickBoton(i, 1, "select");
            registrarJoystickBoton(i, 2, "exit");
            registrarJoystickBoton(i, 3, "select");
            registrarJoystickBoton(i, 9, "select"); // Start en muchos controles
            registrarJoystickEje(i, sf::Joystick::Axis::Y, "vertical");
            registrarJoystickEje(i, sf::Joystick::Axis::PovY, "vertical");
        }

        updatePositions();
        
        if (!crepuscularShader.loadFromFile(ASSETS "/shaders/crepuscular.frag", sf::Shader::Type::Fragment)) {
            // Manejar error si falla la carga
        }
        
        sf::Vector2u winSize = CE::Render::Get().GetVentana().getSize();
        crepuscularShader.setUniform("resolution", sf::Vector2f((float)winSize.x, (float)winSize.y));
        
        if (!occlusionTexture.resize(winSize)) {
            // Manejar error
        }
        // Asegurar que la vista de la textura de oclusion coincida con la de la ventana
        sf::View view(sf::FloatRect({0, 0}, {(float)winSize.x, (float)winSize.y}));
        occlusionTexture.setView(view);

        inicializar = false;
    }

    void EscenaMenu3::setupBackground()
    {
        sf::Vector2u winSize = CE::Render::Get().GetVentana().getSize();
        background.setSize({(float)winSize.x, (float)winSize.y});
        background.setFillColor(P5_BLACK);

        // Efecto dithering
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
                    dot.setFillColor(sf::Color(255, 0, 0, 40)); // Puntos mas brillantes
                    bgDecorations.push_back(dot);
                }
            }
        }
    }

    void EscenaMenu3::setupDecorations()
    {
        sf::Font& font = CE::GestorAssets::Get().getFont("p5_font");

        // Linea de Seleccion
        selectionStripe.setSize({2000.0f, 80.0f});
        selectionStripe.setFillColor(sf::Color(211, 0, 0, 100));
        selectionStripe.setRotation(sf::degrees(-10.0f));
        selectionStripe.setOrigin({1000.0f, 40.0f});

        // Texto flotante del background
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

        // Lineas glitcheadas
        for(int i=0; i<15; ++i) {
            sf::ConvexShape line;
            createJaggedBox(line, {2000, 3}, 0);
            line.setFillColor(sf::Color(255, 255, 255, 25));
            line.setPosition({-500, (float)(rand()%720)});
            line.setRotation(sf::degrees(-25.0f));
            bgDecorations.push_back(line);
        }

        // Personaje en el menu
        {
            sf::Texture& silTexture = CE::GestorAssets::Get().getTextura("p5_player");
            silTexture.setSmooth(false); // Pixel art style
            silhouette = std::make_unique<sf::Sprite>(silTexture);
            silhouette->setTextureRect(sf::IntRect({0, 0}, {20, 20}));
            silhouette->setOrigin({10.0f, 10.0f});
            silhouette->setPosition({850, 400});
            silhouette->setScale({30.0f, 30.0f}); 
            silhouette->setColor(sf::Color(255, 255, 255,255)); 
        }

        // --- TITULO DEL JUEGO ---
        {
            titleText = std::make_unique<sf::Text>(font);
            titleText->setString("UNLIKELY RIVALS");
            titleText->setCharacterSize(100);
            titleText->setStyle(sf::Text::Bold | sf::Text::Italic);
            titleText->setFillColor(sf::Color::White);
            titleText->setOutlineColor(P5_RED);
            titleText->setOutlineThickness(5.0f);
            titleText->setRotation(sf::degrees(-5.0f));
            titleText->setPosition({100, 30});
        }
    }

    void EscenaMenu3::setupMenu()
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

    void EscenaMenu3::setupCharacterSelect()
    {
        sf::Font& font = CE::GestorAssets::Get().getFont("p5_font");
        std::vector<std::string> chars = {"SNOOPY", "MASTER CHIEF", "GOKU", "CANCEL"};
        
        for(size_t i=0; i<chars.size(); ++i) {
            MenuItem item;
            item.text = chars[i];
            item.sfText = std::make_unique<sf::Text>(font);
            item.sfText->setString(item.text);
            item.sfText->setCharacterSize(45);
            item.sfText->setStyle(sf::Text::Bold);
            createJaggedBox(item.box, {400, 70}, 30);
            item.rotation = -5.0f + (static_cast<float>(rand() % 10));
            item.box.setPosition({-600, 200}); 
            characterItems.push_back(std::move(item));
        }
    }

    void EscenaMenu3::updatePositions()
    {
        float startY = 150.0f;
        float spacing = 100.0f;

        // Posiciones de los items principales
        for(size_t i=0; i<mainItems.size(); ++i) {
            if (currentState == MenuState::MAIN) {
                float targetX = (i == (size_t)selectedIndex) ? 150.0f : 100.0f;
                mainItems[i].targetPos = {targetX, startY + i * spacing};
                mainItems[i].box.setFillColor((i == (size_t)selectedIndex) ? sf::Color::White : P5_RED);
                if (mainItems[i].sfText) mainItems[i].sfText->setFillColor((i == (size_t)selectedIndex) ? P5_BLACK : sf::Color::White);
            } else {
                mainItems[i].targetPos = {-500.0f, startY + i * spacing}; // Deslizarse a la izquierda
            }
        }

        // pOSICIONES DE LOS ITEMS De los Settings
        for(size_t i=0; i<settingsItems.size(); ++i) {
            if (currentState == MenuState::SETTINGS) {
                float targetX = (i == (size_t)selectedIndex) ? 150.0f : 100.0f;
                settingsItems[i].targetPos = {targetX, startY + i * spacing};
                settingsItems[i].box.setFillColor((i == (size_t)selectedIndex) ? sf::Color::White : P5_RED);
                if (settingsItems[i].sfText) settingsItems[i].sfText->setFillColor((i == (size_t)selectedIndex) ? P5_BLACK : sf::Color::White);
            } else {
                settingsItems[i].targetPos = {1500.0f, startY + i * spacing}; // Deslizarse hacia la derecha
            }
        }

        // Posiciones de los items de Personajes
        for(size_t i=0; i<characterItems.size(); ++i) {
            if (currentState == MenuState::CHAR_SELECT) {
                float targetX = (i == (size_t)selectedIndex) ? 150.0f : 100.0f;
                characterItems[i].targetPos = {targetX, startY + i * spacing};
                characterItems[i].box.setFillColor((i == (size_t)selectedIndex) ? sf::Color::White : P5_RED);
                if (characterItems[i].sfText) characterItems[i].sfText->setFillColor((i == (size_t)selectedIndex) ? P5_BLACK : sf::Color::White);
            } else {
                characterItems[i].targetPos = {-700.0f, startY + i * spacing};
            }
        }
    }


    void EscenaMenu3::onUpdate(float dt)
    {
        timer += dt;
        if (joyTimer > 0) joyTimer -= dt;

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
        animateItems(characterItems);


        // --- Background Dynamimco ---
        
        // Update Stripe position to follow selected item
        if (currentItems && selectedIndex < (int)currentItems->size()) {
            sf::Vector2f targetPos = (*currentItems)[selectedIndex].box.getPosition();
            sf::Vector2f currentStripePos = selectionStripe.getPosition();
            selectionStripe.setPosition({currentStripePos.x + (targetPos.x + 100 - currentStripePos.x) * 8.0f * dt, 
                                        currentStripePos.y + (targetPos.y + 35 - currentStripePos.y) * 8.0f * dt});
        }

        // Update Textos flotantes
        for(auto& txt : bgFloatingTexts) {
            txt->move({-20.0f * dt, 0}); // Deslizarse a la izquierda
            if (txt->getPosition().x < -200) txt->setPosition({1200, (float)(rand()%700)});
        }

        // Update Particulas
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

        // --- Animacion de Silueta (Dinámica) ---
        if (silhouette) {
            // Cambio de personaje en hover
            if (currentState == MenuState::CHAR_SELECT && selectedIndex != lastCharIndex && selectedIndex < 3) {
                lastCharIndex = selectedIndex;
                
                // Efecto Slide: lo movemos fuera de pantalla a la derecha
                // el lerp de abajo lo traerá de vuelta suavemente
                silhouette->setPosition({1200.0f, 400.0f});
                
                if (selectedIndex == 0) { // Snoopy
                    silhouette->setTexture(CE::GestorAssets::Get().getTextura("p5_player"));
                    frameW = 20; frameH = 20;
                } else if (selectedIndex == 1) { // Master Chief
                    silhouette->setTexture(CE::GestorAssets::Get().getTextura("p5_jeilo"));
                    frameW = 16; frameH = 32;
                } else if (selectedIndex == 2) { // Goku
                    silhouette->setTexture(CE::GestorAssets::Get().getTextura("p5_goku"));
                    frameW = 16; frameH = 32;
                }
                silhouette->setTextureRect(sf::IntRect({0, 0}, {frameW, frameH}));
                silhouette->setOrigin({(float)frameW/2.0f, (float)frameH/2.0f});
            } else if (currentState != MenuState::CHAR_SELECT && lastCharIndex != -2) {
                // Reset a Snoopy si salimos de selección
                lastCharIndex = -2;
                silhouette->setTexture(CE::GestorAssets::Get().getTextura("p5_player"));
                frameW = 20; frameH = 20;
                silhouette->setTextureRect(sf::IntRect({0, 0}, {frameW, frameH}));
                silhouette->setOrigin({10.0f, 10.0f});
            }

            // Animación de frames
            animTimer += dt;
            if (animTimer >= FRAME_TIME) {
                animTimer = 0.0f;
                currentFrame = (currentFrame + 1) % TOTAL_FRAMES;
                silhouette->setTextureRect(sf::IntRect({currentFrame * frameW, 0}, {frameW, frameH}));
            }
            
            // Proporciones visuales
            float finalScale = 30.0f;
            if (frameH > 20) finalScale = 20.0f; 

            float hover = std::sin(timer * 1.5f) * 10.0f;
            float tilt = std::sin(timer * 2.0f) * 2.0f;
            float shiftX = (currentState == MenuState::MAIN) ? 0.0f : -150.0f;
            
            sf::Vector2f currentPos = silhouette->getPosition();
            // Deslizamiento suave (Lerp)
            float targetX = 900.0f + shiftX;
            silhouette->setPosition({currentPos.x + (targetX - currentPos.x) * 8.0f * dt, 400.0f + hover});
            silhouette->setScale({finalScale, finalScale});
            silhouette->setRotation(sf::degrees(-10.0f + tilt));
        }



        // --- Actualizar Shader ---
        crepuscularShader.setUniform("time", timer);
        sf::Vector2i mousePos = sf::Mouse::getPosition(CE::Render::Get().GetVentana());
        float winH = (float)CE::Render::Get().GetVentana().getSize().y;
        crepuscularShader.setUniform("mouse", sf::Vector2f((float)mousePos.x, winH - (float)mousePos.y));
    }

    void EscenaMenu3::onInputs(const CE::Botones& accion)
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
                        currentState = MenuState::CHAR_SELECT;
                        currentItems = &characterItems;
                        selectedIndex = 0;
                        selectionStep = 0; // Reset a selección P1
                        if (titleText) titleText->setString("SELECT P1");
                        updatePositions();
                    } else if (selected.text == "LOAD GAME") {
                        CE::GestorEscenas::Get().cambiarEscena("EAtlas");
                    } else if (selected.text == "CONFIG") {
                        currentState = MenuState::SETTINGS;
                        currentItems = &settingsItems;
                        selectedIndex = 0;
                        updatePositions();
                    } else if (selected.text == "LEAVE") {
                        CE::Render::Get().GetVentana().close();
                    }
                } else if (currentState == MenuState::CHAR_SELECT) {
                    if (selected.text == "CANCEL") {
                        currentState = MenuState::MAIN;
                        currentItems = &mainItems;
                        selectedIndex = 0;
                        if (titleText) titleText->setString("UNLIKELY RIVALS");
                        updatePositions();
                    } else {
                        // Mapeo de selección
                        PersonajeID id = PersonajeID::SNOOPY;
                        if (selected.text == "MASTER CHIEF") id = PersonajeID::MASTER_CHIEF;
                        if (selected.text == "GOKU") id = PersonajeID::GOKU;

                        if (selectionStep == 0) {
                            Globales::p1_seleccionado = id;
                            selectionStep = 1;
                            selectedIndex = 0;
                            if (titleText) titleText->setString("SELECT P2");
                            updatePositions();
                        } else {
                            Globales::p2_seleccionado = id;
                            CE::GestorEscenas::Get().cambiarEscena("EAtlas");
                        }
                    }
                } else if (currentState == MenuState::SETTINGS) {

                    if (selected.text == "BACK") {
                        currentState = MenuState::MAIN;
                        currentItems = &mainItems;
                        selectedIndex = 2; // Retornar al menu
                        updatePositions();
                    }
                    // Futuros settings de ser necesario
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
        else if (accion.getTipo() == CE::Botones::TipoAccion::Moved)
        {
            if (joyTimer <= 0.0f)
            {
                float pos = accion.getAxisPos();
                if (accion.getNombre() == "vertical")
                {
                    if (std::abs(pos) > 50.f)
                    {
                        if (pos > 0) selectedIndex = (selectedIndex + 1) % currentItems->size();
                        else selectedIndex = (selectedIndex - 1 + currentItems->size()) % currentItems->size();
                        
                        updatePositions();
                        joyTimer = 0.25f; // Cooldown de 250ms
                    }
                }
            }
        }
    }

    void EscenaMenu3::onRender()
    {
        // --- Generar Mapa de Oclusion ---
        occlusionTexture.clear(sf::Color::Black);
        
        // Dibujamos los cuadros de los items como oclusores (en blanco)
        if (currentItems) {
            for(auto& item : *currentItems) {
                sf::Color oldColor = item.box.getFillColor();
                item.box.setFillColor(sf::Color::White);
                occlusionTexture.draw(item.box);
                item.box.setFillColor(oldColor);
            }
        }
        
        // El personaje tambien puede obstruir la luz
        if (silhouette) {
            sf::Color oldColor = silhouette->getColor();
            silhouette->setColor(sf::Color::White);
            occlusionTexture.draw(*silhouette);
            silhouette->setColor(oldColor);
        }
        
        occlusionTexture.display();
        
        // Pasar el mapa al shader
        crepuscularShader.setUniform("occlusionMap", occlusionTexture.getTexture());

        CE::Render::Get().AddToDraw(background, &crepuscularShader);
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
        for(auto& item : characterItems) {
            CE::Render::Get().AddToDraw(item.box);
            if (item.sfText) CE::Render::Get().AddToDraw(*item.sfText);
        }


        if (titleText) CE::Render::Get().AddToDraw(*titleText);
    }

    void EscenaMenu3::onFinal() {}
}
