#include "agent.h"
#include "environment.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <utility>

using namespace std;

Agent::ActionType Agent::Think()
{
    ActionType accion;

    //actualizo los tiempos en los que no se han visitado las casillas
    //(incremento en 1, excepto si era obstáculo (-1) que no aumenta)
    ActualizarTiempos();

    //actualizo el estado de la casilla actual
    //si choco marco la casilla con OBSTACULO (-1) y retrocedo
    //si no choco digo que es HUECO (0)
    ActualizarCasilla();

    //método para decidir si hay que marcar paredes y hacerlo
    //(con esto nos evitamos muchos choques)
    BuscarParedes();

    //si la casilla donde estoy está sucia, limpio
    if (dirty_)
    {
        accion = actSUCK;
    }
    //si choco, decido dirección de giro mirando la casilla de la IZQUIERDA y de la DERECHA,
    //la que tiene mayor número (se pasó hace más tiempo o no se pasó aun) tiene mayor peso
    else if (bump_)
    {
        accion = DecidirDireccion();
    }
    //si no sucio, ni choco, decido acción.
    //1.- compruebo si puedo avanzar:
    //1.1- si NO puedo hacerlo llamo a la función decidir acción que hace lo mismo que el caso anterior.
    //1.2.- si puedo avanzar, decido cual es la mejor opción (mayor tiempo) entre la casilla de delante,
    //la de la IZQUIERDA y la de la DERECHA.
    //
    //si decide delante devuelve actFORWARD, IZQUIERDA actTURN_L, y DERECHA actTURN_R.
    else
    {
        accion = DecidirAccion();
    }

    //actualizo la posición en el mapa dependiendo de la acción a tomar:
    //si voy a avanzar, incremento o decremento (x o y) según la dirección.
    //si voy a girar a IZQUIERDA, direccion_--
    //si voy a girar a DERECHA, direccion++
    ActualizarPosicion(accion);

    //salidas por monitor de los estados y las acciones (para depuración)
    //ImprimeEstado();
    //ImprimeMapaTiempos();
    //ImprimeAccionRealizar(accion);

    return accion;
}

//***********************************************************************************************************
//MÉTODOS AUXILIARES

//actualizo los tiempos en los que no se han visitado las casillas
void Agent::ActualizarTiempos()
{
    for (int i=0; i<TAM_MAPA; i++)
    {
        for (int j=0; j<TAM_MAPA; j++)
        {
            //(incremento en 1, excepto si era obstáculo (-1) que no aumenta)
            if (mapa_tiempos[j][i]>-1)
            {
                mapa_tiempos[j][i]++;
            }
        }
    }
}


//actualizo el estado de la casilla actual
void Agent::ActualizarCasilla()
{
    //si choco lo marco y retrocedo la posición en función de direccion_
    if (bump_)
    {
        mapa_tiempos[eje_x_][eje_y_]=OBSTACULO;
        switch(direccion_)
        {
        case ARRIBA:
            eje_y_++;
            break;
        case DERECHA:
            eje_x_--;
            break;
        case ABAJO:
            eje_y_--;
            break;
        case IZQUIERDA:
            eje_x_++;
        }
    }
    //si no choco digo que es un hueco (y que acabo de pasar)
    else
    {
        mapa_tiempos[eje_x_][eje_y_]=HUECO;
    }
}


