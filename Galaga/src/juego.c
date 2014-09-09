#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
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

int posX=300;
int posY=350;
pthread_mutex_t mut;
pthread_t hilo[CANT_HILOS];

  /*Arreglos de marcianos*/
  GtkWidget* Boss[CANT_BOSS];
  GtkWidget* Medio[CANT_BAJO];
  GtkWidget* Bajo[CANT_BAJO];


void lock(){
    pthread_mutex_lock(&mut);
}

void unlock(){
    pthread_mutex_unlock(&mut);
}


/*Funcion para los eventos de las teclas*/
gboolean evento_tecla (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
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
                gtk_fixed_move((params->Panel), GTK_WIDGET(bala), xAct,yAct);
                usleep(10000);
                yAct-=20;
        } while(yAct>0);
        gtk_widget_set_redraw_on_allocate(params->Panel,FALSE);
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

  /*Imagen de fondo */
  GdkPixbufAnimation* anim=gdk_pixbuf_animation_new_from_file("img/bg.jpg",NULL);
  GtkImage* fondo= gtk_image_new_from_animation(anim);


  Datos * params;/*Struct para pasar parametros*/

/*Creación y configuración de la ventana*/
  GameWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(GameWin), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(GameWin), MAX_VENTANA+60, 400);
  gtk_window_set_title(GTK_WINDOW(GameWin), "Galaga");
  gtk_container_set_border_width(GTK_CONTAINER(GameWin), 2);

/*Fixed para mover las imagenes*/
  GameBox = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(GameWin), GameBox);
  gtk_fixed_put(GameBox,fondo,0,0);

/*Declaración de la imagen del avion*/
  Nave = gtk_image_new_from_file("img/plane.png");
  gtk_fixed_put (GTK_FIXED (GameBox), Nave,posX,posY);

/*Declaración de la imagen del boss*/
int bossCont=0;
int xBoss=COLUMNA_BOSS;
for(bossCont; bossCont < CANT_BOSS; bossCont++){
            Boss[bossCont] = gtk_image_new_from_file("img/superior.png");
            gtk_fixed_put (GTK_FIXED (GameBox), Boss[bossCont], xBoss, FILA_SUPERIOR);
            xBoss+=60;
}

/*Declaración de la imagen de los marcianos del medio*/
int medioCont=0;
int xMedio=COLUMNA_MEDIA;
for(medioCont; medioCont < CANT_MEDIO; medioCont++){
            Medio[medioCont] = gtk_image_new_from_file("img/medio.png");
            gtk_fixed_put (GTK_FIXED (GameBox), Medio[medioCont], xMedio, FILA_MEDIO);
            xMedio+=60;
}

/*Declaración de la imagen de los marcianos de abajo*/
int bajoCont=0;
int xBajo=COLUMNA_BAJO;
for(bajoCont; bajoCont < CANT_BAJO; bajoCont++){
            Bajo[bajoCont] = gtk_image_new_from_file("img/bajo.png");
            gtk_fixed_put (GTK_FIXED (GameBox), Bajo[bajoCont], xBajo, FILA_BAJA);
            xBajo+=60;
}




/*Boton de salir*/
  g_signal_connect_swapped(G_OBJECT(GameWin), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(GameWin));

/*Inicialización de los parámetros*/
params=malloc(sizeof(Datos));
params->Panel=GameBox;
params->Nave=Nave;
/*params->Bala=BalaImg;*/

/*Event handler del teclado*/
    g_signal_connect (G_OBJECT (GameWin), "key_press_event", G_CALLBACK (evento_tecla), params);

    hilo[0] = malloc(sizeof(pthread_t));
    pthread_create(hilo[0], NULL, dibujar, GameWin);
    gtk_main();



}
