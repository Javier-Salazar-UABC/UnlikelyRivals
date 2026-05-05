#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <memory>
#include "Juego/objetos/Texto.hpp"


namespace IVJ
{
    class Escena_Sim : public CE::Escena
    {
    public:

        explicit Escena_Sim();
        virtual ~Escena_Sim(){};
        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;
    private:
        int inicializar{1};
        std::vector<std::vector<std::shared_ptr<CE::Objeto>>> matriz_personas;
        std::shared_ptr<IVJ::Texto> texto_estadisticas;
        bool simulacion_activa{true};
    };
}
