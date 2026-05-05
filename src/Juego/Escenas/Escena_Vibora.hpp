#pragma once
#include <Motor/Primitivos/Escena.hpp>
#include <Juego/Figuras/Figuras.hpp>
#include <memory>
// 1. Añadimos el include de tu EnteVibora
#include "../objetos/EnteVibora.hpp"

namespace IVJ
{
    class Escena_Vibora : public CE::Escena
    {
    public:
        explicit Escena_Vibora(std::shared_ptr<Entidad>& pref);
        virtual ~Escena_Vibora(){};
        Escena_Vibora();

        void onInit() override;
        void onFinal() override;
        void onUpdate(float dt) override;
        void onInputs(const CE::Botones& accion) override;
        void onRender() override;
    private:
        int inicializar{1};

        // 2. Apuntador a la cabeza de la víbora
        std::shared_ptr<EnteVibora> cabeza;

        // 3. Temporizador para agregar partes cada 3 segundos
        float timer_nueva_parte{0.0f};
    };
}