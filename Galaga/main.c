/*Hola anamari*/
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdio.h>

static void
start_game (GtkWidget *widget,
             gpointer   data)
{
  GtkWidget *GameWin;
  GtkWidget *GameBox;
  GtkWidget *PlaneImg;

  GameWin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(GameWin), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(GameWin), 600, 400);
  gtk_window_set_title(GTK_WINDOW(GameWin), "Galaga");
  gtk_container_set_border_width(GTK_CONTAINER(GameWin), 5);


  gtk_container_set_border_width(GTK_CONTAINER(GameWin), 2);

  GameBox = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(GameWin), GameBox);

  PlaneImg = gtk_image_new_from_file("src/plane.png");

  gtk_fixed_put (GTK_FIXED (GameBox), PlaneImg,300,300);

  g_signal_connect_swapped(G_OBJECT(GameWin), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(GameWin));

  gtk_widget_show_all(GameWin);
}


int main( int argc, char *argv[])
{

  GtkWidget *window;
  GtkWidget *vbox;

  GtkWidget *start;
  GtkWidget *help;
  GtkWidget *close;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(window), "Galaga");
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);

  vbox = gtk_vbox_new(TRUE, 1);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  start = gtk_button_new_with_label("Iniciar");
  help = gtk_button_new_with_label("Ayuda");
  close = gtk_button_new_with_label("Cerrar");

  gtk_box_pack_start(GTK_BOX(vbox), start, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), help, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), close, TRUE, TRUE, 0);

  g_signal_connect_swapped(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(window));

  g_signal_connect(G_OBJECT(start), "clicked",
      G_CALLBACK(start_game), NULL);

  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
