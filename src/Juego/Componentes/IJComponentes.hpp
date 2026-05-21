#pragma once
#include "../Maquinas/FSM.hpp"
#include "../../Motor/Componentes/IComponentes.hpp"
#include "../../Motor/Primitivos/Objetos.hpp"
#include "../Figuras/Figuras.hpp"
#include <vector>

#include "Motor/Primitivos/CEPool.hpp"

namespace IVJ
{
    class FSM; //refefencia circular
    //Maquina estado componente
    class IMaquinaEstado : public CE::IComponentes
    {
        public:
            explicit IMaquinaEstado();
            ~IMaquinaEstado() override{};

            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IMaquinaEstado>(*this);
            };
        public:
            std::shared_ptr<FSM> fsm;
            bool congelar{false};
    };

    //componente QUEST con un arreglo de int de 8 bit

    class IRayo : public CE::IComponentes
    {
        public:
            explicit IRayo(CE::Vector2D& pos,float& sig,const float magnitud=1.f );
            ~IRayo() override{};
        private:
            float magnitud;
            float& dir;
            CE::Vector2D& lp1;
            CE::Vector2D lp2;
        public:
            CE::Vector2D& getP1() const;
            CE::Vector2D& getP2();
            float getMagnitud() const {return magnitud;}

            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IRayo>(*this);
            };
    };

    class IInteractuable : public CE::IComponentes
    {
        public:
            IInteractuable()=default;
            virtual ~IInteractuable() =default;
            virtual void onInteractuar(CE::Objeto& obj)=0;
        public:
            bool interactuado{false};
    };

    class IIndicador : public IInteractuable
    {
        public:
            explicit IIndicador(const sf::Texture &ref,float escala,Entidad *p, const sf::Color& c);
            IIndicador(const IIndicador& other);
            virtual ~IIndicador() =default;
            void onInteractuar(CE::Objeto& obj) override;
            void onRender();
        public:
            sf::Sprite sprite;
            sf::Shader shader;
            sf::Color color;
            float escala;
            int frame_activo{0};
            bool activo{false};

            std::shared_ptr<IComponentes> clonar() const override
            {
                // Note: sf::Shader is not copyable. If this is cloned, 
                // the shader state might need manual handling.
                return std::make_shared<IIndicador>(*this);
            };
        private:
            Entidad* parent;
    };

    class IDialogo : public IInteractuable
    {
        public:
            IDialogo();
            virtual ~IDialogo() = default;
            void onInteractuar(CE::Objeto& obj) override;
            void onRender();
        private:
            std::wstring agregarSaltoLinea(const std::wstring& str, size_t max_len);
        public:
            std::wstring texto;
            int id_texto;
            bool activo{false};

            std::shared_ptr<IComponentes> clonar() const override
            {
                return std::make_shared<IDialogo>(*this);
            };
    };

    class IGirar : public CE::IComponentes
    {
    public:
        explicit IGirar(const float ang,const float r);
    public:
        float angulo;
        float radio;

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IGirar>(*this);
        };
    };

    class IArribaAbajo : public CE::IComponentes
    {
    public:
        explicit IArribaAbajo(const float dist);
    public:
        float distancia;
        float angulo{0.0f};

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IArribaAbajo>(*this);
        };
    };

    class IOnda : public CE::IComponentes
    {
    public:
        explicit IOnda(const float amp, const float frc);
    public:
        float amplitud;
        float frecuencia;
        float angulo{0.0f};

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IOnda>(*this);
        };
    };

    class ICTimer : public CE::IComponentes
    {
    public:
        explicit ICTimer(const int curr_frame,const int max_frame);
    public:
        int curr_frame;
        int max_frame;

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<ICTimer>(*this);
        };
    };

    class ICheckEstado : public CE::IComponentes
    {
    public:
        explicit ICheckEstado();
    public:
        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<ICheckEstado>(*this);
        };
    };

    struct NodoPosicion {
        CE::Vector2D posicion;
        NodoPosicion* siguiente;
        explicit NodoPosicion(const CE::Vector2D& pos) : posicion(pos), siguiente(nullptr) {}
    };

    class ColaPosiciones {
    private:
        NodoPosicion* frente; // Apunta al primer elemento (el que va a salir)
        NodoPosicion* final;  // Apunta al último elemento (el que acaba de entrar)

    public:
        ColaPosiciones() : frente(nullptr), final(nullptr) {}

        ~ColaPosiciones() {
            // Limpiamos la memoria al destruir la cola
            while (!estaVacia()) {
                sacar();
            }
        }

        // Agregar un elemento a la cola (Push / Enqueue)
        void agregar(const CE::Vector2D& pos) {
            NodoPosicion* nuevoNodo = new NodoPosicion(pos);
            if (estaVacia()) {
                frente = final = nuevoNodo;
            } else {
                final->siguiente = nuevoNodo;
                final = nuevoNodo;
            }
        }

        // Quitar el primer elemento de la cola (Pop / Dequeue)
        void sacar() {
            if (estaVacia()) return;
            NodoPosicion* nodoAEliminar = frente;
            frente = frente->siguiente;
            if (frente == nullptr) {
                final = nullptr; // Si la cola quedó vacía
            }
            delete nodoAEliminar;
        }

        // Ver el primer elemento (Front)
        CE::Vector2D verFrente() const {
            if (!estaVacia()) return frente->posicion;
            return CE::Vector2D{0.0f, 0.0f}; // Retorno por defecto si está vacía
        }

        bool estaVacia() const {
            return frente == nullptr;
        }
    };

    class ICParte : public CE::IComponentes
    {
    public:
        ICParte();
        ICParte(const ICParte& other);
        ~ICParte() override;

        ColaPosiciones cola_posiciones;

        IVJ::Figuras* parte;     // <-- Cambiamos esto a Figuras*
        CE::ITransform* pos;
        IVJ::ICTimer* timer;

        bool hacerAccion{false};

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<ICParte>(*this);
        };
    };

    class ICPartesCuerpo : public CE::IComponentes
    {
    public:
        // Constructor que pide el ancho y alto
        explicit ICPartesCuerpo(int w, int h);

        // 1) Vector de componentes ICParte (usamos shared_ptr por la arquitectura de tu motor)
        std::vector<std::shared_ptr<ICParte>> partes;

        // 2) Largo de la parte
        int width;

        // 3) Alto de la parte
        int height;

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<ICPartesCuerpo>(*this);
        };
    };

    enum class TipoSalud {
        Normal,
        Enfermo,
        Recuperacion,
        Inmune,
        Muerto
    };

    class IEstadoSalud : public CE::IComponentes
    {
    public:
        explicit IEstadoSalud(TipoSalud inicial = TipoSalud::Normal);
        ~IEstadoSalud() override = default;

        TipoSalud estado_actual;
        TipoSalud estado_siguiente;

        // Temporizadores
        float tiempo_en_estado{0.0f};
        float timer_infeccion{0.0f};         // Para el tick de contagio cada 1 segundo
        float inmunidad_parcial_timer{0.0f}; // Dura 2 segundos al volver a Normal

        // Probabilidades individuales (generadas al nacer)
        int prob_contagio;   // Del 15% al 30%
        int prob_inmunidad;  // Del 1% al 10%

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IEstadoSalud>(*this);
        };
    };

    class IEmisor : public CE::IComponentes
    {
    public:
        explicit IEmisor(Entidad &parent, Entidad &prefab);
        ~IEmisor(void) {};
        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IEmisor>(*this);
        };
        void onUpdate(float dt);
        void crearParticula(void);
        CE::Pool &getPool(void)
        {
            return pool;
        }
    private:
        CE::Pool pool;
        Entidad &parent; // quien tiene el emisor
        Entidad &prefab; // el objeto que se va copiar
    };

    class IVisualSalud : public CE::IComponentes
    {
    public:
        IVisualSalud();
        ~IVisualSalud() override = default;

        std::map<TipoSalud, sf::Color> colores;
        IVJ::Rectangulo* figura{nullptr};

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IVisualSalud>(*this);
        };
    };

    class ITriangulo : public CE::IComponentes
    {
    public:
        explicit ITriangulo(float r, float ang);
        ~ITriangulo() override{};
    public:
        sf::CircleShape tri_shape;
        float angulo;

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<ITriangulo>(*this);
        };
    };

    class ITarget : public CE::IComponentes
    {
    public:
        //puede ser nula por eso *
        explicit ITarget(CE::Objeto * target);
        void setTarget(CE::Objeto& t);
    public:
        CE::Vector2D* pos;

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<ITarget>(*this);
        };
    };

    class IRangoAggro : public CE::IComponentes
    {
    public:
        explicit IRangoAggro(float radio);
        virtual ~IRangoAggro(){};
        bool estaDentroRango(CE::Objeto& parent, CE::Vector2D& target);
    public:
        float radio;
        sf::CircleShape marcador;

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IRangoAggro>(*this);
        };
    };

    class IGravedad : public CE::IComponentes
    {
    public:
        explicit IGravedad(float fuerza = 980.0f, float max_vel = 1500.0f);
        ~IGravedad() override = default;
    public:
        float fuerza;
        float max_vel;
        float velocidad_Y;
        bool en_suelo;
        int saltos_restantes;

        // Ledge Grab (Colgarse del borde)
        bool colgado_borde{false};
        float ledge_grab_cooldown{0.0f};

        // Atravesar plataformas flotantes
        float atravesar_timer{0.0f};
        float tiempo_ultimo_abajo{999.0f};
        bool abajo_prev{false};
        float Y_inicial_traspaso{0.0f};

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IGravedad>(*this);
        };
    };

    /**
     * @class IAnimaciones
     * @brief Mapa de claves de textura y metadatos por nombre de animación.
     *
     * Permite que los FSMs sean genéricos: en lugar de tener el nombre del
     * personaje hardcodeado, leen la clave y configuración de aquí.
     */
    class IAnimaciones : public CE::IComponentes
    {
    public:
        struct AnimData {
            std::string clave_textura;
            int frames{1};
            float frame_rate{0.1f};
            int width{0};  // 0 significa usar el default del ISprite
            int height{0}; // 0 significa usar el default del ISprite
        };

        IAnimaciones() = default;
        ~IAnimaciones() override = default;

        /** @brief Asigna la configuración para una animación. */
        void set(const std::string& nombre_anim, const std::string& clave_textura, int frames = 1, float rate = 0.1f, int w = 0, int h = 0)
        {
            m_mapa[nombre_anim] = {clave_textura, frames, rate, w, h};
        }

        /** @brief Obtiene la configuración para una animación. */
        const AnimData* get(const std::string& nombre_anim) const
        {
            auto it = m_mapa.find(nombre_anim);
            return (it != m_mapa.end()) ? &it->second : nullptr;
        }

        std::shared_ptr<IComponentes> clonar() const override
        {
            return std::make_shared<IAnimaciones>(*this);
        }

    private:
        std::map<std::string, AnimData> m_mapa;
    };
}
