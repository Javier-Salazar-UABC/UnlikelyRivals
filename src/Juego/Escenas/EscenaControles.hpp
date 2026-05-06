#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace IVJ
{
    class EscenaControles : public CE::Escena
    {
    public:
        EscenaControles() : CE::Escena() {};
        ~EscenaControles() override = default;

        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;

    private:
        struct ControlItem {
            std::string actionName;
            std::string displayName;
            std::string currentKey;
            std::string currentJoy;
            sf::Text sfAction;
            sf::Text sfMapping;
            sf::ConvexShape box;
            sf::Vector2f targetPos;
        };

        void updatePositions();
        void createJaggedBox(sf::ConvexShape& shape, sf::Vector2f size, float skew);
        void updateMappingTexts();
        std::string getScancodeName(sf::Keyboard::Scancode code);

        bool inicializar{true};
        int selectedIndex{0};
        std::vector<ControlItem> controlItems;
        
        bool listeningForKey{false};
        bool listeningForJoy{false};

        sf::RectangleShape background;
        sf::Text title;
        sf::Text instruction;
        
        float timer{0.0f};
        bool axisInUse{false};

        const sf::Color P5_RED = sf::Color(211, 0, 0);
        const sf::Color P5_BLACK = sf::Color(20, 20, 20);
    };
}
