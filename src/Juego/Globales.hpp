#pragma once
#include <string>

namespace IVJ
{
    /**
     * @enum PersonajeID
     * @brief Identificadores de personajes disponibles.
     */
    enum class PersonajeID
    {
        SNOOPY,
        MASTER_CHIEF,
        GOKU
    };

    /**
     * @enum GameMode
     * @brief Modos de juego disponibles.
     */
    enum class GameMode
    {
        PVP,
        PVC
    };

    /**
     * @struct SeleccionPersonajes
     * @brief Almacena los personajes seleccionados por los jugadores.
     */
    struct Globales
    {
        static inline PersonajeID p1_seleccionado = PersonajeID::SNOOPY;
        static inline PersonajeID p2_seleccionado = PersonajeID::MASTER_CHIEF;
        static inline GameMode modo_juego = GameMode::PVP;
    };
}
