/***
 *    ██╗     ██╗██████╗ ███████╗
 *    ██║     ██║██╔══██╗██╔════╝
 *    ██║     ██║██████╔╝███████╗
 *    ██║     ██║██╔══██╗╚════██║
 *    ███████╗██║██████╔╝███████║
 *    ╚══════╝╚═╝╚═════╝ ╚══════╝
 *
 */
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <unistd.h>
#include <pthread.h>
#include "juego.h"
#include "random.h"


/*
Notas:
- Los Marcianos se llaman Boss , Medio y Bajo
- Faltan hilos
*/


/***
 *    ███╗   ███╗ █████╗  ██████╗██████╗  ██████╗ ███████╗
 *    ████╗ ████║██╔══██╗██╔════╝██╔══██╗██╔═══██╗██╔════╝
 *    ██╔████╔██║███████║██║     ██████╔╝██║   ██║███████╗
 *    ██║╚██╔╝██║██╔══██║██║     ██╔══██╗██║   ██║╚════██║
 *    ██║ ╚═╝ ██║██║  ██║╚██████╗██║  ██║╚██████╔╝███████║
 *    ╚═╝     ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝
 *
 */
#define TAM_MOVIMIENTO 10 /*Tamaño de movimiento de la nave*/
#define ANCHO 700
#define ALTO 400
/*Margenes de la ventana*/
#define MIN_VENTANA 30
#define MAX_VENTANA 630

// Márgenes para los marcianos
#define MIN_MARCIANO_X 30
#define MAX_MARCIANO_X 630
#define MIN_MARCIANO_Y 10
#define MAX_MARCIANO_Y 250

/*Cantidad de marcianos*/
#define CANT_BOSS 4
#define CANT_MEDIO 8
#define CANT_BAJO 10
#define CANT_MARCIANOS 40
/*Filas en la formación*/
#define COLUMNA_NAVE ANCHO/2
#define FILA_NAVE 350
#define COLUMNA_VIDAS 10
#define FILA_VIDAS 10

#define FILA_SUPERIOR 30
#define COLUMNA_BOSS 235
#define FILA_MEDIO 65
#define FILA_MEDIA_BAJA 100
#define COLUMNA_MEDIA 115
#define FILA_BAJA 135
#define FILA_BAJA_BAJA 170
#define COLUMNA_BAJO 55


#define POS_INICIAL_BOSS (COLUMNA_BOSS- ANCHO)

#define MAS_ALLA 1000

#define CANT_BALAS 5

/*Velocidades de los marcianos*/
#define VELOCIDAD_MARCIANOS 0.100008
#define VELOCIDAD_MOVIMIENTO 0.2
#define VELOCIDAD_BALA 0.02

#define ARR_BALA_X 14
#define ARR_BALA_Y -14

#define MOV_MARCIANOS 10  //Tamaño del movimiento de los marcianos

// Punataje
#define PUNTOS 100

/***
 *    ██╗   ██╗ █████╗ ██████╗ ██╗ █████╗ ██████╗ ██╗     ███████╗███████╗
 *    ██║   ██║██╔══██╗██╔══██╗██║██╔══██╗██╔══██╗██║     ██╔════╝██╔════╝
 *    ██║   ██║███████║██████╔╝██║███████║██████╔╝██║     █████╗  ███████╗
 *    ╚██╗ ██╔╝██╔══██║██╔══██╗██║██╔══██║██╔══██╗██║     ██╔══╝  ╚════██║
 *     ╚████╔╝ ██║  ██║██║  ██║██║██║  ██║██████╔╝███████╗███████╗███████║
 *      ╚═══╝  ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝╚══════╝
 *
 */

DatosGlobales * JuegoDatos;

//Elementos gráficos
 ALLEGRO_DISPLAY *Pantalla;
ALLEGRO_EVENT_QUEUE *EventQueue;
ALLEGRO_EVENT Event;
const float FPS = 60;
int redibujar;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_SAMPLE *shot= NULL;
ALLEGRO_SAMPLE *song= NULL;
ALLEGRO_SAMPLE *sonido_explosion= NULL;
ALLEGRO_SAMPLE *game_over= NULL;
ALLEGRO_SAMPLE_INSTANCE *songInstance  = NULL;

// Explosión
ALLEGRO_BITMAP *explosion;