//método para comprobar si 2 obstáculos son paredes
void Agent::BuscarParedes()
{
    //si aún no he descubierto las paredes de las columnas (verticales)
    if(!paredes_horizontales_)
    {
        bool encontrado = false;

        //para cada columna (hasta la mitad), busco dentro de esa columna y columna+dimensión (todas sus filas)
        //si coinciden 2 obstáculos, eso significaría que tenemos 2 casillas marcadas como obstáculo que son las paredes
        for(int j=0; j<DIMENSION-2 && !encontrado; j++)
        {
            for(int i=0; i<TAM_MAPA && !encontrado; i++)
            {
                for (int k=0; k<TAM_MAPA && !encontrado; k++)
                {
                    //para que no busque en las columnas que podría haber marcado antes el procedimiento de búsqueda de filas
                    if(i!=fila1_ && i!=fila2_ && k!=fila1_ && k!=fila2_)
                    {
                        //si coinciden apunto la columna1 y la columna2:
                        //las almaceno para marcarlas y digo que he encontrado las columnas para que no siga buscando
                        if(mapa_tiempos[j][i]==OBSTACULO && mapa_tiempos[DIMENSION-1+j][k]==OBSTACULO)
                        {
                            columna1_ = j;
                            columna2_ = DIMENSION-1+j;
                            encontrado = true;
                        }
                    }
                }
            }
        }

        //si he encontrando las columnas, las marco en el mapa
        if(encontrado)
        {
            for(int i=0; i<TAM_MAPA; i++)
            {
                mapa_tiempos[columna1_][i]=OBSTACULO;
                mapa_tiempos[columna2_][i]=OBSTACULO;
            }

            //digo que se han ancontrado las paredes de las columnas, para no volver a buscar
            paredes_horizontales_ = true;
        }
    }


    //si aún no he descubierto las paredes de las filas (horizontales)
    if(!paredes_verticales_)
    {
        bool encontrado = false;

        //para cada fila (hasta la mitad), busco dentro de esa fila y fila+dimensión (todas sus columnas)
        //si coinciden 2 obstáculos, eso significaría que tenemos 2 casillas marcadas como obstáculo que son las paredes
        for(int j=0; j<DIMENSION-2 && !encontrado; j++)
        {
            for(int i=0; i<TAM_MAPA && !encontrado; i++)
            {
                for (int k=0; k<TAM_MAPA && !encontrado; k++)
                {
                    //para que no busque en las filas que podría haber marcado antes el procedimiento de búsqueda de columnas
                    if(i!=columna1_ && i!=columna2_ && k!=columna1_ && k!=columna2_)
                    {
                        //si coinciden apunto la fila1_ y la fila2_:
                        //las almaceno para marcarlas y digo que he encontrado las filas para que no siga buscando
                        if(mapa_tiempos[i][j]==OBSTACULO && mapa_tiempos[k][DIMENSION-1+j]==OBSTACULO)
                        {
                            fila1_ = j;
                            fila2_ = DIMENSION-1+j;
                            encontrado = true;
                        }
                    }
                }
            }
        }

        //si he encontrando las filas, las marco en el mapa
        if(encontrado)
        {
            for(int j=0; j<TAM_MAPA; j++)
            {
                mapa_tiempos[j][fila1_]=OBSTACULO;
                mapa_tiempos[j][fila2_]=OBSTACULO;
            }

            //digo que se han ancontrado las paredes de las filas, para no volver a buscar
            paredes_verticales_ = true;
        }
    }
}


//decido la dirección en los casos de que haya chocado o tenga obstáculo delante
//dependiendo de la dirección actual, miro la casilla de la IZQUIERDA y de la DERECHA,
//la que tenga mayor número (se pasó hace más tiempo o no se pasó aun) tiene mayor peso
Agent::ActionType Agent::DecidirDireccion()
{
    ActionType accion;

    switch (direccion_)
    {
    case ARRIBA:
        if(mapa_tiempos[eje_x_-1][eje_y_] > mapa_tiempos[eje_x_+1][eje_y_])
        {
            accion=actTURN_L;
        }
        else
        {
            accion= actTURN_R;
        }
        break;
    case DERECHA:
        if(mapa_tiempos[eje_x_][eje_y_-1] > mapa_tiempos[eje_x_][eje_y_+1])
        {
            accion=actTURN_L;
        }
        else
        {
            accion= actTURN_R;
        }
        break;
    case ABAJO:
        if(mapa_tiempos[eje_x_+1][eje_y_] > mapa_tiempos[eje_x_-1][eje_y_])
        {
            accion=actTURN_L;
        }
        else
        {
            accion= actTURN_R;
        }
        break;
    case IZQUIERDA:
        if(mapa_tiempos[eje_x_][eje_y_+1] > mapa_tiempos[eje_x_][eje_y_-1])
        {
            accion=actTURN_L;
        }
        else
        {
            accion= actTURN_R;
        }
    }
    return accion;
}


