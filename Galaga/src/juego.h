#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

typedef struct {
    int xNave, yNave;
    ALLEGRO_BITMAP *Nave;
}Nave;

///Struct para paso de parámetros para mover la nave
typedef struct {
    int jugando;                ///Estado del juego
    ALLEGRO_BITMAP *BG; ///Fondo de pantalla
    Nave *Nave;                     ///Nave principal
} DatosGlobales;

#endif // JUEGO_H_INCLUDED
