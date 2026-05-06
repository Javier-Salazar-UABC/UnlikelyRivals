#include "Botones.hpp"

namespace CE
{
    Botones::Botones(const std::string& nombre, const TipoAccion& tipo, const sf::Keyboard::Scancode& scan)
    :nombre{nombre},accion{tipo},tecla{scan}, source{InputSource::Keyboard}{}

    Botones::Botones(const std::string& nombre, const TipoAccion& tipo, unsigned int joyId, unsigned int button)
    :nombre{nombre}, accion{tipo}, joystickId{joyId}, button{button}, source{InputSource::JoystickButton}{}

    Botones::Botones(const std::string& nombre, const TipoAccion& tipo, unsigned int joyId, sf::Joystick::Axis axis, float pos)
    :nombre{nombre}, accion{tipo}, joystickId{joyId}, axis{axis}, axisPos{pos}, source{InputSource::JoystickAxis}{}

    const std::string Botones::getNombre() const
    {
        return nombre;
    }
    const std::string Botones::getTipoString() const
    {
        switch(accion)
        {
            case TipoAccion::OnPress:
                return "OnPress";
            case TipoAccion::OnRelease:
                return "OnRelease";
            case TipoAccion::Moved:
                return "Moved";
            default:
                return "None";
        }
    }
    const sf::Keyboard::Scancode& Botones::getScancode() const
    {
        return tecla;
    }
    const Botones::TipoAccion& Botones::getTipo() const
    {
        return accion;
    }
    const std::string Botones::toString() const
    {
        std::string srcStr = (source == InputSource::Keyboard) ? "KB" : "JOY";
        return "[" + srcStr + "] " + nombre + " --> " + getTipoString();  
    }
}
