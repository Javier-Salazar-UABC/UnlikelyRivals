#pragma once
#include "../FSM.hpp"
namespace IVJ
{
    class SaltarJugador : public FSM
    {
    public:
        SaltarJugador();
        ~SaltarJugador() override{};
        FSM* onInputs(const Entidad& obj, const CE::IControl& control)override;
        void onEntrar(const Entidad& obj)override;
        void onSalir(const Entidad& obj)override;
        void onUpdate(const Entidad& obj,float dt)override;
    private:
        sf::Sprite *sprite;
        int s_w;
        int s_h;
        bool salto_soltado;
        float act_tiempo;
        int id_frame;
        bool aterrizando;
        float tiempo_aterrizaje;
    };
}
