#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

typedef struct {
    int xNave, yNave;
    ALLEGRO_BITMAP *Nave;
}Nave;

typedef struct {
    int xBoss, yBoss;
    bool herido;    ///Para saber si le han disparado
    ALLEGRO_BITMAP *BossImg;
    bool visible;
}BossG;

typedef struct {
    int xMedio, yMedio;
    ALLEGRO_BITMAP *MedioImg;
    bool visible;
}MedioG;

typedef struct {
    int xBajo, yBajo;
    ALLEGRO_BITMAP *BajoImg;
    bool visible;
}BajoG;

///Struct para paso de parámetros para mover la nave
typedef struct {
    bool jugando;                ///Estado del juego
    ALLEGRO_BITMAP *BG; ///Fondo de pantalla
    Nave *Nave;                     ///Nave principal
} DatosGlobales;

#endif // JUEGO_H_INCLUDED
