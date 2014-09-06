#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "juego.h"

/*Macros*/
#define TAM_MOVIMIENTO 35 /*Tamaño de movimiento de la nave*/
/*Margenes de la ventana*/
#define MIN_VENTANA 30
#define MAX_VENTANA 530

int posX=300;
int posY=350;


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
        params->disparar= malloc(sizeof(pthread_t));
        printf("Espacio\n");
        params->xAct=posX;
        params->yAct=posY;
      break;
    default:
      return FALSE;
  }

  return FALSE;
}


void dibujarPantalla(int* argc,char **argv[]){

/*Inicialización de GTK*/
  gtk_init(argc, argv);

/*Widgets de la ventana de juego*/
  GtkWidget *GameWin;
  GtkWidget *GameBox;/*Panel para dibujar*/
  GtkWidget *PlaneImg;
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
  PlaneImg = gtk_image_new_from_file("img/plane.png");
  gtk_fixed_put (GTK_FIXED (GameBox), PlaneImg,posX,posY);


/*Boton de salir*/
  g_signal_connect_swapped(G_OBJECT(GameWin), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(GameWin));

/*Inicialización de los parámetros*/
params=malloc(sizeof(Datos));
params->Panel=GameBox;
params->Nave=PlaneImg;
/*params->Bala=BalaImg;*/

/*Event handler del teclado*/
    g_signal_connect (G_OBJECT (GameWin), "key_press_event", G_CALLBACK (on_key_press), params);

/*Mostrar la ventana*/
  gtk_widget_show_all(GameWin);

/*Ciclo del programa*/
  gtk_main();

}
