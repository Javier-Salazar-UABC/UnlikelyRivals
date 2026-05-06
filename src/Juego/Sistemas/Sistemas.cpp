#include "Sistemas.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include <Juego/Componentes/IJComponentes.hpp>
#include "../../Motor/Primitivos/GestorAssets.hpp"
#include "../objetos/Entidad.hpp"
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "../../Motor/Utils/Lerp.hpp"
#include "../../Motor/Render/Render.hpp"
#include "../../Motor/Primitivos/GestorColisiones.hpp"
#include "Motor/Camaras/CamarasGestor.hpp"
#include "Juego/Maquinas/Naves/GolpearJugador.hpp"
#include "Juego/Maquinas/Naves/PatearJugador.hpp"
#include <memory>


namespace IVJ
{
    void SistemaControl(CE::Objeto&ente, float dt)
    {
        //no hay necesidad verificar si tiene, se asume que tiene
        //control y transformada
        auto p = ente.getTransformada();
        auto c = ente.getComponente<CE::IControl>();
        (void)dt;
        if(c->arr)
            p->velocidad.y=-200;

        if(c->abj)
            p->velocidad.y=200;
        if(c->der)
            p->velocidad.x=200;
        if(c->izq)
            p->velocidad.x=-200;
        if(c->run)
            p->velocidad.escala(2.f);

    }

    void SistemaMover(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt)
    {

        for(auto& ente : entes)
        {
            //todo ente tiene ITransform por lo que no requiere verificación
            auto trans = ente->getTransformada();
            trans->posicion.suma(trans->velocidad.escala(dt));

        }

    }
    void SistemaMover(const std::shared_ptr<CE::Objeto>& objeto, float dt)
    {
        auto trans = objeto->getTransformada();
        auto control = objeto->getComponente<CE::IControl>();
        auto grav = objeto->getComponente<IGravedad>();

        float move_speed = 150.0f; // Velocidad base de caminata
        float vel_x_final = 0.0f;

        if (control && control->isActivo()) {
            if (control->der) vel_x_final = move_speed;
            if (control->izq) vel_x_final = -move_speed;
            if (control->run) vel_x_final *= 1.6f;

            // Orientación del sprite basada en movimiento
            if (objeto->tieneComponente<CE::ISprite>()) {
                auto sprite = objeto->getComponente<CE::ISprite>();
                if (vel_x_final > 0) sprite->m_sprite.setScale({sprite->escala, sprite->escala});
                else if (vel_x_final < 0) sprite->m_sprite.setScale({-sprite->escala, sprite->escala});
            }
        }

        // Aplicar velocidad persistente (knockback/impulsos)
        trans->posicion.x += (vel_x_final + trans->velocidad.x) * dt;
        
        // --- GESTIÓN DE FRICCIÓN (Resbalamiento) ---
        // Si está en el suelo, la fricción es altísima para que no se resbale
        float friction_factor = 0.3f; // Aire
        if (grav && grav->en_suelo) {
            friction_factor = 0.0001f; // Suelo (casi instantáneo)
        }

        trans->velocidad.x *= std::pow(friction_factor, dt); 
        if (std::abs(trans->velocidad.x) < 10.0f) trans->velocidad.x = 0;
    }

