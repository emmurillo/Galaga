#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <unistd.h>
#include <pthread.h>
#include "juego.h"


/*
Notas:
- Los Marcianos se llaman Boss , Medio y Bajo
- Faltan hilos
*/

/*Macros*/
#define TAM_MOVIMIENTO 30 /*Tamaño de movimiento de la nave*/
#define ANCHO 700
#define ALTO 400
/*Margenes de la ventana*/
#define MIN_VENTANA 30
#define MAX_VENTANA 630
#define CANT_HILOS 5
/*Cantidad de marcianos*/
#define CANT_BOSS 4
#define CANT_MEDIO 8
#define CANT_BAJO 10
/*Filas en la formación*/
#define COLUMNA_NAVE ANCHO/2
#define FILA_NAVE 350
#define FILA_SUPERIOR 50
#define COLUMNA_BOSS 235
#define FILA_MEDIO 100
#define COLUMNA_MEDIA 115
#define FILA_BAJA 150
#define COLUMNA_BAJO 55


#define CANT_BALAS 5

/*Velocidades de los marcianos*/
#define VELOCIDAD_MARCIANOS 0.5
#define VELOCIDAD_BALA 0.09

#define ARR_BALA_X 14
#define ARR_BALA_Y 20

DatosGlobales * JuegoDatos;

///Elementos gráficos
 ALLEGRO_DISPLAY *Pantalla;
ALLEGRO_EVENT_QUEUE *EventQueue;
ALLEGRO_EVENT Event;
const float FPS = 24;
bool redibujar;
ALLEGRO_TIMER *timer = NULL;

///Marcianos
BossG BossArray[CANT_BOSS];     ///Arreglo con los Boss Galaga
MedioG MedioArray[CANT_MEDIO];     ///Arreglo con los marcianos del medio
BajoG BajoArray[CANT_BAJO];     ///Arreglo con los marcianos de abajo

///Balas
BalaG *Bala[CANT_BALAS];


/// Hilos
pthread_t *hiloRefrescar;
pthread_mutex_t *mut;
ALLEGRO_THREAD * hilo_bala[CANT_BALAS];
ALLEGRO_THREAD * hilo_animacion;
ALLEGRO_MUTEX *mutex;

///Funciones para los hilos

void lock(){
        pthread_mutex_lock(mut);
}

void unlock(){
        pthread_mutex_unlock(mut);
}



void *disparar(ALLEGRO_THREAD *thr, void *datos){
    bool listo;
    int xBala,yBala, act;
    act = JuegoDatos->Nave->cont_balas;
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    xBala = (mis_datos->Nave->xNave)+ARR_BALA_X;
    yBala = mis_datos->Nave->yNave+(ARR_BALA_Y);
    listo=true;
    Bala[JuegoDatos->Nave->cont_balas]->disparada = true;
    while(listo){
            if(xBala>0){
                al_rest(VELOCIDAD_BALA);
                al_lock_mutex(mutex);
                yBala -= ARR_BALA_Y;
                Bala[act]->yBala = yBala;
                Bala[act]->xBala = xBala;
                al_unlock_mutex(mutex);
            }
            else{
                    listo = false;
            }
    }
}

void *anim_marcianos(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    while(1){
            al_rest(VELOCIDAD_MARCIANOS);
            DerechaMarcianos();
            al_rest(VELOCIDAD_MARCIANOS);
            ArribaMarcianos();
            al_rest(VELOCIDAD_MARCIANOS);
            IzquierdaMarcianos();
            al_rest(VELOCIDAD_MARCIANOS);
            AbajoMarcianos();

    }
}

/// Refresca la pantalla dibujando el BackGround
void refrescar(DatosGlobales * datos){
    al_draw_bitmap(datos->BG, 0, 0, 0);
    al_flip_display();
}

///Dibuja la nave, segun las posiciones que tenga en el momento de llamar al metodo
void dibujarNave( DatosGlobales * datos){
    al_draw_bitmap((datos->Nave->Nave), (datos->Nave->xNave), (datos->Nave->yNave), 0);

}


