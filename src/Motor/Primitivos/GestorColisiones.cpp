#include "GestorColisiones.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace CE
{
    GestorColisiones& GestorColisiones::Get()
    {
        static GestorColisiones instance;
        return instance;
    }

    GestorColisiones::GestorColisiones()
    {
        // Configuración por defecto: todo colisiona con todo
        // Se puede sobreescribir cargando un archivo
    }

    void GestorColisiones::cargarConfiguracion(const std::string& path)
    {
        ultimoPath = path;
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "[GestorColisiones] Error al abrir: " << path << std::endl;
            return;
        }

        matriz.clear();
        std::string line;
        while (std::getline(file, line))
        {
            // Limpiar comentarios y espacios
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) line = line.substr(0, commentPos);
            
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string layerName;
            if (!(ss >> layerName)) continue;

            // Quitar el ':' si existe
            if (layerName.back() == ':') layerName.pop_back();

            CollisionLayer mainLayer = stringToLayer(layerName);
            if (mainLayer == CollisionLayer::NONE) continue;

            uint32_t mask = 0;
            std::string otherLayerName;
            while (ss >> otherLayerName)
            {
                // Soporte para comas si el usuario las usa
                if (otherLayerName.back() == ',') otherLayerName.pop_back();
                
                CollisionLayer otherLayer = stringToLayer(otherLayerName);
                if (otherLayer != CollisionLayer::NONE)
                {
                    mask |= (uint32_t)otherLayer;
                }
                else if (otherLayerName == "ALL")
                {
                    mask = (uint32_t)CollisionLayer::ALL;
                }
            }
            matriz[mainLayer] = mask;
        }
        std::cout << "[GestorColisiones] Configuración cargada desde " << path << std::endl;
    }

    bool GestorColisiones::puedenColisionar(CollisionLayer a, CollisionLayer b) const
    {
        auto it = matriz.find(a);
        if (it != matriz.end())
        {
            return (it->second & (uint32_t)b) != 0;
        }
        // Si no está en la matriz, por defecto colisiona con todo (ALL)
        return true; 
    }

    uint32_t GestorColisiones::getMaskParaCapa(CollisionLayer layer) const
    {
        auto it = matriz.find(layer);
        if (it != matriz.end()) return it->second;
        return (uint32_t)CollisionLayer::ALL;
    }

    void GestorColisiones::recargar()
    {
        if (!ultimoPath.empty()) cargarConfiguracion(ultimoPath);
    }

    CollisionLayer GestorColisiones::stringToLayer(const std::string& name) const
    {
        std::string upper = name;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

        if (upper == "PLAYER") return CollisionLayer::PLAYER;
        if (upper == "ENEMY") return CollisionLayer::ENEMY;
        if (upper == "PLAYER_ATTACK") return CollisionLayer::PLAYER_ATTACK;
        if (upper == "ENEMY_ATTACK") return CollisionLayer::ENEMY_ATTACK;
        if (upper == "GROUND") return CollisionLayer::GROUND;
        if (upper == "PROJECTILE") return CollisionLayer::PROJECTILE;
        if (upper == "TRIGGER") return CollisionLayer::TRIGGER;
        if (upper == "ALL") return CollisionLayer::ALL;
        
        return CollisionLayer::NONE;
    }
}
