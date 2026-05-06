#pragma once
#include "../../Motor/Primitivos/Objetos.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../Componentes/IJComponentes.hpp"
#include "../../Motor/Primitivos/CEPool.hpp"
#include "../objetos/Entidad.hpp"
#include "../Maquinas/Naves/GolpearJugador.hpp"

namespace IVJ
{
    [[maybe_unused]]void SistemaControl(CE::Objeto&ente, float dt);
    [[maybe_unused]]void SistemaMover(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt);
    [[maybe_unused]]void SistemaMover(const std::shared_ptr<CE::Objeto>& objeto, float dt);
    [[maybe_unused]]bool SistemaColAABB(CE::Objeto& A,CE::Objeto& B, bool resolucion =false);
    [[maybe_unused]]bool SistemaColAABBMid(CE::Objeto& A, CE::Objeto& B, bool resolucion =false);


    [[maybe_unused]]void SistemaSpawn(CE::Pool& spwans);

    [[maybe_unused]]void SistemaPaths(std::vector<std::shared_ptr<CE::Objeto>>& obj);

    [[maybe_unused]]bool checkRayoHit(CE::Objeto& npc, CE::Vector2D& p1, CE::Vector2D& p2);

    //void SistemaDialogos(CE::Objeto& npc);
    [[maybe_unused]]void SistemaDialogos(IVJ::IDialogo* dialogo, CE::Objeto& obj);

    [[maybe_unused]] void SistemaGirar(CE::Objeto &ente, float dt);

    [[maybe_unused]] void SistemaArribaAbajo(CE::Objeto &ente, float dt);

    [[maybe_unused]] void SistemaOnda(CE::Objeto &ente, float dt);

    [[maybe_unused]] void SistemaGravedad(const std::shared_ptr<CE::Objeto>& objeto, float dt);
    [[maybe_unused]] void SistemaGravedad(const std::vector<std::shared_ptr<CE::Objeto>>& entes, float dt);


    using MatrizEntes = std::vector<std::vector<std::shared_ptr<CE::Objeto>>>;

    [[maybe_unused]] void SistemaEpidemia(const MatrizEntes& matriz, float dt);
    [[maybe_unused]] void SistemaActualizarSalud(const MatrizEntes& matriz);

    bool revisarDistanciaInteraccion(CE::Objeto& jugador, CE::Objeto& npc, float distancia_maxima);

    [[maybe_unused]] void SistemaNPCLookTarget(CE::Objeto &ente, CE::Vector2D& target);

    //DEBUG
    [[maybe_unused]]void pintarLinea(CE::Vector2D& p1, CE::Vector2D& p2, const sf::Color& color=sf::Color::Red);

    [[maybe_unused]] void SistemaGolpe(const std::shared_ptr<Entidad>& jugador, CE::Pool& objetos);
    [[maybe_unused]] void SistemaDibujarGolpe(const std::shared_ptr<Entidad>& jugador);
    [[maybe_unused]] void SistemaLimitesPantalla(const std::vector<std::shared_ptr<CE::Objeto>>& entes);
    [[maybe_unused]] void SistemaMuerteBlastZone(const std::vector<std::shared_ptr<CE::Objeto>>& entes, sf::Vector2f centro, float radio, float dt);
    [[maybe_unused]] void SistemaUpdateParticulasMuerte(float dt);
    [[maybe_unused]] void SistemaDibujarParticulasMuerte();
}
