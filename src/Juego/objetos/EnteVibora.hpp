#pragma once
#include "Juego/Figuras/Figuras.hpp"
#include "Motor/Utils/Vector2D.hpp"

class EnteVibora : public IVJ::Triangulo {
public:
    explicit EnteVibora(float lado, const sf::Color& relleno, const sf::Color& contorno);
    ~EnteVibora() override {};

    void onUpdate(float dt) override;

    // ¡NUEVO! Sobrescribimos el draw para poder dibujar el cuerpo
    void draw(sf::RenderTarget& target, sf::RenderStates state) const override;

    void setDireccion(float dx, float dy);

private:
    CE::Vector2D direccion{1.0f, 0.0f};
    float velocidad{150.0f}; // Píxeles por segundo (movimiento fluido)
};