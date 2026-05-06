#pragma once
#include <map>
#include <string>
#include <vector>
#include "../Componentes/IComponentes.hpp"

namespace CE
{
    /**
     * @class GestorColisiones
     * @brief Singleton que gestiona la matriz de colisiones binaria.
     * 
     * Permite definir qué capas pueden colisionar entre sí mediante una máscara de bits.
     * Carga la configuración desde un archivo externo para actualizaciones en tiempo real.
     */
    class GestorColisiones
    {
    public:
        /** @brief Obtiene la instancia única del gestor */
        static GestorColisiones& Get();

        /** 
         * @brief Carga la configuración de colisiones desde un archivo.
         * @param path Ruta al archivo (ej. assets/config/collisions.txt)
         */
        void cargarConfiguracion(const std::string& path);

        /**
         * @brief Verifica si dos capas pueden colisionar.
         * @param a Capa del primer objeto
         * @param b Capa del segundo objeto
         * @return true si la colisión está permitida
         */
        bool puedenColisionar(CollisionLayer a, CollisionLayer b) const;

        /**
         * @brief Obtiene la máscara de colisión para una capa específica.
         * @param layer Capa a consultar
         * @return Máscara de bits con las capas permitidas
         */
        uint32_t getMaskParaCapa(CollisionLayer layer) const;

        /** @brief Recarga la configuración desde el último archivo cargado */
        void recargar();

    private:
        GestorColisiones();
        ~GestorColisiones() = default;
        GestorColisiones(const GestorColisiones&) = delete;
        GestorColisiones& operator=(const GestorColisiones&) = delete;

        std::map<CollisionLayer, uint32_t> matriz;
        std::string ultimoPath;

        CollisionLayer stringToLayer(const std::string& name) const;
    };
}
