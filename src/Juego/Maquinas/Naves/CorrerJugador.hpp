#pragma once
#include <Juego/Maquinas/FSM.hpp>
#include <Juego/Componentes/IJComponentes.hpp>

namespace IVJ
{
    class CorrerJugador : public FSM
    {
        public:
            CorrerJugador(int max_frames,float frame_rate);
            FSM* onInputs(const Entidad& obj, const CE::IControl& control) override;
            void onEntrar(const Entidad& obj) override;
            void onSalir(const Entidad& obj) override;
            void onUpdate(const Entidad& obj,float dt) override;
        private:
            sf::Sprite *sprite;
            int s_w;
            int s_h;
            float max_tiempo;
            float act_tiempo;
            int id_frame{0};
            int max_frames;
    };
}
