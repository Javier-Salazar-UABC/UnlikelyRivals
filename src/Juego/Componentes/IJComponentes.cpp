#include "IJComponentes.hpp"
#include "../../Motor/Render/Render.hpp"
#include "../../Motor/Primitivos/GestorAssets.hpp"
#include <cmath>
#include <string>
#include "../Sistemas/Sistemas.hpp"
#include "../Maquinas/Balas/BalasIdle.hpp"


namespace IVJ
{

    IMaquinaEstado::IMaquinaEstado()
    {
        fsm = nullptr;
    }

    IRayo::IRayo(CE::Vector2D& pos,float& sig,const float magnitud)
    :magnitud{magnitud},dir{sig},lp1{pos},lp2{0.f}
    {}

    CE::Vector2D& IRayo::getP1() const
    {
        return lp1;
    }

    CE::Vector2D& IRayo::getP2()
    {
        //int direction = (dir > 0) ? 1 :(dir<0) ? -1 : 0;
        int direction = (dir > 0) ? 1 : -1;
        lp2.x = lp1.x + direction * magnitud;
        lp2.y = lp1.y ;
        return lp2;
    }

    //Interactuables
    IDialogo::IDialogo()
        :IInteractuable(),texto{L""},id_texto{0}
    {
        texto= L"Texto de Prueba ABC ABC, se debe cargar por el arbol, al precionar una tecla" 
            L"se avanza al siguiente texto del dialogo, esto es solo una demostración." 
            L" El texto debe tener saltos de linea automáticos para no salirse de la caja"
            L"  de dialogo que se renderiza en pantalla."
            L"Espero que funcione correctamente y se vea bien en pantalla.";
        
    }
    void IDialogo::onInteractuar(CE::Objeto& obj)
    {
        (void)obj;
        //fetch el dialogo del árbol
        if(!activo) return;
        //auto control = obj.getComponente<CE::IControl>();
        //control->setActivo(false);
    }
    
    std::wstring IDialogo::agregarSaltoLinea(const std::wstring& str, size_t max_len)
    {
        std::wstring resultado;
        size_t pos = 0;
        while (pos < str.size()) 
        {
            size_t chunk_size = std::min(max_len, str.size() - pos);
            resultado += str.substr(pos, chunk_size); //extraer un fragmento
            pos += chunk_size;
            if (pos < str.size()) 
                resultado += L"\n";
        }
        return resultado;
    }
    void IDialogo::onRender()
    {
        if(!activo) return;
        auto dim = CE::Render::Get().GetVentana().getSize();
        sf::RectangleShape fondo;
        fondo.setSize({dim.x*0.7f,150.f});
        fondo.setFillColor({0,0,0,150});
        fondo.setPosition({(dim.x/3.f)-200.f,120.f});
        sf::Font font{CE::GestorAssets::Get().getFont("nova")};
        std::wstring texto_con_saltos = agregarSaltoLinea(texto,76);
        sf::Text texto_render{font,texto_con_saltos,20};
        texto_render.setPosition({(dim.x/3.f)-180.f,130.f});
        CE::Render::Get().AddToDraw(fondo);
        CE::Render::Get().AddToDraw(texto_render);
        //renderizar el dialogo
    }

    IIndicador::IIndicador(const sf::Texture &ref, float escala,Entidad *p,const sf::Color& c)
        : IInteractuable(),sprite{ref},color{c},escala{escala}
    {
        sprite.setScale({escala,escala});
        auto dim = sprite.getTextureRect().size;
        sprite.setOrigin({dim.x/2.f,dim.y/2.f});
        parent = p;
        if(!shader.loadFromFile(ASSETS "/shaders/prueba.frag",sf::Shader::Type::Fragment)) exit(1);
        
        //blob
        shader.setUniform("blob",
        CE::GestorAssets::Get().getTextura("blob"));
        shader.setUniform("mask_color",sf::Glsl::Vec4(color));
    }
    IIndicador::IIndicador(const IIndicador& other)
        : IInteractuable(other), sprite(other.sprite), color(other.color), escala(other.escala), frame_activo(other.frame_activo), activo(other.activo), parent(other.parent)
    {
        // Reload shader
        if(!shader.loadFromFile(ASSETS "/shaders/prueba.frag",sf::Shader::Type::Fragment)) exit(1);
        shader.setUniform("blob", CE::GestorAssets::Get().getTextura("blob"));
        shader.setUniform("mask_color",sf::Glsl::Vec4(color));
    }
    void IIndicador::onInteractuar(CE::Objeto &obj)
    {
        (void)obj;
    }
    void IIndicador::onRender()
    {
        if(!activo) return;
        auto pos = parent->getTransformada()->posicion;
        auto sd = parent->getComponente<CE::ISprite>()->m_sprite.getGlobalBounds().size;
        pos = pos + CE::Vector2D{0,-sd.y};
        sprite.setPosition({pos.x,pos.y});
        sf::RenderStates estados;
        estados.shader = &shader;
        CE::Render::Get().AddToDraw(sprite,estados);
    }