//Marcianos
BossG BossArray[CANT_BOSS];     //Arreglo con los Boss Galaga
MedioG MedioArray[CANT_MEDIO];     //Arreglo con los marcianos del medio
MedioG MedioBajoArray[CANT_MEDIO];     //Arreglo con los marcianos del medio de abajo
BajoG BajoArray[CANT_BAJO];     //Arreglo con los marcianos de abajo
BajoG BajoBajoArray[CANT_BAJO];     //Arreglo con los marcianos de abajo

//Contadores lógicos
int Puntaje;

// Arreglo de naves
NaveG *Nave;
NaveG *Vida1;
NaveG *Vida2;


// Balas
BalaG *Bala;


// Hilos
ALLEGRO_THREAD * hilo_bala;
ALLEGRO_THREAD * hilo_animacion;
ALLEGRO_THREAD * hilo_ataque;
ALLEGRO_THREAD * hilo_colision;
ALLEGRO_MUTEX *mutex;

// Funciones para los hilos



/***
 *    ██╗  ██╗██╗██╗      ██████╗ ███████╗
 *    ██║  ██║██║██║     ██╔═══██╗██╔════╝
 *    ███████║██║██║     ██║   ██║███████╗
 *    ██╔══██║██║██║     ██║   ██║╚════██║
 *    ██║  ██║██║███████╗╚██████╔╝███████║
 *    ╚═╝  ╚═╝╚═╝╚══════╝ ╚═════╝ ╚══════╝
 *
 */

void *anim_marcianos(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    while(JuegoDatos->jugando){
        al_rest(VELOCIDAD_MOVIMIENTO);
        DerechaMarcianos();
        al_rest(VELOCIDAD_MOVIMIENTO);
        IzquierdaMarcianos();
        al_rest(VELOCIDAD_MOVIMIENTO);
        ArribaMarcianos();
        al_rest(VELOCIDAD_MOVIMIENTO);
        AbajoMarcianos();
    }

}

// Determina si dos puntos están cercanos con un rango de 25 pixeles
int cercano(int x1,int x2){
            return abs(x1-x2) < 25;
}

// Indica si hay una colision entre 2 coordenadas
int colision(int xMarciano,int yMarciano, int xNave,int yNave){
        if(cercano(yMarciano, yNave) && cercano(xMarciano, xNave)){
                return 1;
        }
        return 0;
}

int cercano_bala(int x1,int x2){
            return abs(x1-x2) < 20;
}

// Indica si hay una colision entre 2 coordenadas
int colision_bala(int xMarciano,int yMarciano, int xBala,int yBala){
        if(cercano_bala(yMarciano, yBala) && cercano_bala(xMarciano, xBala)){
                return 1;
        }
        return 0;
}

void desparacer_bala(){
    Bala->yBala = -10;
}

// Hilo que espera a que colisione una bala con un marciano
void *espera_colision(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    int i = 0;
    while(JuegoDatos->jugando){
            // Muertes de los boss
            i=0;
            for(i; i < CANT_BOSS ; i++){
                if(cercano_bala(Bala->xBala,BossArray[i].xBoss) && cercano_bala(Bala->yBala,BossArray[i].yBoss))
                {
                    BossArray[i].visible = 0;
                    BossArray[i].xBoss = MAS_ALLA;
                    BossArray[i].yBoss = MAS_ALLA;
                    desparacer_bala();
                    al_lock_mutex(mutex);
                    Puntaje ++;
                    al_unlock_mutex(mutex);
                }
            }

            // Muertes de los Medios
            i=0;
            for(i; i < CANT_MEDIO ; i++){
                if(cercano_bala(Bala->xBala,MedioArray[i].xMedio) && cercano_bala(Bala->yBala,MedioArray[i].yMedio))
                {
                    MedioArray[i].visible = 0;
                    MedioArray[i].xMedio = MAS_ALLA;
                    MedioArray[i].yMedio = MAS_ALLA;
                    desparacer_bala();
                    al_lock_mutex(mutex);
                    Puntaje ++;
                    al_unlock_mutex(mutex);
                }
            }

            // Muertes de los Medios Bajos
            i=0;
            for(i; i < CANT_MEDIO ; i++){
                if(cercano_bala(Bala->xBala,MedioBajoArray[i].xMedio) && cercano_bala(Bala->yBala,MedioBajoArray[i].yMedio))
                {
                    MedioBajoArray[i].visible = 0;
                    MedioBajoArray[i].xMedio = MAS_ALLA;
                    MedioBajoArray[i].yMedio = MAS_ALLA;
                    desparacer_bala();
                    al_lock_mutex(mutex);
                    Puntaje ++;
                    al_unlock_mutex(mutex);
                }
            }

            // Muertes de los Bajos
            i=0;
            for(i; i < CANT_BAJO ; i++){
                if(cercano_bala(Bala->xBala,BajoArray[i].xBajo) && cercano_bala(Bala->yBala,BajoArray[i].yBajo))
                {
                    BajoArray[i].visible = 0;
                    BajoArray[i].xBajo = MAS_ALLA;
                    BajoArray[i].yBajo = MAS_ALLA;
                    desparacer_bala();
                    al_lock_mutex(mutex);
                    Puntaje ++;
                    al_unlock_mutex(mutex);
                }
            }

            // Muertes de los Bajos Bajos
            i=0;
            for(i; i < CANT_BAJO ; i++){
                if(cercano_bala(Bala->xBala,BajoBajoArray[i].xBajo) && cercano_bala(Bala->yBala,BajoBajoArray[i].yBajo))
                {
                    BajoBajoArray[i].visible = 0;
                    BajoBajoArray[i].xBajo = MAS_ALLA;
                    BajoBajoArray[i].yBajo = MAS_ALLA;
                    desparacer_bala();
                    al_lock_mutex(mutex);
                    Puntaje ++;
                    al_unlock_mutex(mutex);
                }
            }

    }
}