    bool SistemaColAABB(CE::Objeto& A, CE::Objeto& B, bool resolucion)
    {
        if(!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
            return false;
        auto boxA = A.getComponente<CE::IBoundingBox>();
        auto boxB = B.getComponente<CE::IBoundingBox>();

        // Check collision matrix
        if (!CE::GestorColisiones::Get().puedenColisionar(boxA->layer, boxB->layer))
            return false;

        auto mA = boxA->mitad;
        auto *pa = &A.getTransformada()->posicion;
        auto prevA = A.getTransformada()->pos_prev;
        auto mB = B.getComponente<CE::IBoundingBox>()->mitad;
        auto pb = B.getTransformada()->posicion;
        float dX = std::abs(pb.x - pa->x);
        float dY = std::abs(pb.y - pa->y);
        float sumMidX = mA.x + mB.x;
        float sumMidY = mA.y + mB.y;
        
        float penX = sumMidX - dX;
        float penY = sumMidY - dY;
        bool hay_colision = penX > 0 && penY > 0;

        if(hay_colision && resolucion) {
            if (penX < penY) {
                pa->x = prevA.x;
            } else {
                if (A.tieneComponente<IGravedad>() && pa->y > prevA.y) {
                    auto grav = A.getComponente<IGravedad>();
                    grav->velocidad_Y = 0.0f;
                    grav->en_suelo = true;
                    grav->saltos_restantes = 2;
                }
                pa->y = prevA.y;
            }
        }
        return hay_colision;
    }

    bool SistemaColAABBMid(CE::Objeto& A, CE::Objeto& B, bool resolucion)
    {
        if(!A.tieneComponente<CE::IBoundingBox>() || !B.tieneComponente<CE::IBoundingBox>())
            return false;
        auto boxA = A.getComponente<CE::IBoundingBox>();
        auto boxB = B.getComponente<CE::IBoundingBox>();

        // Check collision matrix
        if (!CE::GestorColisiones::Get().puedenColisionar(boxA->layer, boxB->layer))
            return false;

        auto midA = boxA->mitad;
        auto *pa = &A.getTransformada()->posicion;
        auto prevA = A.getTransformada()->pos_prev;
        auto midB = B.getComponente<CE::IBoundingBox>()->mitad;
        auto *pb = &B.getTransformada()->posicion;
        float dX = std::abs(pb->x - pa->x);
        float dY = std::abs(pb->y - pa->y);
        float sumMidX = midA.x + midB.x;
        float sumMidY = midA.y + midB.y;
        
        float penX = sumMidX - dX;
        float penY = sumMidY - dY;
        bool hay_colision = penX > 0 && penY > 0;
        
        if (resolucion && hay_colision) {
            if (penX < penY) { 
                pa->x = prevA.x;
            } else { 
                if (A.tieneComponente<IGravedad>() && pa->y > prevA.y) {
                    auto grav = A.getComponente<IGravedad>();
                    grav->velocidad_Y = 0.0f;
                    grav->en_suelo = true;
                    grav->saltos_restantes = 2;
                }
                pa->y = prevA.y;
            }
        }
        return hay_colision;
    }

    bool checkRayoHit(CE::Objeto& npc, CE::Vector2D& p1, CE::Vector2D& p2)
    {
        if(!npc.tieneComponente<CE::IBoundingBox>()) return false;
        
        auto midnpc = npc.getComponente<CE::IBoundingBox>()->mitad;
        auto posnpc = npc.getTransformada()->posicion;
        auto linea = (p2-p1);//.escala(1.f);
        //std::cout<<"Mid linea: "<<midlinea<<"\n";
        float dx = std::abs(posnpc.x - p1.x);
        float dy = std::abs(posnpc.y - p1.y);
        float sumMidX = midnpc.x + std::abs(linea.x) ;
        float sumMidY = midnpc.y + std::abs(linea.y) ;
        
        return (sumMidX - dx >0) && (sumMidY - dy >0);
        
    }

    void SistemaSpawn(CE::Pool& spawns)
    {
        for(auto &s : spawns.getPool())
        {
            auto respawndata = s->getComponente<CE::IRespawn>();
            if(!respawndata || respawndata->curr_spawn >= respawndata->max_spawn)
                continue;
            //respawn
            if(respawndata->timer_actual >= respawndata->timer_maximo)
            {
                //crear objeto
                auto malillo = std::make_shared<Entidad>(); //(*(respawndata->prefab));
                //auto comp = respawndata->prefab->getListaComponentes();
                //auto comp = respawndata->prefab->getComponente<CE::ISprite>();
                //for(auto &c : comp)
                //{
                    //malillo->copyComponente<CE::ISprite>(comp);
                    //malillo->copyComponente<CE::IPaths>(respawndata->prefab->getComponente<CE::IPaths>());
                //}
                 
                malillo->addComponente(std::make_shared<CE::IPaths>(60))
                .addComponente(std::make_shared<CE::ISprite>(
                            CE::GestorAssets::Get().getTextura("hoja_blue"),68,91,1.f))
                .addComponente(std::make_shared<CE::IShader>("",ASSETS "/shaders/contorno.frag"));
                malillo->getComponente<CE::IPaths>()->addCurva(
                        CE::Vector2D{500.f,300.f},CE::Vector2D{200.f,500.f},CE::Vector2D{800.f,300.f});
                malillo->getComponente<CE::IPaths>()->addCurva(
                        CE::Vector2D{800.f,300.f},CE::Vector2D{200.f,100.f},CE::Vector2D{500.f,300.f});


                //mandar datos al shader
                auto shader = malillo->getComponente<CE::IShader>();
                static sf::Texture textura=malillo->getComponente<CE::ISprite>()->m_sprite.getTexture();
                
                static sf::Glsl::Vec4 uOutlineColor{1.f,0.f,1.f,1.f};
                //static  float uEdgeThreshold= 0.15f; //0.15-0.35
                //static  float uEdgeSoftness = 0.02f;
                static  float uThickness = 0.15f;
                auto w = malillo->getComponente<CE::ISprite>()->width;
                auto h = malillo->getComponente<CE::ISprite>()->height;
                static sf::Glsl::Vec2 uTexSize{(float)w,(float)h};

                shader->setTextura("uTex",&textura);
                shader->setVector4("uOutlineColor",&uOutlineColor);
                //shader->setEscalar("uEdgeThreshold",&uEdgeThreshold);
                //shader->setEscalar("uEdgeSoftness",&uEdgeSoftness);
                shader->setEscalar("uThickness",&uThickness);
                shader->setVector2("uTexSize",&uTexSize);

                //posiciones
                malillo->getStats()->hp =100;
                //asumimos que spwan tine un cuadro , tomamos el cuadro como el area
                int size_x = respawndata->width; 
                int size_y = respawndata->height;
                float x0 = s->getTransformada()->posicion.x-size_x/2.f;
                float y0 = s->getTransformada()->posicion.y-size_y/2.f;
                float x = x0+(rand()%size_x);
                float y = y0+(rand()%size_y);
                malillo->setPosicion(x,y);
                //spawns.push_back(malillo);
                spawns.agregarPool(malillo);
                respawndata->timer_actual=0;
                respawndata->curr_spawn++;
            }
            respawndata->timer_actual++;
        }
    }
    void SistemaPaths(std::vector<std::shared_ptr<CE::Objeto>>& obj)
    {
        for(auto &o: obj)
        {
            if(o->tieneComponente<CE::IPaths>())
            {
                auto path = o->getComponente<CE::IPaths>();
                int num_curvas = path->puntos.size()/3;
                if(path->puntos.size()==0) continue;
                if(path->id_curva >= num_curvas) continue;
                float t = (path->frame_actual_curva%(path->frame_total_curva+1))/(float)path->frame_total_curva;
                //std::cout<<o<<" t: "<<t<<"\n";
                CE::Vector2D P0 = path->puntos[0+path->id_curva*(path->offset-0)];
                CE::Vector2D P1 = path->puntos[1+path->id_curva*(path->offset-0)];
                CE::Vector2D P2 = path->puntos[2+path->id_curva*(path->offset-0)];
                auto posiciones_actual = CE::lerp2(P0, P1, P2, t);
                path->frame_actual_curva++;
                o->setPosicion(posiciones_actual.x,posiciones_actual.y);
                if(t == 1.f )
                    path->id_curva++;
                    //path->frame_actual_curva=0;
                //std::cout<<"[ "<<o<<" ] "<<path->frame_actual_curva<<"\n";
                if(path->id_curva == 2 && o->tieneComponente<CE::IShader>())
                {
                    //cambiar shader
                    auto shader = o->getComponente<CE::IShader>();
                    shader->cambiarShader("",ASSETS "/shaders/color.frag");
                    static sf::Glsl::Vec4 color{0.f,1.f,0.f,1.f};
                    shader->m_vars.clear();
                    shader->setVector4("color",&color);
                }
                else
                {
                    //cambiar shader
                    auto shader = o->getComponente<CE::IShader>();
                    shader->cambiarShader("",ASSETS "/shaders/contorno.frag");
                    static sf::Glsl::Vec4 outlinecolor{0.f,1.f,0.f,1.f};
                    static float thickness = 0.1;
                    static sf::Glsl::Vec2 size{63,92};
                    shader->m_vars.clear();
                    shader->setVector4("uOutlineColor",&outlinecolor);
                    shader->setEscalar("uThickness",&thickness);
                    shader->setVector2("uTexSize",&size);
                    
                }
            }
        }
    }

    //Dialogos
    void SistemaDialogos(IVJ::IDialogo* dialogo,CE::Objeto& obj)
    {
        if(!dialogo) return;
        dialogo->activo = true;
        dialogo->onInteractuar(obj);
    }

    void SistemaGirar(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IGirar>()) return;
        auto componente = ente.getComponente<IGirar>();
        auto pos = ente.getTransformada()->posicion;
        float radio = componente->radio;
        float x = radio *cos(componente->angulo);
        float y = radio *sin(componente->angulo);
        ente.setPosicion(pos.x+x,pos.y+y);
        componente->angulo+= 3.146f*dt;
    }

