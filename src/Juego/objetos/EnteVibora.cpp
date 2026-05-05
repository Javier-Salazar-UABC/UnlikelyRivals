#include "EnteVibora.hpp"
#include "Juego/Componentes/IJComponentes.hpp"

EnteVibora::EnteVibora(float tam_lado, const sf::Color& relleno, const sf::Color& contorno)
    : Triangulo{tam_lado, relleno, contorno}
{
}

void EnteVibora::setDireccion(float dx, float dy)
{
    if (direccion.x != 0 && dx != 0) return;
    if (direccion.y != 0 && dy != 0) return;

    direccion.x = dx;
    direccion.y = dy;
}

void EnteVibora::onUpdate(float dt)
{
    auto transform_cabeza = this->getTransformada();

    // 1. Guardamos la posición vieja ANTES de mover la cabeza
    CE::Vector2D posicion_vieja_cabeza = transform_cabeza->posicion;

    // 2. Movimiento Fluido: Se multiplica por velocidad y dt
    transform_cabeza->posicion.x += direccion.x * velocidad * dt;
    transform_cabeza->posicion.y += direccion.y * velocidad * dt;

    // 3. Lógica del Cuerpo
    auto cuerpo = this->getComponente<IVJ::ICPartesCuerpo>();

    if (cuerpo && !cuerpo->partes.empty())
    {
        // En movimiento fluido, la cabeza SIEMPRE le manda su posición a la parte 0
        cuerpo->partes[0]->cola_posiciones.agregar(posicion_vieja_cabeza);

        for (size_t i = 0; i < cuerpo->partes.size(); ++i)
        {
            auto& parte_actual = cuerpo->partes[i];

            // Si la cola tiene posiciones, activamos el temporizador para la separación
            if (!parte_actual->cola_posiciones.estaVacia() && !parte_actual->hacerAccion) {
                parte_actual->timer->curr_frame++;
                if (parte_actual->timer->curr_frame >= parte_actual->timer->max_frame) {
                    parte_actual->hacerAccion = true; // ¡Ya pasó el tiempo, a perseguir!
                }
            }

            // Si la parte ya puede moverse
            if (parte_actual->hacerAccion && !parte_actual->cola_posiciones.estaVacia())
            {
                CE::Vector2D posicion_vieja_parte = parte_actual->pos->posicion;

                // Movemos el componente Transform
                parte_actual->pos->posicion = parte_actual->cola_posiciones.verFrente();

                // ¡Sincronizamos la Figura Visual (Rectangulo) con esa nueva posición!
                if(parte_actual->parte) {
                    parte_actual->parte->getTransformada()->posicion = parte_actual->pos->posicion;
                    parte_actual->parte->onUpdate(dt);
                }

                parte_actual->cola_posiciones.sacar();

                // Le mandamos la posición que acabamos de dejar a la siguiente parte
                if (i + 1 < cuerpo->partes.size())
                {
                    cuerpo->partes[i + 1]->cola_posiciones.agregar(posicion_vieja_parte);
                }
            }
        }
    }

    // Actualizamos el dibujo de la cabeza
    Triangulo::onUpdate(dt);
}

// NUEVA FUNCIÓN: Dibujamos en pantalla los cuadrados de la cola
void EnteVibora::draw(sf::RenderTarget& target, sf::RenderStates state) const
{
    auto cuerpo = this->getComponente<IVJ::ICPartesCuerpo>();

    if (cuerpo) {
        // Dibujamos todas las partes que ya estén activas (hacerAccion)
        for (auto& p : cuerpo->partes) {
            if (p->parte && p->hacerAccion) {
                p->parte->draw(target, state);
            }
        }
    }

    // Al final, dibujamos la cabeza para que se vea por encima de la cola
    Triangulo::draw(target, state);
}