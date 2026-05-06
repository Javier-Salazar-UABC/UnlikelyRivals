#pragma once
#include "../FSM.hpp"

namespace IVJ
{
    class PatearJugador : public FSM
    {
    public:
        PatearJugador(int max_frames, float frame_rate);
        ~PatearJugador() override{};
        FSM* onInputs(const Entidad& obj, const CE::IControl& control)override;
        void onEntrar(const Entidad& obj)override;
        void onSalir(const Entidad& obj)override;
        void onUpdate(const Entidad& obj,float dt)override;
    private:
        sf::Sprite *sprite;
        int s_w;
        int s_h;
        float max_tiempo;
        float act_tiempo;
        int max_frames;
        int id_frame;
    public:
        bool hitbox_activa;
        bool golpe_procesado;
    };
}