// Hilo que espera a que una bala sea disparada
void *espera_balas(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    int i = 0;
    while(JuegoDatos->jugando){
        if(Bala->disparada){
            Bala->xBala = Nave->xNave + ARR_BALA_X;
            Bala->yBala = Nave->yNave + ARR_BALA_Y;
            while(Bala->yBala > 0){
                    Bala->yBala -= 10;
                    al_rest(VELOCIDAD_BALA);
            }
                    al_set_window_title(Pantalla,"Galaga");
                    Bala->xBala = MAS_ALLA;
                    Bala->yBala = MAS_ALLA;
                    Bala->disparada = 0;
    }
    }
}





void matar_nave(){  // Determina la cantidad de vidas y las desaparece de pantalla
        switch(JuegoDatos->cantidad_vidas){
                case 0:
                            JuegoDatos->fin_del_juego = 1;
                case 2:
                            Vida2->xNave=-100; // Desaparece una vida de la pantalla
                            Vida2->yNave=-100;
                            JuegoDatos->cantidad_vidas--;
                            break;
                case 1:
                            Vida1->xNave=-100; // Desaparece una vida de la pantalla
                            Vida1->yNave=-100;
                            JuegoDatos->cantidad_vidas--;
                            break;
                default:
                            break;
}

}


// Hilo que hace que los marcianos ataquen
void *ataque_marcianos(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    while(mis_datos->jugando){
    int rand = my_random(0,4);
    switch(rand){
            case 0:  //Para los boss
            {
            rand = my_random(0,3);
            if(BossArray[rand].visible){
                            al_lock_mutex(mutex);
                            BossArray[rand].xRespBoss = BossArray[rand].xBoss;
                            BossArray[rand].yRespBoss = BossArray[rand].yBoss;
                        while(BossArray[rand].yBoss <= ALTO+50){
                            al_rest(VELOCIDAD_MARCIANOS);
                            BossArray[rand].yBoss += MOV_MARCIANOS;
                            if(BossArray[rand].yBoss > FILA_NAVE -50){
                                    if(rand%2 == 0)
                                        BossArray[rand].xBoss += MOV_MARCIANOS;
                                    else
                                        BossArray[rand].xBoss -= MOV_MARCIANOS;
                            }
                            else{
                                    BossArray[rand].xBoss = Nave->xNave;
                            }
                            // Verifica si hay colision de la nave con el marciano
                            if(colision(BossArray[rand].xBoss, BossArray[rand].yBoss, Nave->xNave, Nave->yNave)){
                                    BossArray[rand].xBoss = MAS_ALLA;
                                    BossArray[rand].yBoss = MAS_ALLA;
                                    BossArray[rand].visible = 0;
                                    matar_nave();
                                    Nave->colisionado = 1;
                            }
                            }
                            BossArray[rand].xBoss = BossArray[rand].xRespBoss;
                            BossArray[rand].yBoss = BossArray[rand].yRespBoss;
                            al_unlock_mutex(mutex);
                }
                break;
                }
            case 1:     //PAra los del medio
            {
            rand = my_random(0,7);
            if(MedioArray[rand].visible){
                        al_lock_mutex(mutex);
                        MedioArray[rand].xRespMedio = MedioArray[rand].xMedio;
                        MedioArray[rand].yRespMedio = MedioArray[rand].yMedio;
                    while(MedioArray[rand].yMedio <= ALTO+50){
                        al_rest(VELOCIDAD_MARCIANOS);
                        MedioArray[rand].yMedio += MOV_MARCIANOS;
                        if(MedioArray[rand].yMedio > FILA_NAVE -50){
                                if(rand%2 == 0)
                                        MedioArray[rand].xMedio += MOV_MARCIANOS;
                                    else
                                        MedioArray[rand].xMedio-= MOV_MARCIANOS;
                        }
                        else{
                                MedioArray[rand].xMedio = Nave->xNave;
                        }
                        // Verifica si hay colision de la nave con el marciano
                         if(colision(MedioArray[rand].xMedio, MedioArray[rand].yMedio, Nave->xNave, Nave->yNave)){
                                MedioArray[rand].xMedio= MAS_ALLA;
                                MedioArray[rand].yMedio= MAS_ALLA;
                                MedioArray[rand].visible = 0;
                                matar_nave();
                                Nave->colisionado = 1;
                        }
                        }
                        MedioArray[rand].xMedio = MedioArray[rand].xRespMedio;
                        MedioArray[rand].yMedio = MedioArray[rand].yRespMedio;
                        al_unlock_mutex(mutex);
                }
                break;
                }
                case 2:             //Para los del medio abajo
            {
            rand = my_random(0,7);
            if(MedioBajoArray[rand].visible){
                        al_lock_mutex(mutex);
                        MedioBajoArray[rand].xRespMedio = MedioBajoArray[rand].xMedio;
                        MedioBajoArray[rand].yRespMedio = MedioBajoArray[rand].yMedio;
                    while(MedioBajoArray[rand].yMedio <= ALTO+50){
                        al_rest(VELOCIDAD_MARCIANOS);
                        MedioBajoArray[rand].yMedio+=MOV_MARCIANOS;
                        if(MedioBajoArray[rand].yMedio > FILA_NAVE -50){
                                if(rand%2 == 0)
                                        MedioBajoArray[rand].xMedio += MOV_MARCIANOS;
                                    else
                                        MedioBajoArray[rand].xMedio-= MOV_MARCIANOS;
                        }
                        else{
                                MedioBajoArray[rand].xMedio = Nave->xNave;
                        }
                        // Verifica si hay colision de la nave con el marciano
                        if(colision(MedioBajoArray[rand].xMedio, MedioBajoArray[rand].yMedio, Nave->xNave, Nave->yNave)){
                                MedioBajoArray[rand].xMedio= MAS_ALLA;
                                MedioBajoArray[rand].yMedio= MAS_ALLA;
                                MedioBajoArray[rand].visible = 0;
                                matar_nave();
                                Nave->colisionado = 1;
                        }
                        }
                        MedioBajoArray[rand].xMedio = MedioBajoArray[rand].xRespMedio;
                        MedioBajoArray[rand].yMedio = MedioBajoArray[rand].yRespMedio;
                        al_unlock_mutex(mutex);
                }
                break;
                }
                case 3:             // Para los de abajo
                {
                rand = my_random(0,9);  // Excluye al primer marciano
                if(BajoArray[rand].visible){
                            al_lock_mutex(mutex);
                            BajoArray[rand].xRespBajo = BajoArray[rand].xBajo;
                            BajoArray[rand].yRespBajo = BajoArray[rand].yBajo;
                        while(BajoArray[rand].yBajo <= ALTO+20){
                            al_rest(VELOCIDAD_MARCIANOS);
                            BajoArray[rand].yBajo+=MOV_MARCIANOS;
                            if(BajoArray[rand].yBajo > FILA_NAVE -50){
                                    if(rand%2 == 0)
                                            BajoArray[rand].xBajo += MOV_MARCIANOS;
                                        else
                                            BajoArray[rand].xBajo-= MOV_MARCIANOS;
                            }
                            else{
                                    BajoArray[rand].xBajo = Nave->xNave;
                            }
                            // Verifica si hay colision de la nave con el marciano
                            if(colision(BajoArray[rand].xBajo, BajoArray[rand].yBajo, Nave->xNave, Nave->yNave)){
                                    BajoArray[rand].xBajo= MAS_ALLA;
                                    BajoArray[rand].xBajo= MAS_ALLA;
                                    BajoArray[rand].visible = 0;
                                    matar_nave();
                                    Nave->colisionado = 1;
                            }
                            }
                            BajoArray[rand].xBajo = BajoArray[rand].xRespBajo;
                            BajoArray[rand].yBajo = BajoArray[rand].yRespBajo;
                            al_unlock_mutex(mutex);
                    }
                    break;
                }
                    case 4:             //Para los ultimos marcianos
                {
                    rand = my_random(0,9);  // Excluye al primer marciano
                    if(BajoBajoArray[rand].visible){
                                al_lock_mutex(mutex);
                                BajoBajoArray[rand].xRespBajo = BajoBajoArray[rand].xBajo;
                                BajoBajoArray[rand].yRespBajo = BajoBajoArray[rand].yBajo;
                            while(BajoBajoArray[rand].yBajo <= ALTO+20){
                                al_rest(VELOCIDAD_MARCIANOS);
                                BajoBajoArray[rand].yBajo+=MOV_MARCIANOS;
                                if(BajoBajoArray[rand].yBajo > FILA_NAVE -50){
                                        if(rand%2 == 0)
                                                BajoBajoArray[rand].xBajo += MOV_MARCIANOS;
                                            else
                                                BajoBajoArray[rand].xBajo-= MOV_MARCIANOS;
                                }
                                else{
                                        BajoBajoArray[rand].xBajo = Nave->xNave;
                                }
                                // Verifica si hay colision de la nave con el marciano
                                if(colision(BajoBajoArray[rand].xBajo, BajoBajoArray[rand].yBajo, Nave->xNave, Nave->yNave)){
                                        BajoBajoArray[rand].xBajo= MAS_ALLA;
                                        BajoBajoArray[rand].yBajo= MAS_ALLA;
                                        BajoBajoArray[rand].visible = 0;
                                        matar_nave();
                                        Nave->colisionado = 1;
                                }
                                }
                                BajoBajoArray[rand].xBajo = BajoBajoArray[rand].xRespBajo;
                                BajoBajoArray[rand].yBajo = BajoBajoArray[rand].yRespBajo;
                                al_unlock_mutex(mutex);
                        }
                        break;
                    }
        }
    }
}

