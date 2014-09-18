#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

typedef struct {
    int xNave, yNave;
    bool colisionado;
    ALLEGRO_BITMAP *Imagen_Nave;
}NaveG;

typedef struct {
    int xBala, yBala;
    ALLEGRO_BITMAP *Bala;
    bool disparada;
}BalaG;

typedef struct {
    int xBoss, yBoss;
    int xRespBoss, yRespBoss;
    bool herido;    ///Para saber si le han disparado
    ALLEGRO_BITMAP *BossImg;
    bool visible;
}BossG;

typedef struct {
    int xMedio, yMedio;
    int xRespMedio, yRespMedio; ///Respaldos para que retomen la formación
    ALLEGRO_BITMAP *MedioImg;
    bool visible;
}MedioG;

typedef struct {
    int xBajo, yBajo;
    int xRespBajo, yRespBajo;
    ALLEGRO_BITMAP *BajoImg;
    bool visible;
}BajoG;

///Struct para paso de parámetros para mover la nave
typedef struct {
    int cantidad_vidas;
    bool jugando;                ///Estado del juego
    bool fin_del_juego;
    ALLEGRO_BITMAP *BG; ///Fondo de pantalla
} DatosGlobales;

#endif // JUEGO_H_INCLUDED
