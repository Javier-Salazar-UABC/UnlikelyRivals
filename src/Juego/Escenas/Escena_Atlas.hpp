#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include "Juego/objetos/Entidad.hpp"
#include "Juego/Figuras/Figuras.hpp"
namespace IVJ
{
    class Escena_Atlas : public CE::Escena
    {
    public:
        explicit Escena_Atlas(std::shared_ptr<Entidad>& pref);
        virtual ~Escena_Atlas(){};
        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;
        std::shared_ptr<Entidad> getJugador() override {return player;}
    private:
        int inicializar{1};
        std::shared_ptr<Entidad>& player;
        std::shared_ptr<Entidad> p2;

        // Countdown variables
        std::unique_ptr<sf::Text> m_countdownText;
        int m_countdownState{3}; // 3, 2, 1, 0 (GO), -1 (Active fight)
        float m_countdownTimeRemaining{1.0f};
        float m_elapsedTime{0.0f};

        // Bobbing start positions
        const float m_p1StartX{300.f};
        const float m_p1StartY{260.f};
        const float m_p2StartX{420.f};
        const float m_p2StartY{260.f};
    };
}