/***
 *    ██╗      ██████╗  ██████╗ ██╗ ██████╗ █████╗
 *    ██║     ██╔═══██╗██╔════╝ ██║██╔════╝██╔══██╗
 *    ██║     ██║   ██║██║  ███╗██║██║     ███████║
 *    ██║     ██║   ██║██║   ██║██║██║     ██╔══██║
 *    ███████╗╚██████╔╝╚██████╔╝██║╚██████╗██║  ██║
 *    ╚══════╝ ╚═════╝  ╚═════╝ ╚═╝ ╚═════╝╚═╝  ╚═╝
 *
 */

// Inicializa los Marcianos
void IniciarMarcianos(){
    int i=0;
    int dist = 60;      // Distancia entre los marcianos

//         BOSS
    int xInicial = COLUMNA_BOSS;    //Coordenadas del Boss mas a la izquierda en la formación fija
    int yInicial = FILA_SUPERIOR;
    for(i; i<CANT_BOSS;i++){ //Inicialización
            BossArray[i].BossImg = al_load_bitmap("img/medio.bmp");
            BossArray[i].xBoss = xInicial;
            BossArray[i].yBoss = yInicial;
            BossArray[i].xRespBoss = xInicial;
            BossArray[i].yRespBoss = yInicial;
            BossArray[i].visible = 1;
            xInicial+=dist;
    }

//         MEDIOS
    i=0;
    xInicial = COLUMNA_MEDIA;    //Coordenadas del Boss mas a la izquierda en la formación fija
    yInicial = FILA_MEDIO;
    for(i; i<CANT_MEDIO;i++){ //Inicialización
            MedioArray[i].MedioImg = al_load_bitmap("img/superior.bmp");
            MedioArray[i].xMedio = xInicial;
            MedioArray[i].yMedio = yInicial;
            MedioArray[i].xRespMedio = xInicial;
            MedioArray[i].yRespMedio = yInicial;
            MedioArray[i].visible = 1;
            // Medios bajos
            MedioBajoArray[i].MedioImg = al_load_bitmap("img/mediobajo.png");
            MedioBajoArray[i].xMedio = xInicial;
            MedioBajoArray[i].yMedio = FILA_MEDIA_BAJA;
            MedioBajoArray[i].xRespMedio = xInicial;
            MedioBajoArray[i].yRespMedio = FILA_MEDIA_BAJA;
            MedioBajoArray[i].visible = 1;
            xInicial+=dist;
    }

//         BAJOS
    i=0;
    xInicial = COLUMNA_BAJO;    //Coordenadas del Boss mas a la izquierda en la formación fija
    yInicial = FILA_BAJA;
    for(i; i<CANT_BAJO;i++){ //Inicialización
            BajoArray[i].BajoImg = al_load_bitmap("img/bajobajo.png");
            BajoArray[i].xBajo = xInicial;
            BajoArray[i].yBajo = yInicial;
            BajoArray[i].xRespBajo = xInicial;
            BajoArray[i].yRespBajo = yInicial;
            BajoArray[i].visible = 1;
            // Ultimos marcianos
            BajoBajoArray[i].BajoImg = al_load_bitmap("img/bajo.bmp");
            BajoBajoArray[i].xBajo = xInicial;
            BajoBajoArray[i].yBajo = FILA_BAJA_BAJA;
            BajoBajoArray[i].xRespBajo = xInicial;
            BajoBajoArray[i].yRespBajo = FILA_BAJA_BAJA;
            BajoBajoArray[i].visible = 1;
            xInicial+=dist;
    }
}

