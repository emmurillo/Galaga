#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h> ///include the header to initialize the Nave addon
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
/*Margenes de la ventana*/
#define MIN_VENTANA 30
#define MAX_VENTANA 630
#define CANT_HILOS 5
/*Cantidad de marcianos*/
#define CANT_BOSS 4
#define CANT_MEDIO 8
#define CANT_BAJO 10
/*Filas en la formación*/
#define FILA_SUPERIOR 50
#define COLUMNA_BOSS 240
#define FILA_MEDIO 100
#define COLUMNA_MEDIA 120
#define FILA_BAJA 150
#define COLUMNA_BAJO 60

DatosGlobales * JuegoDatos;

///Elementos gráficos
 ALLEGRO_DISPLAY *Pantalla;
ALLEGRO_EVENT_QUEUE *EventQueue;
ALLEGRO_EVENT Event;

///Marcianos
BossG BossArray[CANT_BOSS];     ///Arreglo con los Boss Galaga
MedioG MedioArray[CANT_MEDIO];     ///Arreglo con los marcianos del medio
BajoG BajoArray[CANT_BAJO];     ///Arreglo con los marcianos de abajo


/// Hilos
pthread_t *hiloRefrescar;


/// Refresca la pantalla dibujando el BackGround
void refrescar(DatosGlobales * datos){
    al_draw_bitmap(datos->BG, 0, 0, 0);
    al_flip_display();
}

void *ref(void *a){

}
///Dibuja la nave, segun las posiciones que tenga en el momento de llamar al metodo
void dibujarNave( DatosGlobales * datos){
    al_draw_bitmap((datos->Nave->Nave), (datos->Nave->xNave), (datos->Nave->yNave), 0);
    al_flip_display();

}


///Inicializa los Marcianos
void IniciarMarcianos(){
    int i=0;
    int dist = 60;      /// Distancia entre los marcianos

///         BOSS
    int xInicial = COLUMNA_BOSS;    ///Coordenadas del Boss mas a la izquierda en la formación fija
    int yInicial = FILA_SUPERIOR;
    for(i; i<CANT_BOSS;i++){ ///Inicialización
            BossArray[i].BossImg = al_load_bitmap("img/superior.png");
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
            MedioArray[i].MedioImg = al_load_bitmap("img/medio.png");
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
            BajoArray[i].BajoImg = al_load_bitmap("img/bajo.png");
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

    al_flip_display();
}


/// Mueve la bala según las coordenadas de la nave
void disparar(DatosGlobales *datos){
    int yActual=datos->Nave->yNave-20;
    int xActual=datos->Nave->xNave+5;
    ALLEGRO_BITMAP * Bala = al_load_bitmap("img/Bala.png");
    while (yActual>0){
        al_draw_bitmap(Bala, xActual, yActual, 0);
        al_flip_display();
        al_rest(0.05);
        refrescar(datos);
        dibujarNave(datos);
        yActual-=20;
    }
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
    hiloRefrescar = malloc(sizeof(pthread_t*));
    pthread_create(hiloRefrescar, NULL, ref,NULL);
    pthread_join(hiloRefrescar,NULL);
///Iniciar allegro
    al_init();
    al_init_image_addon(); /// Para cargar los bmps

if (!al_install_keyboard()) {
        return 1;
 }
    Pantalla = al_create_display(690, 400);

    EventQueue = al_create_event_queue();
    al_register_event_source(EventQueue, al_get_keyboard_event_source());
    al_register_event_source(EventQueue, al_get_display_event_source(Pantalla));

    ///Inicialización de los parámetros
    JuegoDatos = malloc(sizeof (DatosGlobales)); ///Datos globales del juego
    JuegoDatos->Nave = malloc(sizeof(Nave));
    JuegoDatos->Nave->Nave = al_load_bitmap("img/plane.png");
    JuegoDatos->Nave->xNave=340;
    JuegoDatos->Nave->yNave=350;
    JuegoDatos->jugando = 1;
    JuegoDatos->BG = al_load_bitmap("img/bg.jpg");

    refrescar(JuegoDatos);

    dibujarNave(JuegoDatos);

    IniciarMarcianos();
    DibujarMarcianos();

/// Ciclo principal del juego
    while(JuegoDatos->jugando)
    {

        al_wait_for_event(EventQueue, &Event);

        switch(Event.type){
            case ALLEGRO_EVENT_KEY_DOWN:{
                    if(Event.keyboard.keycode == ALLEGRO_KEY_RIGHT){ ///Movimiento a la izquierda
                        JuegoDatos->Nave->xNave+=20;
                        refrescar(JuegoDatos);
                        dibujarNave(JuegoDatos);
                    }
                    if(Event.keyboard.keycode == ALLEGRO_KEY_LEFT){ ///Movimiento a la derecha
                        JuegoDatos->Nave->xNave-=20;
                        refrescar(JuegoDatos);
                        dibujarNave(JuegoDatos);
                    }
                    if(Event.keyboard.keycode == ALLEGRO_KEY_SPACE){ ///Movimiento a la derecha
                         disparar(JuegoDatos);
                    }
                    break;
            }
            case ALLEGRO_EVENT_DISPLAY_CLOSE:{  ///Cerrar la ventana
                    JuegoDatos->jugando =  0;
                    break;
            }
        }

    }

    return 0;
}
