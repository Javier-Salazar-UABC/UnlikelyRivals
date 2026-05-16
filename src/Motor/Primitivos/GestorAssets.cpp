#include "GestorAssets.hpp"
#include <iostream>
namespace CE
{
    GestorAssets& GestorAssets::Get()
    {
        if(!instancia)
            instancia = new GestorAssets();
        return *instancia;
    }
    void GestorAssets::agregarFont(const std::string& key, const std::string& filepath)
    {
        if(hashFonts.find(key) != hashFonts.end())
            return;

        auto font = std::make_shared<sf::Font>();
        if(!font->openFromFile(filepath))
        {
            std::cerr<<"No se pudo abrir el archivo "<<filepath<<"\n";
            exit(EXIT_FAILURE);
        }

        hashFonts[key] = font;
    }
    void GestorAssets::agregarTextura(const std::string& key, const std::string& filepath,
            const CE::Vector2D& pos_init,const CE::Vector2D& dim)
    {
        if(hashTexturas.find(key) != hashTexturas.end())
            return;

        auto textura = std::make_shared<sf::Texture>();
        if(!textura->loadFromFile(filepath,false,
                    sf::IntRect({(int)pos_init.x,(int)pos_init.y},{(int)dim.x,(int)dim.y})
                    )
        )
        {
            std::cerr<<"No se pudo cargar la textura "<<filepath<<"\n";
            exit(EXIT_FAILURE);
        }

        textura->setSmooth(false);
        hashTexturas[key] = textura;
    }
    
    void GestorAssets::agregarSonido(const std::string& key, const std::string& filepath)
    {   
        if(hashBuffers.find(key) != hashBuffers.end())
            return;

        auto buffer = std::make_shared<sf::SoundBuffer>();
        if(!buffer->loadFromFile(filepath))
            std::cerr<<"No se pudo cargar la Sonido "<<filepath<<"\n";
        
        hashBuffers[key] = buffer;
        hashSonidos[key] = std::make_shared<sf::Sound>(*buffer);
    }
    void GestorAssets::agregarMusica(const std::string& key, const std::string& filepath)
    {   
        auto musica = std::make_shared<sf::Music>();
        if(!musica->openFromFile(filepath))
            std::cerr<<"No se pudo cargar la Música "<<filepath<<"\n";
        hashMusica[key] = musica;
    }
    
}