//Una vez creados los marcianos los dibuja en pantalla
// tomando en cuenta si han sido destruidos
void DibujarMarcianos(){

//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((BossArray[i].BossImg), (BossArray[i].xBoss), (BossArray[i].yBoss), 0);
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((MedioArray[i].MedioImg), (MedioArray[i].xMedio), (MedioArray[i].yMedio), 0);
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((MedioBajoArray[i].MedioImg), (MedioBajoArray[i].xMedio), (MedioBajoArray[i].yMedio), 0);
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((BajoArray[i].BajoImg), (BajoArray[i].xBajo), (BajoArray[i].yBajo), 0);
            }

//         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((BajoBajoArray[i].BajoImg), (BajoBajoArray[i].xBajo), (BajoBajoArray[i].yBajo), 0);
            }
}


// Movimiento Derecho de los marcianos
void DerechaMarcianos(){




//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].xBoss += MOV_MARCIANOS;
                BossArray[i].xRespBoss += MOV_MARCIANOS;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].xMedio += MOV_MARCIANOS;
                MedioArray[i].xRespMedio+= MOV_MARCIANOS;
                }
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible){    //Dibuja el arreglo si está visible
                MedioBajoArray[i].xMedio += MOV_MARCIANOS;
                MedioBajoArray[i].xRespMedio+= MOV_MARCIANOS;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].xBajo += MOV_MARCIANOS;
                BajoArray[i].xRespBajo+= MOV_MARCIANOS;
                }
            }

            //         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoBajoArray[i].xBajo += MOV_MARCIANOS;
                BajoBajoArray[i].xRespBajo+= MOV_MARCIANOS;
                }
            }
}