    IGirar::IGirar(const float ang,const float r)
        :CE::IComponentes{},angulo{ang},radio{r}
    {
    }

    IArribaAbajo::IArribaAbajo(const float dist)
        : CE::IComponentes{},distancia{dist}
    {
    }

    IOnda::IOnda(const float amp, const float frc)
        : CE::IComponentes{}, amplitud{amp}, frecuencia{frc}
    {
    }

    ICTimer::ICTimer(const int curr_frame, const int max_frame)
        : CE::IComponentes{}, curr_frame{curr_frame}, max_frame{max_frame}
    {
    }

    ICParte::ICParte()
        : CE::IComponentes{}, hacerAccion{false}
    {
        pos = new CE::ITransform();

        // Creamos el CUADRADO visual (ancho, alto, relleno, contorno)
        parte = new IVJ::Rectangulo(18.0f, 18.0f, sf::Color::Green, sf::Color::Black);

        // Usaremos 12 frames de retraso para que las partes no se encimen
        timer = new IVJ::ICTimer(0, 12);
    }
    
    ICParte::ICParte(const ICParte& other)
        : CE::IComponentes(other), hacerAccion(other.hacerAccion)
    {
        // Deep copy
        pos = new CE::ITransform(*other.pos);
        if (other.parte) {
            // This is tricky as Figuras might need their own cloning logic,
            // but for now we assume they are copyable or we just recreate them.
            // Assuming Rectangulo is the common one used here.
            parte = new IVJ::Rectangulo(*(IVJ::Rectangulo*)other.parte);
        } else {
            parte = nullptr;
        }
        timer = new IVJ::ICTimer(*other.timer);
    }

    // Nuevo destructor para evitar memory leaks
    ICParte::~ICParte()
    {
        // Si 'parte' se le asignó un 'new CE::ISprite' más adelante, aquí se limpia
        if (parte) delete parte;

        delete pos;
        delete timer;
    }

    ICPartesCuerpo::ICPartesCuerpo(int w, int h)
        : CE::IComponentes{}, width{w}, height{h}
    {
        // El vector "partes" se inicializa vacío automáticamente
    }

    IEstadoSalud::IEstadoSalud(TipoSalud inicial)
        : CE::IComponentes{}, estado_actual{inicial}, estado_siguiente{inicial}
    {
        // Generar probabilidades individuales únicas para cada persona
        prob_contagio = 15 + (rand() % 16); // Da un número entre 15 y 30
        prob_inmunidad = 1 + (rand() % 10); // Da un número entre 1 y 10
    }

    IVisualSalud::IVisualSalud() : CE::IComponentes{}
    {
        // Normal: Transparente por dentro (solo se verá el contorno negro que ya le diste)
        colores[TipoSalud::Normal]       = sf::Color::Transparent;
        colores[TipoSalud::Enfermo]      = sf::Color(255, 20, 0);  // Naranja/Rojo
        colores[TipoSalud::Recuperacion] = sf::Color(155, 0, 255); // Morado/Azul
        colores[TipoSalud::Inmune]       = sf::Color(30, 205, 0); // Verde
        colores[TipoSalud::Muerto]       = sf::Color(20, 20, 20);  // Gris oscuro
    }

    ITriangulo::ITriangulo(float r, float ang)
        :CE::IComponentes{},
        tri_shape{r,3},
        angulo{ang}
    {
        tri_shape.setOrigin({r/2.f,r/2.f});
    }

    ITarget::ITarget(CE::Objeto *target)
            :CE::IComponentes{},pos{nullptr}
    {
        if(target)
            pos = &target->getTransformada()->posicion;
    }
    //& garantiza que no sea nulo
    void ITarget::setTarget(CE::Objeto& t)
    {
        pos = &(t.getTransformada()->posicion);
    }

