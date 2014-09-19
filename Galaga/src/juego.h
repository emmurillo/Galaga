#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

typedef struct {
    int xNave, yNave;
    int colisionado;
    ALLEGRO_BITMAP *Imagen_Nave;
}NaveG;

typedef struct {
    int xBala, yBala;
    ALLEGRO_BITMAP *Bala;
    int disparada;
}BalaG;

typedef struct {
    int xBoss, yBoss;
    int xRespBoss, yRespBoss;
    int herido;    ///Para saber si le han disparado
    ALLEGRO_BITMAP *BossImg;
    int visible;
}BossG;

typedef struct {
    int xMedio, yMedio;
    int xRespMedio, yRespMedio; ///Respaldos para que retomen la formación
    ALLEGRO_BITMAP *MedioImg;
    int visible;
}MedioG;

typedef struct {
    int xBajo, yBajo;
    int xRespBajo, yRespBajo;
    ALLEGRO_BITMAP *BajoImg;
    int visible;
}BajoG;

///Struct para paso de parámetros para mover la nave
typedef struct {
    long int puntaje;
    int cantidad_vidas;
    int jugando;                ///Estado del juego
    int fin_del_juego;
    ALLEGRO_BITMAP *BG; ///Fondo de pantalla
    ALLEGRO_BITMAP *GameOver;
} DatosGlobales;

#endif // JUEGO_H_INCLUDED
