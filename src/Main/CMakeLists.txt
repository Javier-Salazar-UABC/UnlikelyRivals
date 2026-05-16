project(CimaEngine3v)

add_executable(CimaEngine3v main.cpp
        ../Juego/Escenas/Escena_Menu.cpp
        ../Juego/Escenas/Escena_Menu.hpp
        ../Juego/Escenas/Escena_Vibora.cpp
        ../Juego/Escenas/Escena_Vibora.hpp
        ../Juego/objetos/EnteVibora.cpp
        ../Juego/objetos/EnteVibora.hpp
        ../Juego/Escenas/Escena_Sim.cpp
        ../Juego/Escenas/Escena_Sim.hpp
        ../Juego/objetos/Persona.cpp
        ../Juego/objetos/Persona.hpp
        ../Juego/Escenas/Escena_Camara.hpp
        ../Juego/Escenas/Escena_Camara.cpp
        ../Juego/Escenas/Escena_SpriteTiles.cpp
        ../Juego/Escenas/Escena_SpriteTiles.hpp
        ../Juego/Escenas/Escena_Atlas.cpp
        ../Juego/Escenas/Escena_Atlas.hpp
        ../Juego/Maquinas/Naves/IdleJugadores.cpp
        ../Juego/Maquinas/Naves/IdleJugadores.hpp
        ../Juego/Maquinas/Naves/MoverJugadores.cpp
        ../Juego/Maquinas/Naves/MoverJugadores.hpp
        ../Juego/Maquinas/Naves/SaltarJugador.cpp
        ../Juego/Maquinas/Naves/SaltarJugador.hpp
        ../Juego/Maquinas/Naves/CorrerJugador.cpp
        ../Juego/Maquinas/Naves/CorrerJugador.hpp
        ../Juego/Maquinas/Naves/GolpearJugador.cpp
        ../Juego/Maquinas/Naves/GolpearJugador.hpp
        ../Juego/Maquinas/Naves/PatearJugador.cpp
        ../Juego/Maquinas/Naves/PatearJugador.hpp
        ../Juego/Maquinas/Bosses/FSMBoss.hpp
        ../Juego/Maquinas/Bosses/IdleBoss.cpp
        ../Juego/Maquinas/Bosses/IdleBoss.hpp
        ../Juego/Maquinas/Bosses/AttackBoss.cpp
        ../Juego/Maquinas/Bosses/AttackBoss.hpp
        ../Juego/Maquinas/Balas/BalasIdle.cpp
        ../Juego/Maquinas/Balas/BalasIdle.hpp
        ../Juego/Maquinas/Balas/BalasBoom.cpp
        ../Juego/Maquinas/Balas/BalasBoom.hpp
)

# copiar el archivo de configuración para las ides que sepan 
# donde estan todos los paths de las librerias y archivos.
if(NOT WIN32)
add_custom_command(
    TARGET CimaEngine3v POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/build/compile_commands.json # fuente
    ${CMAKE_SOURCE_DIR}/compile_commands.json                           # destino
    COMMENT "copiando compile_commands.json "
    )
endif()
