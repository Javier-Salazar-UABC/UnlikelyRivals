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
#include "Juego/Personajes/Snoopy.hpp"
#include "Juego/Personajes/MasterChief.hpp"
#include "Juego/Personajes/Goku.hpp"
#include "Juego/Globales.hpp"


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

        // --- Configurar límites de la cámara ---
        float worldW = tiles_layers[3].getAnchoTotal();
        float worldH = tiles_layers[3].getAltoTotal();
        
        auto& camActiva = CE::GestorCamaras::Get().getCamaraActiva();
        if (auto* smash = dynamic_cast<CE::CamaraSmash*>(&camActiva)) {
            smash->setWorldBounds(sf::FloatRect({0, 0}, {worldW, worldH}));
        }

        auto terreno = std::make_shared<Entidad>();

        terreno->getStats()->hp_max=100;
        terreno->getStats()->hp=100;
        terreno->getStats()->def=150;


        // --- Cargar jugador 1 según selección ---
        auto loadPlayer = [&](std::shared_ptr<Entidad>& ent, PersonajeID id, float x, float y) {
            switch(id) {
                case PersonajeID::SNOOPY:       Snoopy::cargar(ent, x, y); break;
                case PersonajeID::MASTER_CHIEF: MasterChief::cargar(ent, x, y); break;
                case PersonajeID::GOKU:         Goku::cargar(ent, x, y); break;
            }
        };

        loadPlayer(player, Globales::p1_seleccionado, 300.f, 300.f);

        // --- Cargar jugador 2 según selección ---
        p2 = std::make_shared<Entidad>();
        loadPlayer(p2, Globales::p2_seleccionado, 420.f, 300.f);
        p2->getStats()->porcentaje_danio = 0.f;

        objetos.agregarPool(player);
        objetos.agregarPool(p2);

        // Agregar a la cámara smash si es necesario
        if (auto* s = dynamic_cast<CE::CamaraSmash*>(&camActiva)) {

            s->limpiarTargets();
            s->agregarTarget(player);
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
        // Primera pasada: actualizar todos los objetos
        for(auto& obj: objetos.getPool())
        {
            obj->inputFSM();
            obj->onUpdate(dt);
            if(obj->tieneComponente<ITarget>())
            {
                auto& tpos = *obj->getComponente<ITarget>()->pos;
                SistemaNPCLookTarget(*obj,tpos);
            }
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
                
                // Colisión vs Tiles (objetos estáticos)
                if(!tile->tieneComponente<IGravedad>()) {
                    SistemaColAABBMid(*obj, *tile, true);
                }
                // Colisión vs Otros Jugadores / Dinámicos (opcional, estilo Smash)
                else {
                    // Solo resolvemos si son diferentes para evitar que player choque consigo mismo
                    // (Aunque ya tenemos el 'tile == obj' arriba)
                    SistemaColAABBMid(*obj, *tile, true);
                }
            }
        }

        // Procesar golpes de ambos jugadores
        SistemaGolpe(player, objetos);
        if (p2) SistemaGolpe(p2, objetos);
    }
    void Escena_Atlas::onInputs(const CE::Botones& accion)
    {
        // Ruteo de entrada: Gamepad controla a P2, Teclado controla a P1
        std::shared_ptr<Entidad> target = player;
        if (accion.getSource() == CE::Botones::InputSource::JoystickButton || 
            accion.getSource() == CE::Botones::InputSource::JoystickAxis) {
            target = p2;
        }

        if (!target) return;
        auto control = target->getComponente<CE::IControl>();
        if (!control) return;

        switch(accion.getTipo())
        {
            case CE::Botones::TipoAccion::OnPress:
            {
                if(accion.getNombre() == "arriba")    control->arr = true;
                if(accion.getNombre() == "abajo")     control->abj = true;
                if(accion.getNombre() == "derecha")   control->der = true;
                if(accion.getNombre() == "izquierda") control->izq = true;
                if(accion.getNombre() == "correr")    control->run = true;
                if(accion.getNombre() == "atacar")    control->acc = true;
                break;
            }
            case CE::Botones::TipoAccion::OnRelease:
            {
                if(accion.getNombre() == "arriba")    control->arr = false;
                if(accion.getNombre() == "abajo")     control->abj = false;
                if(accion.getNombre() == "derecha")   control->der = false;
                if(accion.getNombre() == "izquierda") control->izq = false;
                if(accion.getNombre() == "correr")    control->run = false;
                if(accion.getNombre() == "atacar")    control->acc = false;
                break;
            }
            case CE::Botones::TipoAccion::Moved:
            {
                float pos = accion.getAxisPos();
                if (accion.getNombre() == "horizontal")
                {
                    control->der = (pos > 30.f);
                    control->izq = (pos < -30.f);
                }
                if (accion.getNombre() == "vertical")
                {
                    control->abj = (pos > 30.f);
                    control->arr = (pos < -30.f);
                }
                break;
            }
            case CE::Botones::TipoAccion::None: break;
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


#if DEBUG
        SistemaDibujarGolpe(player);
        if (p2) SistemaDibujarGolpe(p2);
#endif
        // --- DIBUJAR PARTÍCULAS DE MUERTE (Al final para que estén arriba) ---
        SistemaDibujarParticulasMuerte();
    }
}
