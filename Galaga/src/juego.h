#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

/*Struct para paso de parámetros para mover la nave*/
typedef struct {
    GtkImage *Nave;
    GtkImage *Bala;
    GtkFixed *Panel;
    GtkWindow *Ventana;
    pthread_t *disparar;
    int xAct;
    int yAct;
} Datos;

void dibujarPantalla(int *argc,char **argv[]);

#endif // JUEGO_H_INCLUDED