// Movimiento Izquierdo de los marcianos
void IzquierdaMarcianos(){


//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].xBoss -= MOV_MARCIANOS;
                BossArray[i].xRespBoss -= MOV_MARCIANOS;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].xMedio -= MOV_MARCIANOS;
                MedioArray[i].xRespMedio-= MOV_MARCIANOS;
                }
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible){    //Dibuja el arreglo si está visible
                MedioBajoArray[i].xMedio -= MOV_MARCIANOS;
                MedioBajoArray[i].xRespMedio-= MOV_MARCIANOS;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].xBajo -= MOV_MARCIANOS;
                BajoArray[i].xRespBajo -= MOV_MARCIANOS;
                }
            }

            //         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoBajoArray[i].xBajo -= MOV_MARCIANOS;
                BajoBajoArray[i].xRespBajo -= MOV_MARCIANOS;
                }
            }
}

// Movimiento hacia arriba de los marcianos
void ArribaMarcianos(){


//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].yBoss -= MOV_MARCIANOS;
                BossArray[i].yRespBoss -= MOV_MARCIANOS;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].yMedio -= MOV_MARCIANOS;
                MedioArray[i].yRespMedio-= MOV_MARCIANOS;
                }
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible){    //Dibuja el arreglo si está visible
                MedioBajoArray[i].yMedio -= MOV_MARCIANOS;
                MedioBajoArray[i].yRespMedio-= MOV_MARCIANOS;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].yBajo -= MOV_MARCIANOS;
                BajoArray[i].yRespBajo -= MOV_MARCIANOS;
                }
            }

            //         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoBajoArray[i].yBajo -= MOV_MARCIANOS;
                BajoBajoArray[i].yRespBajo -= MOV_MARCIANOS;
                }
            }
}

