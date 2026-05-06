#pragma once
#include "../FSM.hpp"
namespace IVJ
{
    class IdleJugadores : public FSM
    {
    public:
        IdleJugadores(int max_frames, float frame_rate);
        ~IdleJugadores() override{};
        FSM* onInputs(const Entidad& obj, const CE::IControl& control)override;
        void onEntrar(const Entidad& obj)override;
        void onSalir(const Entidad& obj)override;
        void onUpdate(const Entidad& obj,float dt)override;
    private:
        sf::Sprite *sprite;
        int s_w;
        int s_h;
        float max_tiempo;   //frame rate
        float act_tiempo;   //frame rate actual
        int max_frames;     //frames de animación
        int id_frame;       //frame actual de animación
    };
}