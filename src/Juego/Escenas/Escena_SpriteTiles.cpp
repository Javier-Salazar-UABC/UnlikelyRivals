#include "Escena_SpriteTiles.hpp"

#include <cmath>

#include "Motor/Utils/Vector2D.hpp"
#include <Motor/Inputs/Botones.hpp>
#include <Motor/Render/Render.hpp>
#include <Juego/objetos/Entidad.hpp>
#include <Motor/Componentes/IComponentes.hpp>
#include <Juego/Componentes/IJComponentes.hpp>
#include <memory>

#include "Juego/Maquinas/Bosses/IdleBoss.hpp"
#include "Juego/Maquinas/Naves/IdleJugadores.hpp"
#include "Juego/Sistemas/Sistemas.hpp"
#include "Motor/Camaras/CamarasGestor.hpp"
#include "Motor/Primitivos/GestorAssets.hpp"


namespace IVJ
{
    Escena_SpriteTiles::Escena_SpriteTiles(std::shared_ptr<Entidad>& pref)
        :CE::Escena{},player{pref}
    {
    }
    void Escena_SpriteTiles::onInit()
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

        //cargar mapa 3 layers
        tiles_layers.push_back(TileMap()); //puro mar
        tiles_layers.push_back(TileMap()); // las islas
        tiles_layers.push_back(TileMap()); // los objetos

        tiles_layers[0].setPosition({-500, -500});

        if(!tiles_layers[0].loadTileMap(ASSETS "/mapas/playa_col_layer1.txt",objetos, {-500.f, -500.f}))
            exit(EXIT_FAILURE);
        if(!tiles_layers[1].loadTileMap(ASSETS "/mapas/playa_layer2.txt"))
            exit(EXIT_FAILURE);
        if(!tiles_layers[2].loadTileMap(ASSETS "/mapas/playa_layer3.txt"))
            exit(EXIT_FAILURE);

        auto techo = std::make_shared<Entidad>();
        techo->getStats()->hp_max=100;
        techo->getStats()->hp=100;
        techo->getStats()->def=150;

        //cargar todo el atlas para poder poner cosas individuales
        CE::GestorAssets::Get().agregarTextura("atlas_playa",
                ASSETS "/atlas/playa_atlas.png",
                CE::Vector2D{0.f,0.f},
                CE::Vector2D{2048,768});

        auto techo_sprite = std::make_shared<CE::ISprite>(
            CE::GestorAssets::Get().getTextura("atlas_playa"),
            128,256, //dim
            1.f);
        //acomodar el sprite en el area deseada del atlas
        techo_sprite->m_sprite.setTextureRect(
                sf::IntRect{
                {64*28,64*6}, //220 indice
                {128,256} //dos tiles, 4 tiles en dimensiones
                });
        //acomodar el pivote para centrar bien el bounding box en el centro
        //128/2 = 64, 256/2 = 128
        techo_sprite->m_sprite.setOrigin({64,128});
        techo->addComponente(techo_sprite)
            .addComponente(std::make_shared<CE::IBoundingBox>(CE::Vector2D{128,256}));

        techo->setPosicion(1300,175);

        auto t_shader = std::make_shared<CE::IShader>("", ASSETS "/shaders/agua.frag");
        static sf::Texture p_tex = techo_sprite->m_sprite.getTexture();
        static float wescala = 0.01;
        static float vescala = 2.0;
        static float frec = 20.0;

        t_shader->setTextura("textura", &p_tex);
        t_shader->setEscalar("escala_distorcion", &wescala);
        t_shader->setEscalar("vel_distorcion", &vescala);
        t_shader->setEscalar("frecuencia", &frec);

        techo->addComponente(t_shader);
        objetos.agregarPool(techo);
        //Lab 8 animaciones
        CE::GestorAssets::Get().agregarTextura(
                "nave_sheet",                               //key
                ASSETS "/sprites/naves/player_sheet.png",   //hoja
                CE::Vector2D{0.f,0.f},                      //inicio
                CE::Vector2D{320.f,64.f}                    //dim de la hoja
            );

