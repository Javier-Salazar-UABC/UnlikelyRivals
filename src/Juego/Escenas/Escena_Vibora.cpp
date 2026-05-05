#include "Escena_Vibora.hpp"

#include <Motor/Primitivos/GestorEscenas.hpp>
#include<Juego/Componentes/IJComponentes.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include "Juego/objetos/Texto.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"
#include "Motor/Render/Render.hpp"

namespace IVJ
{
    Escena_Vibora::Escena_Vibora()
        :CE::Escena{}
    {
    }
    void Escena_Vibora::onInit()
    {
        if(!inicializar) return;
            registrarBotones(sf::Keyboard::Scancode::W,"arriba");
            registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
            registrarBotones(sf::Keyboard::Scancode::S,"abajo");
            registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
            registrarBotones(sf::Keyboard::Scancode::D,"derecha");
            registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
            registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
            registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");

        // TAREA 5: Crear EnteVibora e inicializar con 3 partes
        // Instanciamos la cabeza (que hereda de Triangulo, pide tamaño, relleno, contorno)
        cabeza = std::make_shared<EnteVibora>(20.0f, sf::Color::Green, sf::Color::Black);
        cabeza->setPosicion(400.0f, 300.0f); // Posición inicial en el mapa

        // Creamos el componente de cuerpo con ancho y alto de ejemplo (20, 20)
        auto componente_cuerpo = std::make_shared<ICPartesCuerpo>(20, 20);

        // Iniciamos con 3 partes
        for (int i = 0; i < 3; ++i) {
            componente_cuerpo->partes.push_back(std::make_shared<ICParte>());
        }

        // Le pegamos el componente a la cabeza y la agregamos al pool de la escena
        cabeza->addComponente(componente_cuerpo);
        objetos.agregarPool(cabeza);
        inicializar=false;
    }
    void Escena_Vibora::onFinal()
    {
    }
    void Escena_Vibora::onUpdate(float dt)
    {
        // TAREA 6: Agregar una parte nueva cada 3 segundos
        timer_nueva_parte += dt; // Sumamos el tiempo transcurrido

        if(timer_nueva_parte >= 3.0f)
        {
            // Verificamos que la cabeza tenga el componente
            if(auto cuerpo = cabeza->getComponente<ICPartesCuerpo>())
            {
                cuerpo->partes.push_back(std::make_shared<ICParte>());
                // Debug opcional para ver que funciona:
                // std::cout << "Nueva parte agregada! Total partes: " << cuerpo->partes.size() << "\n";
            }
            timer_nueva_parte = 0.0f; // Reseteamos el temporizador
        }
        
        for (auto&obj : objetos.getPool()) {
            obj->onUpdate(dt);
        }
    }
    void Escena_Vibora::onInputs(const CE::Botones& accion)
    {
        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {
                if(accion.getNombre() == "arriba")
                {
                    cabeza->setDireccion(0.0f, -1.0f); // <-- Faltaba esto
                }

                if(accion.getNombre() == "abajo")
                {
                    cabeza->setDireccion(0.0f, 1.0f);  // <-- Faltaba esto
                }

                if(accion.getNombre() == "derecha")
                {
                    cabeza->setDireccion(1.0f, 0.0f);  // <-- Faltaba esto
                }

                if(accion.getNombre() == "izquierda")
                {
                    cabeza->setDireccion(-1.0f, 0.0f); // <-- Faltaba esto
                }
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                break;
            }
            case CE::Botones::TipoAccion::None:
            {
                break;
            }
        }
    }
    void Escena_Vibora::onRender()
    {
        for (auto&obj : objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
    }
}
