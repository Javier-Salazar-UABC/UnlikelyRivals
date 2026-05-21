#pragma once
#include "../FSM.hpp"

namespace IVJ
{
    class DownBJugador : public FSM
    {
    public:
        DownBJugador();
        ~DownBJugador() override{};
        FSM* onInputs(const Entidad& obj, const CE::IControl& control) override;
        void onEntrar(const Entidad& obj) override;
        void onSalir(const Entidad& obj) override;
        void onUpdate(const Entidad& obj, float dt) override;

    public:
        bool hitbox_activa{false};
        bool golpe_procesado{false};

    private:
        sf::Sprite *sprite;
        int s_w;
        int s_h;
        float max_tiempo;   // frame rate
        float act_tiempo;   // frame rate actual
        int max_frames;     // frames de animación (6)
        int id_frame;       // frame actual

        bool en_aire;
        bool descenso_rapido;
        bool impacto_suelo_reproducido;
    };
}
