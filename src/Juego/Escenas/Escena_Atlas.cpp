#include "Escena_Atlas.hpp"

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
        if(inicializar) {
            auto smashCam = std::make_shared<CE::CamaraSmash>(CE::Vector2D{512, 360}, CE::Vector2D{1024, 720});
            CE::GestorCamaras::Get().agregarCamara(smashCam);
            CE::GestorCamaras::Get().setCamaraActiva(CE::Camara::num_camaras - 1);
        }
        
        // La cámara activa debería ser la Smash ahora
        auto& cam = CE::GestorCamaras::Get().getCamaraActiva();
        if (auto* smash = dynamic_cast<CE::CamaraSmash*>(&cam)) {
            smash->limpiarTargets();
            smash->agregarTarget(player);
            if (p2) smash->agregarTarget(p2);
        }
        if(!inicializar) return;

        registrarBotones(sf::Keyboard::Scancode::W,"arriba");
        registrarBotones(sf::Keyboard::Scancode::Up,"arriba");
        registrarBotones(sf::Keyboard::Scancode::S,"abajo");
        registrarBotones(sf::Keyboard::Scancode::Down,"abajo");
        registrarBotones(sf::Keyboard::Scancode::A,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::Left,"izquierda");
        registrarBotones(sf::Keyboard::Scancode::D,"derecha");
        registrarBotones(sf::Keyboard::Scancode::Right,"derecha");
        registrarBotones(sf::Keyboard::Scancode::LShift,"correr");
        registrarBotones(sf::Keyboard::Scancode::RShift,"correr");
        registrarBotones(sf::Keyboard::Scancode::Enter,"aceptar");
        registrarBotones(sf::Keyboard::Scancode::J,"atacar");

        // --- Mapeo de Control (Switch Pro / Xbox / PS) ---
        for (unsigned int i = 0; i < 4; ++i)
        {
            // Botones principales (A/B/X/Y o Cross/Circle/Square/Triangle)
            registrarJoystickBoton(i, 0, "atacar");
            registrarJoystickBoton(i, 1, "atacar");
            registrarJoystickBoton(i, 2, "correr");
            registrarJoystickBoton(i, 3, "correr");
            
            // Triggers / Bumpers
            registrarJoystickBoton(i, 4, "correr"); // L1 / LB
            registrarJoystickBoton(i, 5, "correr"); // R1 / RB
            registrarJoystickBoton(i, 7, "atacar"); // R2 / ZR
            
            // Ejes para movimiento (Sticks y D-Pad)
            registrarJoystickEje(i, sf::Joystick::Axis::X, "horizontal");
            registrarJoystickEje(i, sf::Joystick::Axis::Y, "vertical");
            registrarJoystickEje(i, sf::Joystick::Axis::PovX, "horizontal");
            registrarJoystickEje(i, sf::Joystick::Axis::PovY, "vertical");
        }

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
            "esnupi_walk",                                //llave
            ASSETS "/sprites/esnupi_walk.png",   //path del sprite
            CE::Vector2D{0.f,0.f},                  //pos dentro de la hoja
            CE::Vector2D{80.f,20.f});               // dimensiones

        CE::GestorAssets::Get().agregarTextura(
            "esnupi_run",                                
            ASSETS "/sprites/esnupi_run.png",   
            CE::Vector2D{0.f,0.f},                  
            CE::Vector2D{120.f,20.f});               // dimensiones

        CE::GestorAssets::Get().agregarTextura(
            "esnupi_jump",                                
            ASSETS "/sprites/esnupi_jump.png",   
            CE::Vector2D{0.f,0.f},                  
            CE::Vector2D{100.f,20.f});               // dimensiones

        CE::GestorAssets::Get().agregarTextura(
            "esnupi_idle",                                
            ASSETS "/sprites/esnupi_idle.png",   
            CE::Vector2D{0.f,0.f},                  
            CE::Vector2D{80.f,20.f});               // dimensiones

        CE::GestorAssets::Get().agregarTextura(
            "esnupi_punch",                                
            ASSETS "/sprites/esnupi_punch.png",   
            CE::Vector2D{0.f,0.f},                  
            CE::Vector2D{80.f,20.f});               // dimensiones

        CE::GestorAssets::Get().agregarTextura(
            "esnupi_kick",                                
            ASSETS "/sprites/esnupi_kick.png",   
            CE::Vector2D{0.f,0.f},                  
            CE::Vector2D{80.f,20.f});               // dimensiones (asumiendo 6 frames de 20x20)

        auto trans = player->getTransformada();
        trans->velocidad = CE::Vector2D{120.f,120.f};
        player->setPosicion(300.f,300.f);

        auto sprite = std::make_shared<CE::ISprite>(
            CE::GestorAssets::Get().getTextura("esnupi_walk"), //textura
            20,20,                                      // dim
            2.f);                                       // escala


        player->addComponente(std::make_shared<CE::IControl>());
        player->addComponente(std::make_shared<CE::IBoundingBox>(
            CE::Vector2D{18*2.f,18*2.f}, //dimensiones del sprite
            CE::CollisionLayer::PLAYER
        ));
        player->addComponente(std::make_shared<IGravedad>(1200.f, 600.f));
        player->addComponente(sprite);

        auto me = std::make_shared<IMaquinaEstado>();
        me->fsm = std::make_shared<IdleJugadores>(4, 0.15f);
        player->addComponente(me);
        //ejecuta onEntrar para inicializar variables
        player->setFSM(me->fsm);

        // --- Jugador 2 (maniquí de prueba para testear golpes) ---
        p2 = std::make_shared<Entidad>();
        p2->setPosicion(420.f, 300.f);
        p2->getTransformada()->velocidad = CE::Vector2D{120.f, 120.f};
        p2->getStats()->porcentaje_danio = 0.f;

        auto sprite2 = std::make_shared<CE::ISprite>(
            CE::GestorAssets::Get().getTextura("esnupi_idle"),
            20, 20, 2.f);
        // Colorear en azul para distinguirlo del jugador 1
        sprite2->m_sprite.setColor(sf::Color{100, 180, 255, 255});

        p2->addComponente(sprite2);
        p2->addComponente(std::make_shared<CE::IBoundingBox>(
            CE::Vector2D{18*2.f, 18*2.f},
            CE::CollisionLayer::ENEMY
        ));
        p2->addComponente(std::make_shared<IGravedad>(1200.f, 600.f));
        objetos.agregarPool(p2);

        // Agregar a la cámara smash si es necesario (ya se hace en el dynamic_cast arriba pero por si acaso)
        auto& camActiva = CE::GestorCamaras::Get().getCamaraActiva();
        if (auto* s = dynamic_cast<CE::CamaraSmash*>(&camActiva)) {
            s->agregarTarget(p2);
        }
        // --------------------------------------------------------

        inicializar=false;
    }
    void Escena_Atlas::onFinal()
    {
        CE::GestorCamaras::Get().setCamaraActiva(0);
    }
    void Escena_Atlas::onUpdate(float dt)
    {
        player->inputFSM();
        player->onUpdate(dt);
        SistemaMover(player, dt);
        SistemaGravedad(player, dt);

        // Primera pasada: actualizar todos los objetos y colisiones vs player
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

        // Sistema de partículas fuera de pantalla (Smash Bros style)
        std::vector<std::shared_ptr<CE::Objeto>> targets;
        targets.push_back(player);
        if (p2) targets.push_back(p2);
        SistemaLimitesPantalla(targets);

        // --- MUERTE POR BLAST ZONE (Círculo de muerte estilo Smash) ---
        SistemaMuerteBlastZone(targets, {512, 360}, 750.0f, dt);

        // Segunda pasada: física de objetos dinámicos (los que tienen IGravedad)
        // y su colisión vs todos los tiles/objetos estáticos del pool
        for(auto& obj: objetos.getPool())
        {
            if(!obj->tieneComponente<IGravedad>()) continue;

            SistemaMover(obj, dt);
            SistemaGravedad(obj, dt);

            // Resolver colisión de este objeto dinámico contra todos los demás del pool
            for(auto& tile: objetos.getPool())
            {
                if(tile == obj) continue;           // no contra sí mismo
                if(tile->tieneComponente<IGravedad>()) continue; // no contra otros dinámicos
                SistemaColAABBMid(*obj, *tile, true);
            }
        }

        // Procesar golpes del jugador
        SistemaGolpe(player, objetos);
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
                if(accion.getNombre() == "correr")
                {
                    player->getComponente<CE::IControl>()->run=true;
                }
                if(accion.getNombre() == "atacar")
                {
                    player->getComponente<CE::IControl>()->acc=true;
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
                if(accion.getNombre() == "correr")
                {
                    player->getComponente<CE::IControl>()->run=false;
                }
                if(accion.getNombre() == "atacar")
                {
                    player->getComponente<CE::IControl>()->acc=false;
                }
                break;
            }
            case CE::Botones::TipoAccion::Moved:
            {
                float pos = accion.getAxisPos();
                if (accion.getNombre() == "horizontal")
                {
                    player->getComponente<CE::IControl>()->der = (pos > 30.f);
                    player->getComponente<CE::IControl>()->izq = (pos < -30.f);
                }
                if (accion.getNombre() == "vertical")
                {
                    player->getComponente<CE::IControl>()->abj = (pos > 30.f);
                    player->getComponente<CE::IControl>()->arr = (pos < -30.f);
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

#if DEBUG
        SistemaDibujarGolpe(player);
#endif
        // --- DIBUJAR PARTÍCULAS DE MUERTE (Al final para que estén arriba) ---
        SistemaDibujarParticulasMuerte();
    }
}