//en el caso de que no sucio, ni choco, decido acción
Agent::ActionType Agent::DecidirAccion()
{
    ActionType accion;
    //si tengo obstáculo delante decido adonde giro
    if(!PuedoAvanzar())
    {
        accion=DecidirDireccion();
    }
    //si puedo avanzar, decido si es mejor avanzar o girar, comprobando vecinos (que casilla es mayor)
    else
    {
        accion=ComprobarVecinos();
    }
}


//compruebo dependiendo de la orientación, si tengo un obstáculo delante
bool Agent::PuedoAvanzar()
{
    bool puedo_avanzar_=false;
    switch(direccion_)
    {
    case ARRIBA:
        if (mapa_tiempos[eje_x_][eje_y_-1]!=OBSTACULO)
        {
            puedo_avanzar_=true;
        }
        break;
    case DERECHA:
        if (mapa_tiempos[eje_x_+1][eje_y_]!=OBSTACULO)
        {
            puedo_avanzar_=true;
        }
        break;
    case ABAJO:
        if (mapa_tiempos[eje_x_][eje_y_+1]!=OBSTACULO)
        {
            puedo_avanzar_=true;
        }
        break;
    case IZQUIERDA:
        if (mapa_tiempos[eje_x_-1][eje_y_]!=OBSTACULO)
        {
            puedo_avanzar_=true;
        }
    }
    return puedo_avanzar_;
}


//si puedo avanzar porque no tengo obstáculo delante, evalúo la mejor opción, adelante, giro IZQUIERDA o DERECHA
//(los giros son para posicionarme apuntando a la mejor casilla posible)
Agent::ActionType Agent::ComprobarVecinos()
{
    //empiezo suponiendo que el mayor tiempo está delante
    ActionType accion=actFORWARD;
    int valor_actual_;

    //dependiendo de la dirección actual, los vecinos cambian
    if (direccion_==ARRIBA)
    {
        //empiezo almacenando ARRIBA y lo comparo con su DERECHA y su IZQUIERDA
        valor_actual_=mapa_tiempos[eje_x_][eje_y_-1];
        if (mapa_tiempos[eje_x_+1][eje_y_]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_+1][eje_y_];
            accion=actTURN_R;
        }

        if (mapa_tiempos[eje_x_-1][eje_y_]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_-1][eje_y_];
            accion=actTURN_L;
        }
    }
    else if (direccion_==DERECHA)
    {
        //empiezo almacenando DERECHA y lo comparo con su DERECHA y su IZQUIERDA
        valor_actual_=mapa_tiempos[eje_x_+1][eje_y_];
        if (mapa_tiempos[eje_x_][eje_y_+1]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_][eje_y_+1];
            accion=actTURN_R;
        }

        if (mapa_tiempos[eje_x_][eje_y_-1]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_][eje_y_-1];
            accion=actTURN_L;
        }
    }
    else if (direccion_==ABAJO)
    {
        //empiezo almacenando ABAJO y lo comparo con su DERECHA y su IZQUIERDA
        valor_actual_=mapa_tiempos[eje_x_][eje_y_+1];
        if (mapa_tiempos[eje_x_-1][eje_y_]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_-1][eje_y_];
            accion=actTURN_R;
        }

        if (mapa_tiempos[eje_x_+1][eje_y_]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_+1][eje_y_];
            accion=actTURN_L;
        }
    }
    else //direccion==3
    {
        //empiezo almacenando IZQUIERDA y lo comparo con su DERECHA y su IZQUIERDA
        valor_actual_=mapa_tiempos[eje_x_-1][eje_y_];
        if (mapa_tiempos[eje_x_][eje_y_-1]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_][eje_y_-1];
            accion=actTURN_R;
        }

        if (mapa_tiempos[eje_x_][eje_y_+1]>valor_actual_)
        {
            valor_actual_=mapa_tiempos[eje_x_][eje_y_+1];
            accion=actTURN_L;
        }
    }
    return accion;
}


