#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdio.h>

static void helloWorld (GtkWidget *wid, GtkWidget *win)
{
  GtkWidget *dialog = NULL;

  dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Hello World!");
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

int main (int argc, char *argv[])
{        // User Interface
        gtk_init (&argc, &argv);
        GtkBuilder *builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "src/UI.glade", NULL);

        GObject *window, *bot_inicio;

        window = gtk_builder_get_object (builder, "Vent_Principal");
        g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

        bot_inicio = gtk_builder_get_object (builder, "Bot_Inicio");
        g_signal_connect(bot_inicio, "clicked", G_CALLBACK(helloWorld), NULL);

        gtk_widget_show_all(GTK_WIDGET(window));

        gtk_main();

        return 0;
}
