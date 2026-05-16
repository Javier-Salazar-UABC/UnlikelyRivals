#pragma once
#include "../FSM.hpp"

namespace IVJ
{
    class RecibirGolpe : public FSM
    {
    public:
        RecibirGolpe();
        ~RecibirGolpe() override = default;
        FSM* onInputs(const Entidad& obj, const CE::IControl& control) override;
        void onEntrar(const Entidad& obj) override;
        void onSalir(const Entidad& obj) override;
        void onUpdate(const Entidad& obj, float dt) override;

    private:
        sf::Sprite* sprite;
        int s_w, s_h;
        int id_frame;
        float act_tiempo;
        float max_tiempo;
        int max_frames;
    };
}
