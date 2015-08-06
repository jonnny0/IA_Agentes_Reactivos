#ifndef AGENT__
#define AGENT__

#include <string>
using namespace std;

// -----------------------------------------------------------
//				class Agent
// -----------------------------------------------------------
class Environment;

#define MAX_VALUE 10 //valor con el que inician las casillas desconocidas
#define OBSTACULO -1
#define HUECO 0
#define ARRIBA 0
#define DERECHA 1
#define ABAJO 2
#define IZQUIERDA 3
#define DIMENSION 10 //para un mapa de 10X10

#define TAM_MAPA (DIMENSION-2)*2+1 //para tamaño MAXIMO de mapa 10X10 (8 en cada dirección + casilla actual)

class Agent
{
public:

    Agent()
    {
        bump_ = false;
        dirty_ = false;
        direccion_=ARRIBA; //supongo que estoy apuntando ARRIBA
        eje_x_=DIMENSION-2; //mi coordenada inicial eje_x_ es 8 (para 10X10) JUSTO EN EL CENTRO
        eje_y_=DIMENSION-2; //mi coordenada inicial eje_y_ es 8 (para 10X10) JUSTO EN EL CENTRO

        //relleno el mapa del agente con los valores iniciales por defecto
        for (int i=0; i<TAM_MAPA; i++)
        {
            for (int j=0; j<TAM_MAPA; j++)
            {
                mapa_tiempos[j][i]=MAX_VALUE;
            }
        }

        //no se han encontrado las paredes de inicio
        bool paredes_horizontales_=false;
        bool paredes_verticales_=false;
    }

    enum ActionType
    {
        actFORWARD,
        actTURN_L,
        actTURN_R,
        actSUCK,
        actIDLE
    };


    void Perceive(const Environment &env);
    ActionType Think();

    //los métodos que utilizo
    void ActualizarTiempos();
    void ActualizarCasilla();
    void BuscarParedes();
    ActionType DecidirDireccion();
    ActionType DecidirAccion();
    bool PuedoAvanzar();
    ActionType ComprobarVecinos();
    void ActualizarPosicion(Agent::ActionType accion);
    void ImprimeEstado();
    void ImprimeMapaTiempos();
    void ImprimeAccionRealizar(Agent::ActionType accion);

private:
    int mapa_tiempos[TAM_MAPA][TAM_MAPA]; //el tiempo que lleva sin pasar
    bool bump_, dirty_;
    int eje_x_, eje_y_, direccion_; //posición en el mapa del agente y su dirección
    bool paredes_horizontales_; //si ya tengo marcadas las paredes verticales
    bool paredes_verticales_; //si ya tengo marcadas las paredes horizontales
    int columna1_, columna2_; //posición de la columna inicial / final
    int fila1_, fila2_; //posición de la fila inicial / final
};

string ActionStr(Agent::ActionType);

#endif