//actualizo la posición en el mapa dependiendo de la acción a tomar:
//si voy a avanzar, incremento o decremento (x o y) según la dirección.
//si voy a girar a IZQUIERDA, direccion_--
//si voy a girar a DERECHA, direccion++
void Agent::ActualizarPosicion(Agent::ActionType accion)
{
    switch (accion)
    {
    case actFORWARD:
        switch(direccion_)
        {
        case ARRIBA:
            eje_y_--;
            break;
        case DERECHA:
            eje_x_++;
            break;
        case ABAJO:
            eje_y_++;
            break;
        case IZQUIERDA:
            eje_x_--;
            break;
        }
        break;
    case actTURN_L:
        direccion_--;
        if (direccion_==-1)
        {
            direccion_=3;
        }
        break;

    case actTURN_R:
        direccion_++;
        if (direccion_==4)
        {
            direccion_=0;
        }
    }
}


//muestra por pantalla el mapa del agente
void Agent::ImprimeMapaTiempos()
{
    cout << endl << "-------------------------------------------------------------------" << endl;
    for (int i=0; i<TAM_MAPA; i++)
    {
        for (int j=0; j<TAM_MAPA; j++)
        {
            cout << mapa_tiempos[j][i] << "| ";
        }
        cout << endl << "-------------------------------------------------------------------" << endl;
    }
}


//muestra por pantalla el estado actual
void Agent::ImprimeEstado()
{
    cout << "ARRIBA    [" << eje_x_ << " " << eje_y_-1 << "]: " << mapa_tiempos[eje_x_][eje_y_-1] << endl;
    cout << "DERECHA   [" << eje_x_+1 << " " << eje_y_ << "]: " << mapa_tiempos[eje_x_+1][eje_y_] << endl;
    cout << "ABAJO     [" << eje_x_ << " " << eje_y_+1 << "]: " << mapa_tiempos[eje_x_][eje_y_+1] << endl;
    cout << "IZQUIERDA [" << eje_x_-1 << " " << eje_y_ << "]: " << mapa_tiempos[eje_x_-1][eje_y_] << endl;

    cout << "DIRTY: " << dirty_ << "  BUMP: " << bump_ <<
         "  DIRECCION: " << direccion_ << "  EJE X: " << eje_x_ << "  EJE Y: " << eje_y_ << endl;
}


//muestra por pantalla la acción que se ha decidido
void Agent::ImprimeAccionRealizar(Agent::ActionType accion)
{
    cout << "VOY A HACER: ";
    switch (accion)
    {
    case actFORWARD:
        cout << "FORWARD" << endl;
        break;
    case actTURN_L:
        cout << "TURN LEFT" << endl;
        break;
    case actTURN_R:
        cout << "TURN RIGHT" << endl;
        break;
    case actSUCK:
        cout << "SUCK" << endl;
        break;
    case actIDLE:
        cout << "IDLE" << endl;
    default:
        cout << "?????" << endl;
    }
    cout << endl;
}


//FIN MÉTODOS AUXILIARES
//***********************************************************************************************************

void Agent::Perceive(const Environment &env)
{
    bump_ = env.isJustBump();
    dirty_ = env.isCurrentPosDirty();
}
// -----------------------------------------------------------
string ActionStr(Agent::ActionType accion)
{
    switch (accion)
    {
    case Agent::actFORWARD:
        return "FORWARD";
    case Agent::actTURN_L:
        return "TURN LEFT";
    case Agent::actTURN_R:
        return "TURN RIGHT";
    case Agent::actSUCK:
        return "SUCK";
    case Agent::actIDLE:
        return "IDLE";
    default:
        return "???";
    }
}