// Movimiento hacia abajo de los marcianos
void AbajoMarcianos(){


//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].yBoss += MOV_MARCIANOS;
                BossArray[i].yRespBoss += MOV_MARCIANOS;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].yMedio += MOV_MARCIANOS;
                MedioArray[i].yRespMedio += MOV_MARCIANOS;
                }
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible){    //Dibuja el arreglo si está visible
                MedioBajoArray[i].yMedio += MOV_MARCIANOS;
                MedioBajoArray[i].yRespMedio += MOV_MARCIANOS;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].yBajo += MOV_MARCIANOS;
                BajoArray[i].yRespBajo += MOV_MARCIANOS;
                }
            }

            //         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoBajoArray[i].yBajo += MOV_MARCIANOS;
                BajoBajoArray[i].yRespBajo += MOV_MARCIANOS;
                }
            }
}

//Dibuja la nave, segun las posiciones que tenga en el momento de llamar al metodo
void dibujarNave( DatosGlobales * datos){
    al_draw_bitmap((Nave->Imagen_Nave), (Nave->xNave), (Nave->yNave), 0);
    al_draw_bitmap((Vida1->Imagen_Nave), (Vida1->xNave), (Vida1->yNave), 0);
    al_draw_bitmap((Vida2->Imagen_Nave), (Vida2->xNave), (Vida2->yNave), 0);
    if(Nave->colisionado){
                al_draw_bitmap(explosion, (Nave->xNave-15), (Nave->yNave-15), 0);
                al_play_sample(sonido_explosion,1,0,1,ALLEGRO_PLAYMODE_ONCE,0);
                DibujarMarcianos();
                al_flip_display();
                al_rest(0.8);
                al_draw_bitmap(explosion, MAS_ALLA, MAS_ALLA, 0);
                Nave->colisionado = 0;
    }
    }

void dibujarBala(){
                    al_draw_bitmap(Bala->Bala, Bala->xBala, Bala->yBala,0);
    }

/***
 *         ██╗██╗   ██╗███████╗ ██████╗  ██████╗
 *         ██║██║   ██║██╔════╝██╔════╝ ██╔═══██╗
 *         ██║██║   ██║█████╗  ██║  ███╗██║   ██║
 *    ██   ██║██║   ██║██╔══╝  ██║   ██║██║   ██║
 *    ╚█████╔╝╚██████╔╝███████╗╚██████╔╝╚██████╔╝
 *     ╚════╝  ╚═════╝ ╚══════╝ ╚═════╝  ╚═════╝
 *
 */

 //Proceimiento que inicia el juego
int iniciarJuego()
{

    int hubo_disparo;
//Iniciar allegro
    al_init();
    al_init_image_addon(); // Para cargar los bmps

    al_install_audio();         // Carga los sonidos
    al_init_acodec_addon();

    timer = al_create_timer(1.0 / FPS);
   if(!timer) {
      printf("Error en el timer");
      return -1;
   }




if (!al_install_keyboard()) {
        printf("Error en el teclado.");
        return -1;
 }
    Pantalla = al_create_display(ANCHO, ALTO);
    al_set_window_title(Pantalla, "Galaga");

    EventQueue = al_create_event_queue();
    if(!EventQueue){
        printf("Error en la cola de eventos.");
        return -1;
    }
    al_register_event_source(EventQueue, al_get_keyboard_event_source());
    al_register_event_source(EventQueue, al_get_display_event_source(Pantalla));
    al_register_event_source(EventQueue, al_get_timer_event_source(timer));

    // Inicialización de la nave
    Nave = malloc(sizeof(Nave));
    Nave->Imagen_Nave = al_load_bitmap("img/plane.bmp");
    Nave->xNave=COLUMNA_NAVE;
    Nave->yNave=FILA_NAVE;
    Nave->colisionado = 0;

    // Puesto en pantalla de las imágenes de las vidas
    Vida1 = malloc(sizeof(Nave));
    Vida1->Imagen_Nave = al_load_bitmap("img/vida.bmp");
    Vida1->xNave=COLUMNA_VIDAS;
    Vida1->yNave=FILA_VIDAS;

    Vida2 = malloc(sizeof(Nave));
    Vida2->Imagen_Nave = al_load_bitmap("img/vida.bmp");
    Vida2->xNave=COLUMNA_VIDAS + 20;
    Vida2->yNave=FILA_VIDAS;

    // Inicialización de la bala
    Bala = malloc(sizeof(Bala));
    Bala->Bala = al_load_bitmap("img/Bala.bmp");
    Bala->xBala = MAS_ALLA;
    Bala->yBala = MAS_ALLA;
    Bala->disparada = 0;

    //Inicialización de los parámetros
    JuegoDatos = malloc(sizeof (DatosGlobales)); //Datos globales del juego
    JuegoDatos->BG = al_load_bitmap("img/Galaga.png");
    JuegoDatos->jugando = 1;
    JuegoDatos->fin_del_juego = 0;
    JuegoDatos->cantidad_vidas = 2;
    redibujar = 1;

    Puntaje = 0;



// Sonidos del juego
    al_set_window_title(Pantalla,"Iniciando Sonido");
    al_draw_bitmap(JuegoDatos->BG,0,0,0);
    al_flip_display();
    al_reserve_samples(10);
    song = al_load_sample("snd/fondo.ogg");
    shot = al_load_sample("snd/disparo.wav");
    game_over = al_load_sample("snd/gameover.ogg");
    sonido_explosion = al_load_sample("snd/explosion.wav");
    songInstance= al_create_sample_instance(song);
    al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());


