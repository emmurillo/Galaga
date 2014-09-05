#include <gtk/gtk.h>

/*Struct para paso de parámetros para mover la nave*/
typedef struct {
    GtkImage *Nave;
    GtkImage *Bala;
    GtkFixed *Panel;
    GtkWindow *Ventana;
    pthread_t *disparar;
    int xAct;
    int yAct;
} Data;