    IRangoAggro::IRangoAggro(float radio)
        :CE::IComponentes{},radio{radio},
        marcador{sf::CircleShape{radio}}
    {
        marcador.setOrigin({radio,radio});
        marcador.setFillColor(sf::Color{255,0,0,128});
    }
    bool IRangoAggro::estaDentroRango(CE::Objeto& parent,CE::Vector2D& target)
    {
        auto pos = parent.getTransformada()->posicion;

        //asumiendo la formula del circulo x^2+y^2 =r^2
        //si quiero saber si un punto esta dentro del circulo entonces
        //el centro en (h,k) podemos calcular la distancia del punto(x,y)
        //con respecto al centro y el radio.
        //(x-h)^2 + (y-k)^2 <= r^2
        //punto es el target y centro la pos del ente
        float r2 = radio*radio;
        float diX = (target.x-pos.x);
        float diY = (target.y-pos.y);
        float dist = diX*diX + diY*diY;

        return dist <= r2;
    }

    IGravedad::IGravedad(float fuerza, float max_vel)
        : CE::IComponentes{}, fuerza{fuerza}, max_vel{max_vel}, velocidad_Y{0.0f}, en_suelo{false}, saltos_restantes{2}
    {
    }

    IEmisor::IEmisor(Entidad &parent, Entidad &prefab)
    : CE::IComponentes{}, pool{1}, parent{parent}, prefab{prefab}
{}

void IEmisor::onUpdate(float dt)
{
    for (auto &p : pool.getPool())
    {
        // asumir que tiene un timer de lo contrario crushear
        auto itimer = p->getComponente<CE::ITimer>();
        // verificar si ya llego al timer, si si poner vida 0 y borrar
        if (itimer->frame_actual >= itimer->frame_maximo)
            p->getStats()->hp = 0;
        // ejecutar update y mover?
        p->onUpdate(dt);
        SistemaMoverBalas(p, dt);
        itimer->frame_actual++;
    }
    //borar si vida es 0
    pool.borrarPool();
}
void IEmisor::crearParticula(void)
{
    auto nuevo = std::make_shared<Entidad>(prefab);
    //  calcular dirección y posición
    auto p_pos = parent.getTransformada()->posicion;
    auto n_vel = nuevo->getTransformada()->velocidad;
    nuevo->getTransformada()->angulo = parent.getTransformada()->angulo;
    float ang = nuevo->getTransformada()->angulo;
    const float pi = std::round(3.14159f * 1000.f) / 1000.f;
    const float pi_2 = std::round(pi / 2.f * 1000.f) / 1000.f;
    const float pi_4 = std::round(pi / 4.f * 1000.f) / 1000.f - 0.001f;
    const float pi_4b = std::round(3.f * pi / 4.f * 1000.f) / 1000.f - 0.001f;
    const float rang = std::round(ang * 1000.f) / 1000.f;
    // std::cout << rang << ": " << pi << ", " << pi_2 << ", " << pi_4 << ", " << pi_4b << "\n";
    auto vel = CE::Vector2D{0.f, 0.f};
    if (rang == 0.f) //arriba
        vel.y = -n_vel.y;
    if (rang == pi) // PI: abajo
        vel.y = n_vel.y;
    if (rang == -pi_4) //-pi/4:izq arriba
    {
        vel.y = -n_vel.y;
        vel.x = -n_vel.x;
    }
    if (rang == pi_4) // pi/4: der arriba
    {
        vel.y = -n_vel.y;
        vel.x = n_vel.x;
    }
    if (rang == pi_2) // PI/2: der
        vel.x = n_vel.x;
    if (rang == -pi_2) // izq
        vel.x = -n_vel.x;
    // patch el 2.3 ya que redondea extraño
    if (rang >= -pi_4b && rang <= -2.3) //-3pi/4 abajo izq
    {
        vel.y = n_vel.y;
        vel.x = -n_vel.x;
    }
    if (rang <= pi_4b && rang >= 2.3) // 3pi/4 abajo der
    {
        vel.y = n_vel.y;
        vel.x = n_vel.x;
    }
    //maquina de estados
    nuevo->getComponente<IMaquinaEstado>()->fsm = std::make_shared<BalasIdle>(1.f);
    nuevo->setFSM(nuevo->getComponente<IMaquinaEstado>()->fsm);
    // en el objeto padre la posición
    nuevo->setPosicion(p_pos.x, p_pos.y);
    nuevo->getTransformada()->velocidad = vel;
    pool.agregarPool(nuevo);
}
}