// Cambio de fondo
    JuegoDatos->BG = al_load_bitmap("img/nube.bmp");
    al_set_window_title(Pantalla,"Galaga");

// Inicializa los marcianos y la nave
    dibujarNave(JuegoDatos);
    IniciarMarcianos();
    DibujarMarcianos();


    // Creación del hilo para dibujar
    mutex = al_create_mutex();
    hilo_animacion = al_create_thread(anim_marcianos, JuegoDatos);
    al_start_thread(hilo_animacion);

    // Hilo que hace que los marcianos ataquen
    hilo_ataque = al_create_thread(ataque_marcianos,JuegoDatos);
    al_start_thread(hilo_ataque);

    // Hilo que espera por un disparo
    hilo_bala = al_create_thread(espera_balas,JuegoDatos);
    al_start_thread(hilo_bala);

    // Hilo que espera por un disparo
    hilo_colision = al_create_thread(espera_colision,JuegoDatos);
    al_start_thread(hilo_colision);

    al_start_timer(timer);


    // Dibuja la explosion

    explosion = al_load_bitmap("img/explosion.bmp");

// Ciclo principal del juego
    while(JuegoDatos->jugando && !Nave->colisionado)
    {

        al_wait_for_event(EventQueue, &Event);
        if(Event.type == ALLEGRO_EVENT_TIMER)
            redibujar = 1;
        else if(Event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            JuegoDatos->jugando = 0;


//     Eventos del teclado
                    do{
                                     switch(Event.keyboard.keycode)
                                {
                                case ALLEGRO_KEY_LEFT:
                                    if(Nave->xNave > MIN_VENTANA){
                                        Nave->xNave-=TAM_MOVIMIENTO;
                                        redibujar = 1;
                                    }
                                    break;
                                case ALLEGRO_KEY_RIGHT:
                                    if(Nave->xNave < MAX_VENTANA){
                                        Nave->xNave+=TAM_MOVIMIENTO;
                                        redibujar = 1;
                                        }
                                    break;

                                case ALLEGRO_KEY_SPACE:
                                    al_play_sample(shot,1,0,1,ALLEGRO_PLAYMODE_ONCE,0);
                                    Bala->disparada = 1;
                                    break;
                            }
                    } while(Event.type == ALLEGRO_KEY_DOWN);

//     Redibujar
		if(JuegoDatos->jugando){
                // Revisa si algun evento dice que terminó el juego
                    if(JuegoDatos->fin_del_juego){
                                        break;
                                }
                    // Redibuja los elementos de la pantalla
                    if(redibujar && al_is_event_queue_empty(EventQueue)){
                            al_draw_bitmap(JuegoDatos->BG, 0, 0, 0);
                            dibujarNave(JuegoDatos);
                            DibujarMarcianos(JuegoDatos);
                            dibujarBala();
                            al_flip_display();
                            al_play_sample_instance(songInstance);      //Reproducir la música
                            redibujar = 0;
                    }
		}

    }
    printf("Puntos: %d", Puntaje);

// Protocolo del fin del juego
    al_destroy_sample(song);
    JuegoDatos->BG = al_load_bitmap("img/gameover.png");
    al_play_sample(game_over,1,0,1,ALLEGRO_PLAYMODE_ONCE,0);
    al_draw_bitmap(JuegoDatos->BG,0 ,0 , 0);
    al_flip_display();
    al_rest(1.5);
    al_destroy_sample(game_over);
     al_destroy_sample(sonido_explosion);
    al_destroy_sample(shot);
    al_destroy_sample_instance(songInstance);
    al_destroy_timer(timer);
    al_destroy_event_queue(EventQueue);
    al_destroy_display(Pantalla);
    return 0;
}
