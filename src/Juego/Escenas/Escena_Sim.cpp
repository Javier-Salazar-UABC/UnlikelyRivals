#include "Escena_Sim.hpp"
#include <Juego/objetos/Persona.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include<Juego/Componentes/IJComponentes.hpp>
#include <Juego/Sistemas/Sistemas.hpp>
#include <Juego/objetos/Texto.hpp>
#include <Motor/Render/Render.hpp>
#include <Motor/Primitivos/GestorAssets.hpp>
#include <Motor/GUI/GLogger.hpp>

namespace IVJ
{
    Escena_Sim::Escena_Sim()
        : CE::Escena{}
    {
    }
            
    void Escena_Sim::onInit()
    {
        if(!inicializar)
            return;
        int filas = 15;
        int columnas = 15;
        float tam = 40.0f; // Tamaño de cada cuadrito

        matriz_personas.resize(filas, std::vector<std::shared_ptr<CE::Objeto>>(columnas, nullptr));

        // 1. Crear las personas
        for (int y = 0; y < filas; ++y) {
            for (int x = 0; x < columnas; ++x) {
                // Se pintará contorno negro
                auto persona = std::make_shared<Persona>(tam, tam, sf::Color::Transparent, sf::Color::Black);
                persona->setPosicion(x * (tam + 2) + 50.0f, y * (tam + 2) + 50.0f); // (+2) para separarlos un poco

                persona->addComponente(std::make_shared<IEstadoSalud>());
                auto visual = std::make_shared<IVisualSalud>();
                visual->figura = persona.get();
                persona->addComponente(visual);

                matriz_personas[y][x] = persona;
                objetos.agregarPool(persona);
            }
        }

        // 2. Infectar exactamente al 10% aleatoriamente
        int total_pob = filas * columnas;
        int infectados_necesarios = total_pob * 0.10f;
        int contagiados = 0;

        while (contagiados < infectados_necesarios) {
            int ry = rand() % filas;
            int rx = rand() % columnas;
            auto salud = matriz_personas[ry][rx]->getComponente<IEstadoSalud>();

            if (salud->estado_actual == TipoSalud::Normal) {
                salud->estado_actual = TipoSalud::Enfermo;
                salud->estado_siguiente = TipoSalud::Enfermo;
                matriz_personas[ry][rx]->getComponente<IVisualSalud>()->figura->setRelleno(sf::Color(255, 20, 0));
                contagiados++;
            }
        }

        // 3. Crear el Texto UI
        texto_estadisticas = std::make_shared<IVJ::Texto>(CE::GestorAssets::Get().getFont("default_font"), "Cargando...");
        texto_estadisticas->setFontSize(20u);
        texto_estadisticas->setPosicion(750.0f, 100.0f); // Ponlo a la derecha del grid
        texto_estadisticas->setColor(sf::Color(0,0,0));
        objetos.agregarPool(texto_estadisticas);
    }
    void Escena_Sim::onFinal()
    {

    }
    void Escena_Sim::onUpdate(float dt)
    {
        // 1. ESCUDO ANTI-CRASHEOS:
        // Si la matriz aún no se crea o el texto aún no existe, nos salimos inmediatamente.
        if (matriz_personas.empty() || !texto_estadisticas) return;

        // Ya sabemos que es seguro leer el tamaño
        int filas = matriz_personas.size();
        int columnas = matriz_personas[0].size();

        if (simulacion_activa) {
            IVJ::SistemaEpidemia(matriz_personas, dt);
            IVJ::SistemaActualizarSalud(matriz_personas);
        }

        // Actualizar animaciones genéricas
        for (auto& obj : objetos.getPool()) {
            obj->onUpdate(dt);
        }

        // Contar los estados para la UI
        int c_nor = 0, c_enf = 0, c_rec = 0, c_inm = 0, c_mue = 0;
        for (const auto& fila : matriz_personas) {
            for (const auto& p : fila) {
                if(!p) continue; // Por si acaso algún puntero está nulo

                auto s = p->getComponente<IVJ::IEstadoSalud>();
                if (s) {
                    if (s->estado_actual == IVJ::TipoSalud::Normal) c_nor++;
                    else if (s->estado_actual == IVJ::TipoSalud::Enfermo) c_enf++;
                    else if (s->estado_actual == IVJ::TipoSalud::Recuperacion) c_rec++;
                    else if (s->estado_actual == IVJ::TipoSalud::Inmune) c_inm++;
                    else if (s->estado_actual == IVJ::TipoSalud::Muerto) c_mue++;
                }
            }
        }

        // Preparar el Texto
        std::string info = "POBLACION TOTAL: " + std::to_string(filas * columnas) + "\n\n" +
                           "Normales: " + std::to_string(c_nor) + "\n" +
                           "Enfermos: " + std::to_string(c_enf) + "\n" +
                           "En Recuperacion: " + std::to_string(c_rec) + "\n" +
                           "Inmunes: " + std::to_string(c_inm) + "\n" +
                           "Muertos: " + std::to_string(c_mue);

        // Checar fin del programa
        if ((c_enf == 0 && c_rec == 0)|| c_mue == (filas * columnas)) {
            simulacion_activa = false;
            info += "\n\n--- FIN DE LA PANDEMIA ---";
        }

        texto_estadisticas->setString(info);
    }
    void Escena_Sim::onInputs(const CE::Botones& accion)
    {
        (void)accion;
    }
    void Escena_Sim::onRender()
    {
        for (auto&obj : objetos.getPool())
            CE::Render::Get().AddToDraw(*obj);
    }
}