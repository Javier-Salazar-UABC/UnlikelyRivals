#include "Persona.hpp"
#include "Juego/Componentes/IJComponentes.hpp"

Persona::Persona(float ancho, float largo, const sf::Color& relleno, const sf::Color& contorno)
    : Rectangulo{ancho, largo, relleno, contorno}
{

}

void Persona::onUpdate(float dt)
{

    Rectangulo::onUpdate(dt);
}

void Persona::draw(sf::RenderTarget& target, sf::RenderStates state) const
{
    Rectangulo::draw(target, state);
}
