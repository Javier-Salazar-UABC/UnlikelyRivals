#include "Juego.hpp"
#include<iostream>

#include <SFML/Graphics.hpp>
#include <string>

#include "../Motor/Primitivos/GestorEscenas.hpp"
#include "../Motor/Primitivos/GestorAssets.hpp"


#include <cstdlib>
#include <ctime>
#include <Juego/Escenas/Escena_Prueba.hpp>
#include <Juego/Escenas/Escena_SS.hpp>
#include <Juego/Escenas/Escena_Menu.hpp>
#include <Juego/Escenas/Escena_Vibora.hpp>
#include <Juego/Escenas/Escena_Sim.hpp>
#include <Juego/Escenas/Escena_Camara.hpp>
#include <Juego/Escenas/Escena_SpriteTiles.hpp>
#include <Juego/Escenas/Escena_Atlas.hpp>
#include <Juego/Escenas/EscenaMenu2.hpp>
#include <Juego/objetos/Entidad.hpp>


#include "Motor/Camaras/CamarasGestor.hpp"


namespace IVJ
{

    Juego::Juego()
        :CE::GameLayer{}
    {
        termino=false;
    }
    void Juego::OnInit(void)
    {
        std::cout<<"Inicializando Juego\n";
        
        
        CE::GestorEscenas::Get().registrarEscena("Prueba",std::make_shared<Escena_Prueba>(jugador));
        CE::GestorEscenas::Get().registrarEscena("SS",std::make_shared<Escena_SS>(jugador));
        CE::GestorEscenas::Get().registrarEscena("Menu",std::make_shared<Escena_Menu>());
        CE::GestorEscenas::Get().registrarEscena("Vibora",std::make_shared<Escena_Vibora>());
        CE::GestorEscenas::Get().registrarEscena("Simulacion",std::make_shared<Escena_Sim>());
        CE::GestorEscenas::Get().registrarEscena("ECamara",std::make_shared<Escena_Camara>(jugador));
        CE::GestorEscenas::Get().registrarEscena("ESpriteTiles",std::make_shared<Escena_SpriteTiles>(jugadornave));
        CE::GestorEscenas::Get().registrarEscena("EAtlas",std::make_shared<Escena_Atlas>(jugadornave));
        CE::GestorEscenas::Get().registrarEscena("Menu2",std::make_shared<EscenaMenu2>());

        //crear el jugador
        jugador = std::make_shared<Entidad>();
        jugador->getStats()->hp = 100;
        jugadornave = std::make_shared<Entidad>();
        jugadornave->getStats()->hp = 100;

        CE::GestorEscenas::Get().cambiarEscena("Menu2"); //ejecuta onInit()A
        escena_actual = &CE::GestorEscenas::Get().getEscenaActual();

        CE::GestorCamaras::Get().agregarCamara(
                std::make_shared<CE::CamaraLERP>(
                    CE::Vector2D{0.f,0.f}, //posición
                    CE::Vector2D{1024,720} //dimensiones
                    )
                );

        CE::GestorCamaras::Get().agregarCamara(
                std::make_shared<CE::CamaraSnapVentana>(
                    CE::Vector2D{0.f,0.f}, //posición
                    CE::Vector2D{1024,720}, //dimensiones
                    CE::Vector2D{500,300} //ventana
                    )
                );

        CE::GestorCamaras::Get().agregarCamara(
                std::make_shared<CE::CamaraSnapLR>(
                    CE::Vector2D{0.f,0.f}, //posición
                    CE::Vector2D{1024/4,720/4}, //dimensiones
                    CE::Vector2D{500/4,300/4} //ventana
                    )
                );
        
    }
    
    void Juego::OnInputs()
    {
        auto br = escena_actual->getBotonesRegistrados();

        CE::Botones::TipoAccion tipo_accion = CE::Botones::TipoAccion::OnRelease;
        std::string strAccion ="None";
        sf::Keyboard::Scancode scan = sf::Keyboard::Scancode::Comma;

        for(auto &sk : br)
        {
            if(sf::Keyboard::isKeyPressed(sk))
            {
                tipo_accion = CE::Botones::TipoAccion::OnPress;
                scan = sk;
                strAccion = escena_actual->getBotones().at(scan);
            }
        }
        escena_actual->onInputs(CE::Botones(strAccion,tipo_accion,scan));
    }
    void Juego::OnInputs(std::optional<sf::Event>& eventos)
    {
        if(eventos->getIf<sf::Event::Closed>())
            termino =true;

        if(eventos->getIf<sf::Event::MouseMoved>())
        {
            
        }

    }
    void Juego::OnInputs(CE::InputBuffer& buffer)
    {
        //procesar la lista circular
        while(!buffer.estaVacio())
        {
            auto evento_ptr = buffer.popData();
            if(!evento_ptr)
                continue;
            auto& evento = *evento_ptr;
            //eventos de la ventana
            if(evento.getIf<sf::Event::Closed>())
            {
                termino=true;
                continue; //o break si no necesitamos procesar más
            }
            //procesar inputs
            sf::Keyboard::Scancode scan = sf::Keyboard::Scancode::Unknown;
            CE::Botones::TipoAccion b_tipo;
            bool procesado=false;

            if(const auto* tecla_presionada = evento.getIf<sf::Event::KeyPressed>())
            {
                scan = tecla_presionada->scancode;
                b_tipo = CE::Botones::TipoAccion::OnPress;
                procesado = true;
            }
            else if(const auto* tecla_liberada = evento.getIf<sf::Event::KeyReleased>())
            {
                scan = tecla_liberada->scancode;
                b_tipo = CE::Botones::TipoAccion::OnRelease;
                procesado = true;
            }

            if(procesado)
            {
                auto iterador = escena_actual->getBotones().find(scan);
                if(iterador!=escena_actual->getBotones().end())
                {
                    //se encontro el input del control
                    std::string strAccion = iterador->second;
                    escena_actual->onInputs(CE::Botones(strAccion,b_tipo,scan));
                }
            }

        }
    }
    void Juego::OnUpdate(float dt)
    {
        escena_actual = &CE::GestorEscenas::Get().getEscenaActual();
        escena_actual->onUpdate(dt);
    }

    void Juego::OnRender()
    {
        escena_actual->onRender();
    }

}
