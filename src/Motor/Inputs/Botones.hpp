#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace CE
{
    /**
     * @class Botones
     * @brief Encapsula la configuración de un botón/tecla con su tipo de acción.
     * 
     * Representa un mapeo entre una tecla del teclado y una acción de entrada
     * específica. Cada botón está identificado por un nombre, tiene un tipo de acción
     * (Al presionar o al liberar) y está asociado a un escancode SFML.
     * 
     * @note Los escancodes permiten mapeo independiente del idioma del teclado
     */
    class Botones
    {
        public:
            /**
             * @enum TipoAccion
             * @brief Tipos de eventos de acción que puede generar un botón.
             */
            enum class TipoAccion
            {
                OnPress,
                OnRelease,
                Moved,
                None
            };

            enum class InputSource
            {
                Keyboard,
                JoystickButton,
                JoystickAxis
            };
            
        public:
            /**
             * @brief Constructor para teclado.
             */
            Botones(const std::string& nombre, const TipoAccion& tipo, const sf::Keyboard::Scancode& scan);
            
            /**
             * @brief Constructor para joystick.
             */
            Botones(const std::string& nombre, const TipoAccion& tipo, unsigned int joyId, unsigned int button);

            /**
             * @brief Constructor para ejes de joystick.
             */
            Botones(const std::string& nombre, const TipoAccion& tipo, unsigned int joyId, sf::Joystick::Axis axis, float pos);

            const std::string getNombre() const;
            const std::string getTipoString() const;
            const TipoAccion& getTipo() const;
            const std::string toString() const;
            
            const sf::Keyboard::Scancode& getScancode() const;
            
            InputSource getSource() const { return source; }
            unsigned int getJoystickId() const { return joystickId; }
            unsigned int getButton() const { return button; }
            sf::Joystick::Axis getAxis() const { return axis; }
            float getAxisPos() const { return axisPos; }

        private:
            std::string nombre;
            TipoAccion accion{TipoAccion::None};
            InputSource source{InputSource::Keyboard};

            // Keyboard data
            sf::Keyboard::Scancode tecla{sf::Keyboard::Scancode::Unknown};
            
            // Joystick data
            unsigned int joystickId{0};
            unsigned int button{0};
            sf::Joystick::Axis axis{sf::Joystick::Axis::X};
            float axisPos{0.f};
    };
}
