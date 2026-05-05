#pragma once
#include "../../Motor/Primitivos/Escena.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace IVJ
{
    class EscenaMenu2 : public CE::Escena
    {
    public:
        EscenaMenu2() : CE::Escena() {};
        ~EscenaMenu2() override = default;

        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;
        
        enum class MenuState { MAIN, SETTINGS, GALLERY };

    private:
        struct Particle {
            sf::ConvexShape shape;
            sf::Vector2f velocity;
            float rotationSpeed;
        };
        struct MenuItem {
            std::string text;
            std::unique_ptr<sf::Text> sfText;
            sf::ConvexShape box;
            sf::Vector2f targetPos;
            float rotation;
        };

        void updatePositions();
        void createJaggedBox(sf::ConvexShape& shape, sf::Vector2f size, float skew);
        void setupBackground();
        void setupMenu();
        void setupDecorations();

        bool inicializar{true};
        MenuState currentState{MenuState::MAIN};
        int selectedIndex{0};
        std::vector<MenuItem> mainItems;
        std::vector<MenuItem> settingsItems;
        
        std::vector<MenuItem>* currentItems;
        
        sf::RectangleShape background;
        std::vector<sf::ConvexShape> bgDecorations;
        std::vector<Particle> particles;
        std::unique_ptr<sf::Sprite> silhouette;
        sf::RectangleShape selectionStripe;
        std::vector<std::unique_ptr<sf::Text>> bgFloatingTexts;
        
        float timer{0.0f};
        const sf::Color P5_RED = sf::Color(211, 0, 0);
        const sf::Color P5_BLACK = sf::Color(20, 20, 20);
    };
}