    void SistemaArribaAbajo(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IArribaAbajo>()) return;
        auto componente = ente.getComponente<IArribaAbajo>();
        auto pos = ente.getTransformada()->posicion;
        float dist = componente->distancia;
        float y = dist *sin(componente->angulo);
        ente.setPosicion(pos.x,pos.y+y);
        componente -> angulo += 3.146f*dt;
    }

    void SistemaOnda(CE::Objeto &ente, float dt)
    {
        if(!ente.getComponente<IOnda>()) return;
        auto componente = ente.getComponente<IOnda>();
        auto pos = ente.getTransformada()->posicion;

        float amp = componente->amplitud;
        float frc = componente->frecuencia;
        float y = amp *sin(componente->angulo);

        ente.setPosicion(pos.x+frc,pos.y+y);
        componente -> angulo += 3.146f*dt;
    }

    void SistemaGravedad(const std::shared_ptr<CE::Objeto>& objeto, float dt)
    {
        if(!objeto->tieneComponente<IGravedad>()) return;
        auto gravedad = objeto->getComponente<IGravedad>();
        auto transform = objeto->getTransformada();
        if(!transform) return;

        gravedad->en_suelo = false;

        gravedad->velocidad_Y += gravedad->fuerza * dt;
        if (gravedad->velocidad_Y > gravedad->max_vel)
        {
            gravedad->velocidad_Y = gravedad->max_vel;
        }
        transform->posicion.y += gravedad->velocidad_Y * dt;
    }

    void SistemaGravedad(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt)
    {
        for(auto& ente : entes)
        {
            SistemaGravedad(ente, dt);
        }
    }

    void SistemaGolpe(const std::shared_ptr<Entidad>& jugador, CE::Pool& objetos)
    {
        auto maquina_estado = jugador->getComponente<IMaquinaEstado>();
        if (!maquina_estado || !maquina_estado->fsm) return;

        bool isPunch = (maquina_estado->fsm->getNombre() == "GolpearJugador");
        bool isKick  = (maquina_estado->fsm->getNombre() == "PatearJugador");

        if (!isPunch && !isKick) return;

        bool hitbox_activa = false;
        bool golpe_procesado = false;
        
        if (isPunch) {
            auto gj = dynamic_cast<GolpearJugador*>(maquina_estado->fsm.get());
            hitbox_activa = gj->hitbox_activa;
            golpe_procesado = gj->golpe_procesado;
        } else {
            auto pj = dynamic_cast<PatearJugador*>(maquina_estado->fsm.get());
            hitbox_activa = pj->hitbox_activa;
            golpe_procesado = pj->golpe_procesado;
        }

        if (!hitbox_activa || golpe_procesado) return;

        auto trans  = jugador->getTransformada();
        auto sprite = jugador->getComponente<CE::ISprite>();
        float dir   = (sprite->m_sprite.getScale().x > 0) ? 1.0f : -1.0f;

        const float hitW = isKick ? 30.f : 20.f; 
        const float hitH = isKick ? 20.f : 15.f; 

        CE::Vector2D centro_hitbox = trans->posicion;
        centro_hitbox.x += dir * (hitW + 5.f);

        for (auto& obj : objetos.getPool())
        {
            if (obj == jugador) continue; // No golpearse a sí mismo
            if (!obj->tieneComponente<IGravedad>()) continue;
            if (!obj->tieneComponente<CE::IBoundingBox>()) continue;

            auto midObj = obj->getComponente<CE::IBoundingBox>()->mitad;
            auto posObj = obj->getTransformada()->posicion;

            float dX = std::abs(posObj.x - centro_hitbox.x);
            float dY = std::abs(posObj.y - centro_hitbox.y);

            if ((dX < hitW + midObj.x) && (dY < hitH + midObj.y))
            {
                auto& stats = obj->getStats();
                stats->porcentaje_danio += isKick ? 15.0f : 10.0f;
                stats->hit_count++;

                float fuerza_base_H = isKick ? 200.0f : 100.0f; 
                float fuerza_base_V = isKick ? 200.0f : 150.0f;
                
                float multiplicador_smash = (1.0f + (stats->porcentaje_danio / 100.0f));

                obj->getTransformada()->velocidad.x = dir * fuerza_base_H * multiplicador_smash;
                auto grav = obj->getComponente<IGravedad>();
                if (grav) {
                    grav->velocidad_Y = -fuerza_base_V * multiplicador_smash;
                    grav->en_suelo = false;
                }

                if (isPunch) dynamic_cast<GolpearJugador*>(maquina_estado->fsm.get())->golpe_procesado = true;
                else dynamic_cast<PatearJugador*>(maquina_estado->fsm.get())->golpe_procesado = true;
            }
        }
    }

    void SistemaDibujarGolpe(const std::shared_ptr<Entidad>& jugador)
    {
#if DEBUG
        auto maquina_estado = jugador->getComponente<IMaquinaEstado>();
        if (!maquina_estado || !maquina_estado->fsm) return;

        bool isPunch = (maquina_estado->fsm->getNombre() == "GolpearJugador");
        bool isKick  = (maquina_estado->fsm->getNombre() == "PatearJugador");
        if (!isPunch && !isKick) return;

        bool activa = false;
        bool procesado = false;
        if (isPunch) {
            auto gj = dynamic_cast<GolpearJugador*>(maquina_estado->fsm.get());
            activa = gj->hitbox_activa;
            procesado = gj->golpe_procesado;
        } else {
            auto pj = dynamic_cast<PatearJugador*>(maquina_estado->fsm.get());
            activa = pj->hitbox_activa;
            procesado = pj->golpe_procesado;
        }

        if (!activa) return;

        auto trans = jugador->getTransformada();
        auto sprite = jugador->getComponente<CE::ISprite>();
        float dir = (sprite->m_sprite.getScale().x > 0) ? 1.0f : -1.0f;

        const float hitW = isKick ? 30.f : 20.f; 
        const float hitH = isKick ? 20.f : 15.f; 

        CE::Vector2D centro = trans->posicion;
        centro.x += dir * (hitW + 5.f);

        sf::RectangleShape debug_hitbox(sf::Vector2f(hitW * 2.f, hitH * 2.f));
        debug_hitbox.setOrigin(sf::Vector2f(hitW, hitH));
        debug_hitbox.setPosition(sf::Vector2f(centro.x, centro.y));
        
        if (procesado)
        {
            debug_hitbox.setFillColor(sf::Color(255, 140, 0, 60));
            debug_hitbox.setOutlineColor(sf::Color(255, 140, 0, 200));
        }
        else
        {
            debug_hitbox.setFillColor(sf::Color(255, 50, 50, 80));
            debug_hitbox.setOutlineColor(sf::Color(255, 50, 50, 220));
        }
        debug_hitbox.setOutlineThickness(2.f);
        CE::Render::Get().AddToDraw(debug_hitbox);
#endif
    }


    void SistemaEpidemia(const MatrizEntes& matriz, float dt)
    {
        if (matriz.empty()) return;
        int filas = matriz.size();
        int columnas = matriz[0].size();

        // Direcciones: [Arriba, Abajo, Izquierda, Derecha]
        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};

        for (int y = 0; y < filas; ++y)
        {
            for (int x = 0; x < columnas; ++x)
            {
                auto ente = matriz[y][x];
                if (!ente) continue;
                auto salud = ente->getComponente<IEstadoSalud>();
                if (!salud) continue;

                salud->estado_siguiente = salud->estado_actual;
                salud->tiempo_en_estado += dt;

                // 1. LÓGICA: NORMAL
                if (salud->estado_actual == TipoSalud::Normal) {
                    // Descontar timer de inmunidad parcial si la tiene
                    if (salud->inmunidad_parcial_timer > 0.0f) {
                        salud->inmunidad_parcial_timer -= dt;
                    }
                }
                // 2. LÓGICA: ENFERMO
                else if (salud->estado_actual == TipoSalud::Enfermo) {
                    salud->timer_infeccion += dt;

                    // Cada 1 segundo, intenta contagiar a sus vecinos
                    if (salud->timer_infeccion >= 1.0f) {
                        salud->timer_infeccion -= 1.0f; // Reinicia el tick

                        for (int i = 0; i < 4; ++i) {
                            int ny = y + dy[i];
                            int nx = x + dx[i];

                            if (ny >= 0 && ny < filas && nx >= 0 && nx < columnas) {
                                auto vecino = matriz[ny][nx];
                                if (vecino) {
                                    auto s_vecino = vecino->getComponente<IEstadoSalud>();
                                    // Solo puede infectar si el vecino es Normal y NO tiene inmunidad parcial
                                    if (s_vecino && s_vecino->estado_actual == TipoSalud::Normal && s_vecino->estado_siguiente == TipoSalud::Normal) {
                                        if (s_vecino->inmunidad_parcial_timer <= 0.0f) {
                                            // Tira los dados contra la probabilidad individual del vecino
                                            if ((rand() % 100) < s_vecino->prob_contagio) {
                                                s_vecino->estado_siguiente = TipoSalud::Enfermo;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // A los 3 segundos exactos, pasa a recuperación
                    if (salud->tiempo_en_estado >= 3.0f) {
                        salud->estado_siguiente = TipoSalud::Recuperacion;
                    }
                }
                // 3. LÓGICA: RECUPERACIÓN
                else if (salud->estado_actual == TipoSalud::Recuperacion) {
                    // A los 2 segundos exactos, se decide su destino
                    if (salud->tiempo_en_estado >= 2.0f) {
                        if ((rand() % 100) < 10) {
                            // 10% de probabilidad de morir
                            salud->estado_siguiente = TipoSalud::Muerto;
                        } else {
                            // Si sobrevive, checa si obtiene inmunidad permanente
                            if ((rand() % 100) < salud->prob_inmunidad) {
                                salud->estado_siguiente = TipoSalud::Inmune;
                            } else {
                                // Si no, vuelve a Normal pero con inmunidad de 2 segundos
                                salud->estado_siguiente = TipoSalud::Normal;
                                salud->inmunidad_parcial_timer = 2.0f;
                            }
                        }
                    }
                }
            }
        }
    }

    void SistemaActualizarSalud(const MatrizEntes& matriz)
    {
        for (const auto& fila : matriz) {
            for (const auto& ente : fila) {
                if (!ente) continue;
                auto salud = ente->getComponente<IEstadoSalud>();
                auto visual = ente->getComponente<IVisualSalud>();

                if (salud && visual) {
                    if (salud->estado_actual != salud->estado_siguiente) {
                        salud->estado_actual = salud->estado_siguiente;

                        salud->tiempo_en_estado = 0.0f;
                        salud->timer_infeccion = 0.0f;

                        if (visual->figura) {
                            visual->figura->setRelleno(visual->colores[salud->estado_actual]);
                        }
                    }
                }
            }
        }
    }

    bool revisarDistanciaInteraccion(CE::Objeto& jugador, CE::Objeto& npc, float distancia_maxima)
    {
        auto pos_jugador = jugador.getTransformada()->posicion;
        auto pos_npc = npc.getTransformada()->posicion;
        float distancia = pos_jugador.distancia(pos_npc);
        return distancia <= distancia_maxima;
    }

    void SistemaNPCLookTarget(CE::Objeto &ente, CE::Vector2D& target)
    {
        if(!ente.tieneComponente<IRangoAggro>()) return;

        auto rango = ente.getComponente<IRangoAggro>();
        if (!rango->estaDentroRango(ente,target)) return;

        auto pos = ente.getTransformada()->posicion;
        auto ente_a_target =  target-pos;
        ente.getTransformada()->angulo = atan2(
                ente_a_target.y,
                ente_a_target.x
            );
        //reflejar imagen
        if (ente_a_target.x <0)
            ente.getComponente<CE::ISprite>()->m_sprite.setScale({1,-1});
        else
            ente.getComponente<CE::ISprite>()->m_sprite.setScale({1,1});
    }

    //DEBUG
    void pintarLinea(CE::Vector2D& p1, CE::Vector2D& p2, const sf::Color& color)
    {
        sf::CircleShape cp1{2.f};
        cp1.setFillColor({0,0,0,255});
        sf::CircleShape cp2{2.f};
        cp2.setFillColor({0,0,0,255});
        cp1.setPosition({p1.x,p1.y});
        cp2.setPosition({p2.x,p2.y});
        //pintar lineas de guía
        for(float t=0;t<=1;t+=0.0015)
        {
            sf::CircleShape pixel{1};
            pixel.setFillColor(color);
            auto puntopos  = CE::lerp(p1,p2,t);
            pixel.setPosition({puntopos.x,puntopos.y});
            CE::Render::Get().AddToDraw(pixel);
        }
        CE::Render::Get().AddToDraw(cp1);
        CE::Render::Get().AddToDraw(cp2);
    }

    void SistemaLimitesPantalla(const std::vector<std::shared_ptr<CE::Objeto>>& entes)
    {
        auto& camara = CE::GestorCamaras::Get().getCamaraActiva();
        sf::View view = camara.getView();
        sf::Vector2f center = view.getCenter();
        sf::Vector2f size = view.getSize();

        float left = center.x - size.x / 2.0f;
        float right = center.x + size.x / 2.0f;
        float top = center.y - size.y / 2.0f;
        float bottom = center.y + size.y / 2.0f;

        for (auto& ente : entes) {
            if (!ente) continue;
            CE::Vector2D pos = ente->getTransformada()->posicion;
            
            bool offScreen = false;
            sf::Vector2f indicatorPos = {pos.x, pos.y};

            if (pos.x < left) { offScreen = true; indicatorPos.x = left + 20; }
            else if (pos.x > right) { offScreen = true; indicatorPos.x = right - 20; }
            
            if (pos.y < top) { offScreen = true; indicatorPos.y = top + 20; }
            else if (pos.y > bottom) { offScreen = true; indicatorPos.y = bottom - 20; }

            if (offScreen) {
                // Dibujar indicador estilo Smash
                sf::CircleShape indicator(15.0f);
                indicator.setOrigin({15.0f, 15.0f});
                indicator.setPosition(indicatorPos);
                indicator.setFillColor(sf::Color::Red);
                indicator.setOutlineThickness(2.0f);
                indicator.setOutlineColor(sf::Color::White);
                CE::Render::Get().AddToDraw(indicator);

                // Dibujar una pequeña línea apuntando al jugador usando VertexArray
                sf::VertexArray line(sf::PrimitiveType::Lines, 2);
                line[0].position = indicatorPos;
                line[0].color = sf::Color::White;
                line[1].position = sf::Vector2f(pos.x, pos.y);
                line[1].color = sf::Color(255, 255, 255, 50);
                CE::Render::Get().AddToDraw(line);
            }
        }
    }

    struct BlastParticle {
        sf::RectangleShape shape;
        sf::Vector2f velocity;
        float lifetime;
        float maxLifetime;
    };
    static std::vector<BlastParticle> blastParticles;

    void SistemaUpdateParticulasMuerte(float dt)
    {
        for (auto it = blastParticles.begin(); it != blastParticles.end();) {
            it->lifetime -= dt;
            if (it->lifetime <= 0) {
                it = blastParticles.erase(it);
            } else {
                it->shape.move(it->velocity * dt);
                // Desvanecimiento
                float alpha = (it->lifetime / it->maxLifetime) * 255.0f;
                sf::Color c = it->shape.getFillColor();
                c.a = static_cast<std::uint8_t>(alpha);
                it->shape.setFillColor(c);
                ++it;
            }
        }
    }

    void SistemaDibujarParticulasMuerte()
    {
        for (auto& p : blastParticles) {
            CE::Render::Get().AddToDraw(p.shape);
        }
    }

    void GenerarEfectoMuerte(sf::Vector2f pos, sf::Vector2f dir)
    {
        // Normalizar dirección para tener una referencia
        float mag = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (mag > 0) dir /= mag;

        // Crear una explosión de líneas hacia arriba (50 partículas)
        for (int i = 0; i < 50; ++i) {
            BlastParticle p;
            p.maxLifetime = 0.8f + (rand() % 80) / 100.0f; 
            p.lifetime = p.maxLifetime;
            
            // Líneas delgadas y largas (estilo Smash K.O.)
            float width = 2.0f + (rand() % 4);
            float height = 20.0f + (rand() % 60);
            p.shape.setSize({width, height});
            p.shape.setOrigin({width / 2.0f, height / 2.0f});
            p.shape.setPosition(pos);
            
            // Colores vibrantes
            int r = 255;
            int g = 100 + rand() % 155;
            int b = 0;
            p.shape.setFillColor(sf::Color(r, g, b, 255));
            
            // Velocidad: Principalmente hacia arriba (bias negativo en Y)
            float speedX = ((rand() % 200) - 100.0f); // Poco movimiento lateral
            float speedY = -(600.0f + (rand() % 800)); // Mucho movimiento hacia arriba
            
            p.velocity = {speedX, speedY};
            
            // Rotación inicial leve para dinamismo
            p.shape.setRotation(sf::degrees(((rand() % 20) - 10.0f)));
            
            blastParticles.push_back(p);
        }
    }

    void SistemaMuerteBlastZone(const std::vector<std::shared_ptr<CE::Objeto>>& entes, sf::Vector2f centro, float radio, float dt)
    {
        // Dibujar el círculo del Blast Zone en modo Debug si se desea
#if DEBUG
        sf::CircleShape zone(radio);
        zone.setOrigin({radio, radio});
        zone.setPosition(centro);
        zone.setFillColor(sf::Color(255, 0, 0, 20));
        zone.setOutlineThickness(5.0f);
        zone.setOutlineColor(sf::Color(255, 0, 0, 50));
        CE::Render::Get().AddToDraw(zone);
#endif

        for (auto& ente : entes) {
            if (!ente) continue;
            auto trans = ente->getTransformada();
            sf::Vector2f pos = {trans->posicion.x, trans->posicion.y};
            sf::Vector2f diff = pos - centro;
            float distSq = diff.x * diff.x + diff.y * diff.y;

            if (distSq > radio * radio) {
                // MUERTE
                sf::Vector2f deathDir = diff; // Dirección hacia afuera
                GenerarEfectoMuerte(pos, deathDir);

                // Respawn
                trans->posicion = {centro.x, centro.y - 100.0f};
                trans->velocidad = {0, 0};
                
                if (ente->tieneComponente<IGravedad>()) {
                    ente->getComponente<IGravedad>()->velocidad_Y = 0;
                }

                auto stats = ente->getStats();
                if (stats) {
                    stats->porcentaje_danio = 0;
                    stats->hit_count = 0;
                }
            }
        }
        
        SistemaUpdateParticulasMuerte(dt);
    }
}