///Inicializa los Marcianos
void IniciarMarcianos(){
    int i=0;
    int dist = 60;      /// Distancia entre los marcianos

///         BOSS
    int xInicial = COLUMNA_BOSS;    ///Coordenadas del Boss mas a la izquierda en la formación fija
    int yInicial = FILA_SUPERIOR;
    for(i; i<CANT_BOSS;i++){ ///Inicialización
            BossArray[i].BossImg = al_load_bitmap("img/superior.bmp");
            BossArray[i].xBoss = xInicial;
            BossArray[i].yBoss = yInicial;
            BossArray[i].visible = true;
            xInicial+=dist;
    }

///         MEDIOS
    i=0;
    xInicial = COLUMNA_MEDIA;    ///Coordenadas del Boss mas a la izquierda en la formación fija
    yInicial = FILA_MEDIO;
    for(i; i<CANT_MEDIO;i++){ ///Inicialización
            MedioArray[i].MedioImg = al_load_bitmap("img/medio.bmp");
            MedioArray[i].xMedio = xInicial;
            MedioArray[i].yMedio = yInicial;
            MedioArray[i].visible = true;
            xInicial+=dist;
    }

///         BAJOS
    i=0;
    xInicial = COLUMNA_BAJO;    ///Coordenadas del Boss mas a la izquierda en la formación fija
    yInicial = FILA_BAJA;
    for(i; i<CANT_BAJO;i++){ ///Inicialización
            BajoArray[i].BajoImg = al_load_bitmap("img/bajo.bmp");
            BajoArray[i].xBajo = xInicial;
            BajoArray[i].yBajo = yInicial;
            BajoArray[i].visible = true;
            xInicial+=dist;
    }
}

///Una vez creados los marcianos los dibuja en pantalla
/// tomando en cuenta si han sido destruidos
void DibujarMarcianos(){

///         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {///Puesta de los boss en pantalla
            if(BossArray[i].visible)    ///Dibuja el arreglo si está visible
                al_draw_bitmap((BossArray[i].BossImg), (BossArray[i].xBoss), (BossArray[i].yBoss), 0);
            }

///         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {///Puesta de los boss en pantalla
            if(MedioArray[i].visible)    ///Dibuja el arreglo si está visible
                al_draw_bitmap((MedioArray[i].MedioImg), (MedioArray[i].xMedio), (MedioArray[i].yMedio), 0);
            }

///         MEDIO
    i=0;
    for(i; i<CANT_BAJO;i++) {///Puesta de los boss en pantalla
            if(BajoArray[i].visible)    ///Dibuja el arreglo si está visible
                al_draw_bitmap((BajoArray[i].BajoImg), (BajoArray[i].xBajo), (BajoArray[i].yBajo), 0);
            }
}


