#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include "Juego/objetos/Entidad.hpp"
#include "Juego/Figuras/Figuras.hpp"
namespace IVJ
{
    class Escena_SpriteTiles : public CE::Escena
    {
    public:
        explicit Escena_SpriteTiles(std::shared_ptr<Entidad>& pref);
        virtual ~Escena_SpriteTiles(){};
        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;
        std::shared_ptr<Entidad> getJugador() override {return player;}
    private:
        int inicializar{1};
        std::shared_ptr<Entidad>& player;
        std::shared_ptr<Entidad> obstaculo;
    };
}
