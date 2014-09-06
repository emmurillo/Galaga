#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "juego.h"

/*Macros*/
#define TAM_MOVIMIENTO 35 /*Tamaño de movimiento de la nave*/
/*Margenes de la ventana*/
#define MIN_VENTANA 30
#define MAX_VENTANA 530
#define CANT_HILOS 5

int posX=300;
int posY=350;
pthread_mutex_t mut;
pthread_t hilo[CANT_HILOS];


void lock(){
    pthread_mutex_lock(&mut);
}

void unlock(){
    pthread_mutex_unlock(&mut);
}


/*Funcion para los eventos de las teclas*/
gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
Datos *params;
params=(Datos*)user_data;
  switch (event->keyval)
  {
    case GDK_Right:
      if(posX<MAX_VENTANA){
        (posX)+=TAM_MOVIMIENTO;
        gtk_fixed_move( GTK_FIXED (params->Panel),GTK_WIDGET (params->Nave),(posX),(posY));
        }
      break;
    case GDK_Left:
      if((posX)>MIN_VENTANA){
        (posX)-=TAM_MOVIMIENTO;
        gtk_fixed_move( GTK_FIXED (params->Panel),GTK_WIDGET (params->Nave),(posX),(posY));
        }
      break;
    case GDK_space:
        printf("Disparo\n");
        GtkImage *bala=gtk_image_new_from_file("img/Bala.png");
        int xAct=posX+3;
        int yAct=posY-20;
        gtk_fixed_put((params->Panel), bala, xAct,yAct);
        do{
                gtk_widget_show_now(bala);
                usleep(1000);
                gtk_fixed_move((params->Panel), bala, xAct,yAct);
                usleep(10000);
                gtk_widget_show_now(bala);
                yAct-=20;
        } while(yAct>0);
      break;
    default:
      return FALSE;
  }

  return FALSE;
}

void *dibujar(void *_datos){
    GtkWindow *vent;
    vent = (GtkWindow*)_datos;
    /*Mostrar la ventana*/
  gtk_widget_show_all(vent);
}

void dibujarPantalla(int* argc,char **argv[]){

/*Inicialización de GTK*/
  gtk_init(argc, argv);

/*Widgets de la ventana de juego*/
  GtkWidget *GameWin;
  GtkWidget *GameBox;/*Panel para dibujar*/
  GtkWidget *Nave;
  Datos * params;/*Struct para pasar parametros*/

/*Creación y configuración de la ventana*/
  GameWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(GameWin), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(GameWin), 600, 400);
  gtk_window_set_title(GTK_WINDOW(GameWin), "Galaga");
  gtk_container_set_border_width(GTK_CONTAINER(GameWin), 2);

/*Fixed para mover las imagenes*/
  GameBox = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(GameWin), GameBox);

/*Declaración de la imagen del avion*/
  Nave = gtk_image_new_from_file("img/plane.png");
  gtk_fixed_put (GTK_FIXED (GameBox), Nave,posX,posY);


/*Boton de salir*/
  g_signal_connect_swapped(G_OBJECT(GameWin), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(GameWin));

/*Inicialización de los parámetros*/
params=malloc(sizeof(Datos));
params->Panel=GameBox;
params->Nave=Nave;
/*params->Bala=BalaImg;*/

/*Event handler del teclado*/
    g_signal_connect (G_OBJECT (GameWin), "key_press_event", G_CALLBACK (on_key_press), params);

    hilo[0] = malloc(sizeof(pthread_t));
    pthread_create(hilo[0], NULL, dibujar, GameWin);
    sleep(1);

  gtk_main();
}
