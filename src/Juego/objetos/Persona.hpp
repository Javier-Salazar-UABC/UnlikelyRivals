#pragma once
#include "Juego/Figuras/Figuras.hpp"

class Persona : public IVJ::Rectangulo {
public:
    explicit Persona(float ancho, float largo, const sf::Color& relleno, const sf::Color& contorno);
    ~Persona() override {};

    void onUpdate(float dt) override;

    void draw(sf::RenderTarget& target, sf::RenderStates state) const override;

private:

};