        CE::GestorAssets::Get().agregarTextura(
                "boss_sheet",
                ASSETS "/sprites/bosses/VileCrustaceanIdleAttack.png",
                CE::Vector2D{0.f,0.f},
                CE::Vector2D{924.f,260.f}
            );

        auto trans = player->getTransformada();
        trans->velocidad = CE::Vector2D{120.f,120.f};
        player->setPosicion(300.f,300.f);

        //Lab 8 animaciones
        auto sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura("nave_sheet"), //textura
                64,64,                                      // dim
                1.f);                                       // escala

        player->addComponente(sprite);
        player->addComponente(std::make_shared<CE::IControl>());
        player->addComponente(std::make_shared<CE::IBoundingBox>(
            CE::Vector2D{64.f,64.f} //dimensiones del sprite
        ));

        //lab 8 animaciones
        auto me = std::make_shared<IMaquinaEstado>();
        me->fsm = std::make_shared<IdleJugadores>(4, 0.15f);
        player->addComponente(me);
        //ejecuta onEntrar para inicializar variables
        player->setFSM(me->fsm);

        //boss entidad
        auto boss = std::make_shared<Entidad>();
        boss->getStats()->hp_max = 255;
        boss->getStats()->hp = 255;
        boss->getStats()->str = 150;
        boss->getStats()->agi = 50;
        boss->getStats()->def = 255;
        auto boss_sprite = std::make_shared<CE::ISprite>(
                CE::GestorAssets::Get().getTextura("boss_sheet"),
                154.f,130.f,
                1.f);
        //Por el momento no tiene estados, esta vacia la maquina
        auto boss_me = std::make_shared<IVJ::IMaquinaEstado>();
        boss_me->fsm = std::make_shared<IVJ::IdleBoss>(6,0.25f);
        auto boss_target = std::make_shared<ITarget>(nullptr);
        boss_target->setTarget(*player);

        boss->addComponente(boss_target)
            .addComponente(boss_sprite)
            .addComponente(boss_me)
            .addComponente(std::make_shared<IRangoAggro>(500.f))

            .setPosicion(500.f,500.f);
        boss->setFSM(boss_me->fsm);

        /// shader
    auto shader = std::make_shared<CE::IShader>(
    "",
    ASSETS "/shaders/bossfrag.frag");

    static sf::Texture boss_tex = boss_sprite->m_sprite.getTexture();

    shader->setTextura("textura",&boss_tex);

    boss->addComponente(shader);

        objetos.agregarPool(boss);

        inicializar=false;
    }
    void Escena_SpriteTiles::onFinal()
    {
        CE::GestorCamaras::Get().setCamaraActiva(0);
    }
    void Escena_SpriteTiles::onUpdate(float dt)
    {
        player->inputFSM(); //ejecutar los inputs de la maquina
        player->onUpdate(dt);
        SistemaMover(player, dt);
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
        static float acum_tiempo = 0;
        acum_tiempo += dt;
        objetos.getPool()[116]->getComponente<CE::IShader>()->setEscalar("dt", &acum_tiempo);
        //fin shaders
        // lab9 borra toda entidad que esta en el pool que hp sea <=0
        objetos.borrarPool();
    }
    void Escena_SpriteTiles::onInputs(const CE::Botones& accion)
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
    void Escena_SpriteTiles::onRender()
    {
        //renderizamos los layers primero para
        //que se pinten atrás de todo
        for(auto& al: tiles_layers)
            CE::Render::Get().AddToDraw(al);

        for(auto& obj: objetos.getPool()) {
            CE::Render::Get().AddToDraw(*obj);
            if(obj->tieneComponente<IRangoAggro>())
            {
                auto rango  = obj->getComponente<IRangoAggro>();
                auto pos = obj->getTransformada()->posicion;
                rango->marcador.setPosition({pos.x,pos.y});
                //CE::Render::Get().AddToDraw(rango->marcador);
            }
        }
        CE::Render::Get().AddToDraw(*player);

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
#endif
    }
}
