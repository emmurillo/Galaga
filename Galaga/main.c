/*
██╗████████╗ ██████╗   ██████╗
██║╚══██╔══╝██╔════╝   ██╔══██╗
██║   ██║   ██║        ██████╔╝
██║   ██║   ██║        ██╔══██╗
██║██╗██║██╗╚██████╗██╗██║  ██║██╗
╚═╝╚═╝╚═╝╚═╝ ╚═════╝╚═╝╚═╝  ╚═╝╚═╝


 ██████╗  █████╗ ██╗      █████╗  ██████╗  █████╗
██╔════╝ ██╔══██╗██║     ██╔══██╗██╔════╝ ██╔══██╗
██║  ███╗███████║██║     ███████║██║  ███╗███████║
██║   ██║██╔══██║██║     ██╔══██║██║   ██║██╔══██║
╚██████╔╝██║  ██║███████╗██║  ██║╚██████╔╝██║  ██║
 ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝


    Emma
    Anamari
*/

/*TEC*/

/*Librerias*/
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "Headers/Estructuras.h"

/*Macros*/
#define TAM_MOVIMIENTO 35 /*Tamaño de movimiento de la nave*/
/*Margenes de la ventana*/
#define MIN_VENTANA 30
#define MAX_VENTANA 530

/*Variables Globales*/
int posX;
int posY;
pthread_mutex_t mut;


void delay(){
    int i=0;
    for(i;i<5000000;i++);
}

void lock(){
    pthread_mutex_lock(&mut);
}

void unlock(){
    pthread_mutex_unlock(&mut);
}

/*Redibuja un widget*/
static gboolean refrescar(GtkWidget *widget)
{
    gtk_widget_queue_draw(widget);
    return TRUE;
}

void *disparar(void *datos){
/*Imagen de la bala*/
    Data *data = (Data*)datos;
    GtkImage *BalaImg;
    BalaImg = gtk_image_new_from_file("src/Bala.png");
    gtk_fixed_put (GTK_FIXED (data->Panel), BalaImg, data->xAct, data->yAct);
    int yBala = data->yAct;
    do{
        lock();
         printf("%d\n",yBala);
         gtk_fixed_move ( GTK_FIXED (data->Panel),GTK_WIDGET (BalaImg),(data->xAct+6),(yBala-20));
         delay();
         yBala-=20;
         unlock();
    }
    while (yBala >= 10);
    gtk_widget_destroy(BalaImg);
    pthread_exit(0);
}

/*Funcion para los eventos de las teclas*/
gboolean on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
Data *params;
params=(Data*)user_data;
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
        pthread_create((params->disparar),NULL,disparar,params);
      break;
    default:
      return FALSE;
  }

  return FALSE;
}


/*Callback del boton que inicia la ventana del juego*/
static void start_game (GtkWidget *widget, gpointer   data)
{

/*Widgets de la ventana de juego*/
  GtkWidget *GameWin;
  GtkWidget *GameBox;/*Panel para dibujar*/
  GtkWidget *PlaneImg;
  Data * params;/*Struct para pasar parametros*/

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
  PlaneImg = gtk_image_new_from_file("src/plane.png");
  gtk_fixed_put (GTK_FIXED (GameBox), PlaneImg,posX,posY);


/*Boton de salir*/
  g_signal_connect_swapped(G_OBJECT(GameWin), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(GameWin));

/*Inicialización de los parámetros*/
params=malloc(sizeof(Data));
params->Panel=GameBox;
params->Nave=PlaneImg;
/*params->Bala=BalaImg;*/

/*Event handler del teclado*/
    g_signal_connect (G_OBJECT (GameWin), "key_press_event", G_CALLBACK (on_key_press), params);


/*Mostrar la ventana*/
gtk_widget_show_all(GameWin);

}





int main( int argc, char *_argv[])
{



/*Widgets del menú de inicio*/
  GtkWidget *window;
  GtkWidget *vbox;

/*Botones del menú de inicio*/
  GtkWidget *start;
  GtkWidget *help;
  GtkWidget *close;

/*Inicialización de GTK*/
  gtk_init(&argc, &_argv);
  posX=300;
  posY=350;

/*Creación y configuración de la ventana*/
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(window), "Galaga");
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

/*Creación y configuración del contenedor de los botones */
  vbox = gtk_vbox_new(TRUE, 1);
  gtk_container_add(GTK_CONTAINER(window), vbox);

/*Creación de los botones*/
  start = gtk_button_new_with_label("Iniciar");
  help = gtk_button_new_with_label("Ayuda");
  close = gtk_button_new_with_label("Cerrar");


  gtk_box_pack_start(GTK_BOX(vbox), start, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), help, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), close, TRUE, TRUE, 0);


/*Event handlers de los botones del menú principal*/

  g_signal_connect_swapped(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(window));

  g_signal_connect(G_OBJECT(start), "clicked",
      G_CALLBACK(start_game), NULL);

    g_signal_connect(G_OBJECT(close), "clicked",
      G_CALLBACK(gtk_main_quit), G_OBJECT(window));

/*Mostrar la ventana*/
  gtk_widget_show_all(window);

/*Ciclo del programa*/
  gtk_main();

  return 0;
}
