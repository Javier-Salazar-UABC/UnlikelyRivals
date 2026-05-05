#include "Escena_Atlas.hpp"

#include <cmath>

#include "Motor/Utils/Vector2D.hpp"
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <memory>

#include "Juego/Maquinas/Naves/IdleJugadores.hpp"
#include "Juego/Sistemas/Sistemas.hpp"
#include "Motor/Camaras/CamarasGestor.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"


namespace IVJ
{
    Escena_Atlas::Escena_Atlas(std::shared_ptr<Entidad>& pref)
        :CE::Escena{},player{pref}
    {
    }
    void Escena_Atlas::onInit()
    {
        CE::GestorCamaras::Get().setCamaraActiva(3);
        //le decimos a quien persigue
        CE::GestorCamaras::Get().getCamaraActiva().lockEnObjeto(player);
        if(!inicializar) return;

        registrarBotones(sf::Keyboard::Scancode::W,"arriba");
        registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
        registrarBotones(sf::Keyboard::Scancode::S,"abajo");
        registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
        registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::D,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");

        //cargar mapa 4 layers
        tiles_layers.push_back(TileMap()); //cielo
        tiles_layers.push_back(TileMap()); //montes
        tiles_layers.push_back(TileMap()); //plantas
        tiles_layers.push_back(TileMap()); //terreno

        if(!tiles_layers[0].loadTileMap(ASSETS "/mapas/tileset_layer4.txt"))
            exit(EXIT_FAILURE);
        if(!tiles_layers[1].loadTileMap(ASSETS "/mapas/tileset_layer3.txt"))
            exit(EXIT_FAILURE);
        if(!tiles_layers[2].loadTileMap(ASSETS "/mapas/tileset_layer2.txt"))
            exit(EXIT_FAILURE);
        if(!tiles_layers[3].loadTileMap(ASSETS "/mapas/tileset_layer1.txt", objetos))
            exit(EXIT_FAILURE);

        auto terreno = std::make_shared<Entidad>();
        terreno->getStats()->hp_max=100;
        terreno->getStats()->hp=100;
        terreno->getStats()->def=150;


        CE::GestorAssets::Get().agregarTextura(
            "esnupi",                                //llave
            ASSETS "/sprites/esnupi_walk.png",   //path del sprite
            CE::Vector2D{0.f,0.f},                  //pos dentro de la hoja
            CE::Vector2D{80.f,20.f});               // dimensiones

        auto trans = player->getTransformada();
        trans->velocidad = CE::Vector2D{120.f,120.f};
        player->setPosicion(300.f,300.f);

        auto sprite = std::make_shared<CE::ISprite>(
            CE::GestorAssets::Get().getTextura("esnupi"), //textura
            20,20,                                      // dim
            2.f);                                       // escala

        player->addComponente(sprite);
        player->addComponente(std::make_shared<CE::IControl>());
        player->addComponente(std::make_shared<CE::IBoundingBox>(
            CE::Vector2D{20*2.f,20*2.f} //dimensiones del sprite
        ));
        player->addComponente(std::make_shared<IGravedad>(1200.f, 600.f));

        auto me = std::make_shared<IMaquinaEstado>();
        me->fsm = std::make_shared<IdleJugadores>();
        player->addComponente(me);
        //ejecuta onEntrar para inicializar variables
        player->setFSM(me->fsm);
        inicializar=false;
    }
    void Escena_Atlas::onFinal()
    {
        CE::GestorCamaras::Get().setCamaraActiva(0);
    }
    void Escena_Atlas::onUpdate(float dt)
    {
        player->inputFSM(); //ejecutar los inputs de la maquina
        player->onUpdate(dt);
        SistemaMover(player, dt);
        SistemaGravedad(player, dt);
        for(auto& obj: objetos.getPool())
        {
            obj->inputFSM();
            obj->onUpdate(dt);
            if(obj->tieneComponente<ITarget>())
            {
                auto& tpos = *obj->getComponente<ITarget>()->pos;
                SistemaNPCLookTarget(*obj,tpos);
            }
            SistemaColAABBMid(*player, *obj, true);
        }
    }
    void Escena_Atlas::onInputs(const CE::Botones& accion)
    {
        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {
                if(accion.getNombre() == "arriba")
                {
                    player->getComponente<CE::IControl>()->arr=true;
                }
                if(accion.getNombre() == "abajo")
                {
                    player->getComponente<CE::IControl>()->abj=true;
                }
                if(accion.getNombre() == "derecha")
                {
                    player->getComponente<CE::IControl>()->der=true;
                }
                if(accion.getNombre() == "izquierda")
                {
                    player->getComponente<CE::IControl>()->izq=true;
                }
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                if(accion.getNombre() == "arriba")
                {
                    player->getComponente<CE::IControl>()->arr=false;
                }
                if(accion.getNombre() == "abajo")
                {
                    player->getComponente<CE::IControl>()->abj=false;
                }
                if(accion.getNombre() == "derecha")
                {
                    player->getComponente<CE::IControl>()->der=false;
                }
                if(accion.getNombre() == "izquierda")
                {
                    player->getComponente<CE::IControl>()->izq=false;
                }
                break;
            }
            case CE::Botones::TipoAccion::None:
            {
                break;
            }
        }
    }
    void Escena_Atlas::onRender()
    {
        //renderizamos los layers primero para
        //que se pinten atrás de todo
        for(auto& al: tiles_layers)
            CE::Render::Get().AddToDraw(al);

        for(auto& obj: objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
        CE::Render::Get().AddToDraw(*player);
/*
#if DEBUG
        auto cam = &CE::GestorCamaras::Get().getCamaraActiva();
        auto csv = (CE::CamaraSnapVentana*)cam;
        auto csvpos = csv->getTransformada().posicion;
        sf::RectangleShape debugcam{{csv->m_vdim.x,csv->m_vdim.y}};
        debugcam.setOrigin({csv->m_vdim.x/2.f,csv->m_vdim.y/2.f});
        debugcam.setPosition({csvpos.x,csvpos.y});
        debugcam.setOutlineThickness(3.f);
        debugcam.setOutlineColor(sf::Color::Yellow);
        debugcam.setFillColor(sf::Color::Transparent);
        CE::Render::Get().AddToDraw(debugcam);
#endif*/
    }
}
