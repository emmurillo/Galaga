#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h> ///include the header to initialize the Nave addon
#include <unistd.h>
#include "juego.h"


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
 ALLEGRO_DISPLAY *Pantalla;
ALLEGRO_EVENT_QUEUE *EventQueue;
ALLEGRO_EVENT Event;

void refrescar(DatosGlobales * datos){

    al_draw_bitmap(datos->BG, 0, 0, 0);
    al_flip_display();

}

void dibujarNave( DatosGlobales * datos){
///Dibujar la nave segun los ultimos datos pasados
    al_draw_bitmap((datos->Nave->Nave), (datos->Nave->xNave), (datos->Nave->yNave), 0);
    al_flip_display();

}

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

 ///Proceimiento que inicia el juego
int iniciarJuego()
{



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
