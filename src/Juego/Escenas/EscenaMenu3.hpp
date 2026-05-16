#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace IVJ
{
    class EscenaMenu3 : public CE::Escena
    {
    public:
        EscenaMenu3() : CE::Escena() {};
        ~EscenaMenu3() override = default;

        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;
        
        enum class MenuState { MAIN, SETTINGS, CHAR_SELECT, MODE_SELECT };

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
        void setupModeSelect();
        void setupCharacterSelect();
        void setupDecorations();

        bool inicializar{true};
        MenuState currentState{MenuState::MAIN};
        int selectedIndex{0};
        int selectionStep{0}; // 0 = P1, 1 = P2
        std::vector<MenuItem> mainItems;
        std::vector<MenuItem> settingsItems;
        std::vector<MenuItem> characterItems;
        std::vector<MenuItem> modeItems;
        
        std::vector<MenuItem>* currentItems;
        
        sf::RectangleShape background;
        std::vector<sf::ConvexShape> bgDecorations;
        std::vector<Particle> particles;
        std::unique_ptr<sf::Sprite> silhouette;
        sf::RectangleShape selectionStripe;
        std::vector<std::unique_ptr<sf::Text>> bgFloatingTexts;
        std::unique_ptr<sf::Text> titleText;
        
        sf::RenderTexture occlusionTexture;
        sf::Shader crepuscularShader;
        float timer{0.0f};
        float animTimer{0.0f};
        float joyTimer{0.0f};
        float silhouetteScale{1.0f};
        float silhouetteTargetScale{1.0f};
        int lastCharIndex{-1};
        int currentFrame{0};
        int frameW{20}, frameH{20};
        const int TOTAL_FRAMES = 4;
        const float FRAME_TIME = 0.15f;
        const sf::Color P5_RED = sf::Color(211, 0, 0);
        const sf::Color P5_BLACK = sf::Color(20, 20, 20);
    };
}