/// Movimiento Derecho de los marcianos
void DerechaMarcianos(){


if(BajoArray[9].xBajo >  620 ){       /// Límite para dejar de bajar antes de atacar
    DibujarMarcianos();
    return;
    }

///         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {///Puesta de los boss en pantalla
            if(BossArray[i].visible){    ///Dibuja el arreglo si está visible
                BossArray[i].xBoss += TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {///Puesta de los boss en pantalla
            if(MedioArray[i].visible){    ///Dibuja el arreglo si está visible
                MedioArray[i].xMedio += TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_BAJO;i++) {///Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   ///Dibuja el arreglo si está visible
                BajoArray[i].xBajo += TAM_MOVIMIENTO;
                }
            }
}


/// Movimiento Izquierdo de los marcianos
void IzquierdaMarcianos(){

if(BajoArray[0].xBajo < 50 ){       /// Límite para dejar de bajar antes de atacar
    DibujarMarcianos();
    return;
    }

///         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {///Puesta de los boss en pantalla
            if(BossArray[i].visible){    ///Dibuja el arreglo si está visible
                BossArray[i].xBoss -= TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {///Puesta de los boss en pantalla
            if(MedioArray[i].visible){    ///Dibuja el arreglo si está visible
                MedioArray[i].xMedio -= TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_BAJO;i++) {///Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   ///Dibuja el arreglo si está visible
                BajoArray[i].xBajo -= TAM_MOVIMIENTO;
                }
            }
}

/// Movimiento Arriba de los marcianos
void ArribaMarcianos(){


if(BossArray[0].yBoss < 50){       /// Límite para dejar de subir
    DibujarMarcianos();
    return;
    }
///         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {///Puesta de los boss en pantalla
            if(BossArray[i].visible){    ///Dibuja el arreglo si está visible
                BossArray[i].yBoss -= TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {///Puesta de los boss en pantalla
            if(MedioArray[i].visible){    ///Dibuja el arreglo si está visible
                MedioArray[i].yMedio -= TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_BAJO;i++) {///Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   ///Dibuja el arreglo si está visible
                BajoArray[i].yBajo -= TAM_MOVIMIENTO;
                }
            }
}


/// Movimiento Arriba de los marcianos
void AbajoMarcianos(){


if(BajoArray[0].yBajo > 200 ){       /// Límite para dejar de bajar antes de atacar
    DibujarMarcianos();
    return;
    }
///         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {///Puesta de los boss en pantalla
            if(BossArray[i].visible){    ///Dibuja el arreglo si está visible
                BossArray[i].yBoss += TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {///Puesta de los boss en pantalla
            if(MedioArray[i].visible){    ///Dibuja el arreglo si está visible
                MedioArray[i].yMedio += TAM_MOVIMIENTO;
                }
            }

///         MEDIO
    i=0;
    for(i; i<CANT_BAJO;i++) {///Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   ///Dibuja el arreglo si está visible
                BajoArray[i].yBajo += TAM_MOVIMIENTO;
                }
            }
}


void mover_bala (){
        al_draw_bitmap((Bala[JuegoDatos->Nave->cont_balas]->Bala),(Bala[0]->xBala),(Bala[0]->yBala),0);
}



/***
 *    ██╗███╗    ██╗██╗ ██████╗ ██╗   ██████╗
 *    ██║████╗   ██║██║ ██╔════╝██║ ██╔═══██╗
 *    ██║██╔██╗  ██║██║ ██║        ██║ ██║    ██║
 *    ██║██║╚██╗ ██║██║ ██║        ██║ ██║    ██║
 *    ██║██║ ╚████ ║██║ ╚██████╗ ██║╚██████╔╝
 *    ╚═╝╚═╝  ╚═══╝  ╚═╝     ╚═════╝╚═╝       ╚═════╝
 *
 */

 ///Proceimiento que inicia el juego
int iniciarJuego()
{

    bool hubo_disparo;
///Iniciar allegro
    al_init();
    al_init_image_addon(); /// Para cargar los bmps

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

    ///Inicialización de los parámetros
    JuegoDatos = malloc(sizeof (DatosGlobales)); ///Datos globales del juego
    JuegoDatos->Nave = malloc(sizeof(Nave));
    JuegoDatos->Nave->Nave = al_load_bitmap("img/plane.bmp");
    JuegoDatos->BG = al_load_bitmap("img/nube.bmp");
    JuegoDatos->Nave->xNave=COLUMNA_NAVE;
    JuegoDatos->Nave->yNave=FILA_NAVE;
    JuegoDatos->Nave->cont_balas = 0;
    JuegoDatos->jugando = true;
    redibujar = true;

int i = 0;
for(i ; i < CANT_BALAS ; i++){
    Bala[i] = malloc(sizeof(BalaG));
    Bala[i]->xBala = JuegoDatos->Nave->xNave;
    Bala[i]->yBala = JuegoDatos->Nave->yNave;
    Bala[i]->Bala = al_load_bitmap("img/Bala.bmp");
}

i = 0;
for(i ; i < CANT_BALAS ; i++){
    al_draw_bitmap((Bala[i]->Bala),-10,-10,0);
    hilo_bala[i] = al_create_thread(disparar, JuegoDatos);
}

    dibujarNave(JuegoDatos);

    IniciarMarcianos();

    DibujarMarcianos();

    hilo_animacion = al_create_thread(anim_marcianos, JuegoDatos);
    al_start_thread(hilo_animacion);
// Creación del hilo para dibujar
    mutex = al_create_mutex();

    al_start_timer(timer);

/// Ciclo principal del juego
    while(JuegoDatos->jugando)
    {

        al_wait_for_event(EventQueue, &Event);
        if(Event.type == ALLEGRO_EVENT_TIMER)
            redibujar = true;
        else if(Event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            JuegoDatos->jugando = false;


///     Eventos del teclado
            do{
                             switch(Event.keyboard.keycode)
                        {
                        case ALLEGRO_KEY_LEFT:
                            if(JuegoDatos->Nave->xNave > MIN_VENTANA){
                                JuegoDatos->Nave->xNave-=TAM_MOVIMIENTO;
                                redibujar = true;
                            }
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            if(JuegoDatos->Nave->xNave < MAX_VENTANA){
                                JuegoDatos->Nave->xNave+=TAM_MOVIMIENTO;
                                redibujar = true;
                                }
                            break;

                        case ALLEGRO_KEY_SPACE:
                            al_start_thread(hilo_bala[JuegoDatos->Nave->cont_balas]);
                            break;
                    }
            } while(Event.type == ALLEGRO_KEY_DOWN);


///     Redibujar
		if(JuegoDatos->jugando){
                if(redibujar && al_is_event_queue_empty(EventQueue)){
                        al_draw_bitmap(JuegoDatos->BG, 0, 0, 0);
                        if(Bala[JuegoDatos->Nave->cont_balas]->disparada){
                                mover_bala();
                        }
                        dibujarNave(JuegoDatos);
                        DibujarMarcianos(JuegoDatos);
                        al_flip_display();
                        redibujar = false;
                }
		}

    }
    al_destroy_timer(timer);
    al_destroy_event_queue(EventQueue);
    al_destroy_display(Pantalla);
    return 0;
}
