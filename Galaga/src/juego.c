#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <unistd.h>
#include <pthread.h>
#include "juego.h"
#include "random.h"


/*
Notas:
- Los Marcianos se llaman Boss , Medio y Bajo
- Faltan hilos
*/

/*Macros*/
#define TAM_MOVIMIENTO 10 /*Tamaño de movimiento de la nave*/
#define ANCHO 700
#define ALTO 400
/*Margenes de la ventana*/
#define MIN_VENTANA 30
#define MAX_VENTANA 630
#define CANT_HILOS 5
/*Cantidad de marcianos*/
#define CANT_BOSS 4
#define CANT_MEDIO 8
#define CANT_BAJO 10
/*Filas en la formación*/
#define COLUMNA_NAVE ANCHO/2
#define FILA_NAVE 350
#define COLUMNA_VIDAS 10
#define FILA_VIDAS 10

#define FILA_SUPERIOR 30
#define COLUMNA_BOSS 235
#define FILA_MEDIO 65
#define FILA_MEDIA_BAJA 100
#define COLUMNA_MEDIA 115
#define FILA_BAJA 135
#define FILA_BAJA_BAJA 170
#define COLUMNA_BAJO 55


#define MAS_ALLA 1000

#define CANT_BALAS 5

/*Velocidades de los marcianos*/
#define VELOCIDAD_MARCIANOS 0.3
#define VELOCIDAD_BALA 0.09

#define ARR_BALA_X 14
#define ARR_BALA_Y -14

DatosGlobales * JuegoDatos;

//Elementos gráficos
 ALLEGRO_DISPLAY *Pantalla;
ALLEGRO_EVENT_QUEUE *EventQueue;
ALLEGRO_EVENT Event;
const float FPS = 24;
bool redibujar;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_SAMPLE *shot= NULL;
ALLEGRO_SAMPLE *song= NULL;
ALLEGRO_SAMPLE *sonido_explosion= NULL;
ALLEGRO_SAMPLE_INSTANCE *songInstance  = NULL;

// Explosión
ALLEGRO_BITMAP *explosion;

//Marcianos
BossG BossArray[CANT_BOSS];     //Arreglo con los Boss Galaga
MedioG MedioArray[CANT_MEDIO];     //Arreglo con los marcianos del medio
MedioG MedioBajoArray[CANT_MEDIO];     //Arreglo con los marcianos del medio de abajo
BajoG BajoArray[CANT_BAJO];     //Arreglo con los marcianos de abajo
BajoG BajoBajoArray[CANT_BAJO];     //Arreglo con los marcianos de abajo

// Arreglo de naves
NaveG *Nave;
NaveG *Vida1;
NaveG *Vida2;


// Balas
BalaG *Bala;


// Hilos
ALLEGRO_THREAD * hilo_bala;
ALLEGRO_THREAD * hilo_animacion;
ALLEGRO_THREAD * hilo_ataque;
ALLEGRO_MUTEX *mutex;

// Funciones para los hilos





void *anim_marcianos(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    while(JuegoDatos->jugando){
            al_rest(VELOCIDAD_MARCIANOS);
            DerechaMarcianos();
            al_rest(VELOCIDAD_MARCIANOS);
            IzquierdaMarcianos();


    }
}


// Hilo que espera a que una bala sea disparada
void *espera_balas(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    while(JuegoDatos->jugando){
            if(Bala->disparada){
                    Bala->xBala = Nave->xNave + ARR_BALA_X;
                    Bala->yBala = Nave->yNave + ARR_BALA_Y;
                    while(Bala->yBala > 0){
                            Bala->yBala -= 20;
                            al_rest(0.05);
                    }
                            Bala->xBala = MAS_ALLA;
                            Bala->yBala = MAS_ALLA;
                            Bala->disparada = false;
            }
    }
}


// Determina si dos puntos están cercanos con un rango de 25 pixeles
bool cercano(int x1,int x2){
            return abs(x1-x2) < 25;
}

// Indica si hay una colision entre 2 coordenadas
bool colision(int xMarciano,int yMarciano, int xNave,int yNave){
        if(cercano(yMarciano, yNave) && cercano(xMarciano, xNave)){
                return true;
        }
        return false;
}


void matar_nave(){  // Determina la cantidad de vidas y las desaparece de pantalla
        switch(JuegoDatos->cantidad_vidas){
                case 0:
                            JuegoDatos->fin_del_juego = true;
                case 2:
                            Vida2->xNave=-100; // Desaparece una vida de la pantalla
                            Vida2->yNave=-100;
                            JuegoDatos->cantidad_vidas--;
                            break;
                case 1:
                            Vida1->xNave=-100; // Desaparece una vida de la pantalla
                            Vida1->yNave=-100;
                            JuegoDatos->cantidad_vidas--;
                            break;
                default:
                            break;
}

}


// Hilo que hace que los marcianos ataquen
void *ataque_marcianos(ALLEGRO_THREAD *thr, void *datos){
    DatosGlobales * mis_datos = malloc(sizeof(DatosGlobales));
    mis_datos = (DatosGlobales*)datos;
    while(mis_datos->jugando){
    int rand = my_random(0,4);
    switch(rand){
            case 0:  //Para los boss
            {
                        rand = my_random(0,3);
                        if(BossArray[rand].visible){
                                        al_lock_mutex(mutex);
                                        BossArray[rand].xRespBoss = BossArray[rand].xBoss;
                                        BossArray[rand].yRespBoss = BossArray[rand].yBoss;
                                    while(BossArray[rand].yBoss <= ALTO+50){
                                        al_rest(0.100008);
                                        BossArray[rand].yBoss+=10;
                                        if(BossArray[rand].yBoss > FILA_NAVE -50){
                                                BossArray[rand].xBoss += 15;
                                        }
                                        else{
                                                BossArray[rand].xBoss = Nave->xNave;
                                        }
                                        // Verifica si hay colision de la nave con el marciano
                                        if(colision(BossArray[rand].xBoss, BossArray[rand].yBoss, Nave->xNave, Nave->yNave)){
                                                BossArray[rand].xBoss = MAS_ALLA;
                                                BossArray[rand].yBoss = MAS_ALLA;
                                                BossArray[rand].visible = false;
                                                matar_nave();
                                                Nave->colisionado = true;
                                        }
                                        }
                                        BossArray[rand].xBoss = BossArray[rand].xRespBoss;
                                        BossArray[rand].yBoss = BossArray[rand].yRespBoss;
                                        al_unlock_mutex(mutex);
                            }
                            break;
                }
            case 1:     //PAra los del medio
            {
                        rand = my_random(0,7);
                        if(MedioArray[rand].visible){
                                    al_lock_mutex(mutex);
                                    MedioArray[rand].xRespMedio = MedioArray[rand].xMedio;
                                    MedioArray[rand].yRespMedio = MedioArray[rand].yMedio;
                                while(MedioArray[rand].yMedio <= ALTO+50){
                                    al_rest(0.100008);
                                    MedioArray[rand].yMedio+=10;
                                    if(MedioArray[rand].yMedio > FILA_NAVE -50){
                                            MedioArray[rand].xMedio += 15;
                                    }
                                    else{
                                            MedioArray[rand].xMedio = Nave->xNave;
                                    }
                                    // Verifica si hay colision de la nave con el marciano
                                     if(colision(MedioArray[rand].xMedio, MedioArray[rand].yMedio, Nave->xNave, Nave->yNave)){
                                            MedioArray[rand].xMedio= MAS_ALLA;
                                            MedioArray[rand].yMedio= MAS_ALLA;
                                            MedioArray[rand].visible = false;
                                            matar_nave();
                                            Nave->colisionado = true;
                                    }
                                    }
                                    MedioArray[rand].xMedio = MedioArray[rand].xRespMedio;
                                    MedioArray[rand].yMedio = MedioArray[rand].yRespMedio;
                                    al_unlock_mutex(mutex);
                            }
                            break;
                }
                case 2:             //Para los del medio abajo
            {
                        rand = my_random(0,7);
                        if(MedioBajoArray[rand].visible){
                                    al_lock_mutex(mutex);
                                    MedioBajoArray[rand].xRespMedio = MedioBajoArray[rand].xMedio;
                                    MedioBajoArray[rand].yRespMedio = MedioBajoArray[rand].yMedio;
                                while(MedioBajoArray[rand].yMedio <= ALTO+50){
                                    al_rest(0.100008);
                                    MedioBajoArray[rand].yMedio+=10;
                                    if(MedioBajoArray[rand].yMedio > FILA_NAVE -50){
                                            MedioBajoArray[rand].xMedio += 15;
                                    }
                                    else{
                                            MedioBajoArray[rand].xMedio = Nave->xNave;
                                    }
                                    // Verifica si hay colision de la nave con el marciano
                                    if(colision(MedioBajoArray[rand].xMedio, MedioBajoArray[rand].yMedio, Nave->xNave, Nave->yNave)){
                                            MedioBajoArray[rand].xMedio= MAS_ALLA;
                                            MedioBajoArray[rand].yMedio= MAS_ALLA;
                                            MedioBajoArray[rand].visible = false;
                                            matar_nave();
                                            Nave->colisionado = true;
                                    }
                                    }
                                    MedioBajoArray[rand].xMedio = MedioBajoArray[rand].xRespMedio;
                                    MedioBajoArray[rand].yMedio = MedioBajoArray[rand].yRespMedio;
                                    al_unlock_mutex(mutex);
                            }
                            break;
                }
                case 3:             // Para los de abajo
            {
                        rand = my_random(1,9);  // Excluye al primer marciano
                        if(BajoArray[rand].visible){
                                    al_lock_mutex(mutex);
                                    BajoArray[rand].xRespBajo = BajoArray[rand].xBajo;
                                    BajoArray[rand].yRespBajo = BajoArray[rand].yBajo;
                                while(BajoArray[rand].yBajo <= ALTO+20){
                                    al_rest(0.100008);
                                    BajoArray[rand].yBajo+=10;
                                    if(BajoArray[rand].yBajo > FILA_NAVE -50){
                                            BajoArray[rand].xBajo += 15;
                                    }
                                    else{
                                            BajoArray[rand].xBajo = Nave->xNave;
                                    }
                                    // Verifica si hay colision de la nave con el marciano
                                    if(colision(BajoArray[rand].xBajo, BajoArray[rand].yBajo, Nave->xNave, Nave->yNave)){
                                            BajoArray[rand].xBajo= MAS_ALLA;
                                            BajoArray[rand].xBajo= MAS_ALLA;
                                            BajoArray[rand].visible = false;
                                            matar_nave();
                                            Nave->colisionado = true;
                                    }
                                    }
                                    BajoArray[rand].xBajo = BajoArray[rand].xRespBajo;
                                    BajoArray[rand].yBajo = BajoArray[rand].yRespBajo;
                                    al_unlock_mutex(mutex);
                            }
                            break;
                }
                case 4:             //Para los ultimos marcianos
            {
                        rand = my_random(1,9);  // Excluye al primer marciano
                        if(BajoBajoArray[rand].visible){
                                    al_lock_mutex(mutex);
                                    BajoBajoArray[rand].xRespBajo = BajoBajoArray[rand].xBajo;
                                    BajoBajoArray[rand].yRespBajo = BajoBajoArray[rand].yBajo;
                                while(BajoBajoArray[rand].yBajo <= ALTO+20){
                                    al_rest(0.100008);
                                    BajoBajoArray[rand].yBajo+=10;
                                    if(BajoBajoArray[rand].yBajo > FILA_NAVE -50){
                                            BajoBajoArray[rand].xBajo += 15;
                                    }
                                    else{
                                            BajoBajoArray[rand].xBajo = Nave->xNave;
                                    }
                                    // Verifica si hay colision de la nave con el marciano
                                    if(colision(BajoBajoArray[rand].xBajo, BajoBajoArray[rand].yBajo, Nave->xNave, Nave->yNave)){
                                            BajoBajoArray[rand].xBajo= MAS_ALLA;
                                            BajoBajoArray[rand].yBajo= MAS_ALLA;
                                            BajoBajoArray[rand].visible = false;
                                            matar_nave();
                                            Nave->colisionado = true;
                                    }
                                    }
                                    BajoBajoArray[rand].xBajo = BajoBajoArray[rand].xRespBajo;
                                    BajoBajoArray[rand].yBajo = BajoBajoArray[rand].yRespBajo;
                                    al_unlock_mutex(mutex);
                            }
                            break;
                }
        }
    }
}


// Inicializa los Marcianos
void IniciarMarcianos(){
    int i=0;
    int dist = 60;      // Distancia entre los marcianos

//         BOSS
    int xInicial = COLUMNA_BOSS;    //Coordenadas del Boss mas a la izquierda en la formación fija
    int yInicial = FILA_SUPERIOR;
    for(i; i<CANT_BOSS;i++){ //Inicialización
            BossArray[i].BossImg = al_load_bitmap("img/superior.bmp");
            BossArray[i].xBoss = xInicial;
            BossArray[i].yBoss = yInicial;
            BossArray[i].xRespBoss = xInicial;
            BossArray[i].yRespBoss = yInicial;
            BossArray[i].visible = true;
            xInicial+=dist;
    }

//         MEDIOS
    i=0;
    xInicial = COLUMNA_MEDIA;    //Coordenadas del Boss mas a la izquierda en la formación fija
    yInicial = FILA_MEDIO;
    for(i; i<CANT_MEDIO;i++){ //Inicialización
            MedioArray[i].MedioImg = al_load_bitmap("img/medio.bmp");
            MedioArray[i].xMedio = xInicial;
            MedioArray[i].yMedio = yInicial;
            MedioArray[i].xRespMedio = xInicial;
            MedioArray[i].yRespMedio = yInicial;
            MedioArray[i].visible = true;
            // Medios bajos
            MedioBajoArray[i].MedioImg = al_load_bitmap("img/medio.bmp");
            MedioBajoArray[i].xMedio = xInicial;
            MedioBajoArray[i].yMedio = FILA_MEDIA_BAJA;
            MedioBajoArray[i].xRespMedio = xInicial;
            MedioBajoArray[i].yRespMedio = FILA_MEDIA_BAJA;
            MedioBajoArray[i].visible = true;
            xInicial+=dist;
    }

//         BAJOS
    i=0;
    xInicial = COLUMNA_BAJO;    //Coordenadas del Boss mas a la izquierda en la formación fija
    yInicial = FILA_BAJA;
    for(i; i<CANT_BAJO;i++){ //Inicialización
            BajoArray[i].BajoImg = al_load_bitmap("img/bajo.bmp");
            BajoArray[i].xBajo = xInicial;
            BajoArray[i].yBajo = yInicial;
            BajoArray[i].xRespBajo = xInicial;
            BajoArray[i].yRespBajo = yInicial;
            BajoArray[i].visible = true;
            // Ultimos marcianos
            BajoBajoArray[i].BajoImg = al_load_bitmap("img/bajo.bmp");
            BajoBajoArray[i].xBajo = xInicial;
            BajoBajoArray[i].yBajo = FILA_BAJA_BAJA;
            BajoBajoArray[i].xRespBajo = xInicial;
            BajoBajoArray[i].yRespBajo = FILA_BAJA_BAJA;
            BajoBajoArray[i].visible = true;
            xInicial+=dist;
    }
}

//Una vez creados los marcianos los dibuja en pantalla
// tomando en cuenta si han sido destruidos
void DibujarMarcianos(){

//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((BossArray[i].BossImg), (BossArray[i].xBoss), (BossArray[i].yBoss), 0);
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((MedioArray[i].MedioImg), (MedioArray[i].xMedio), (MedioArray[i].yMedio), 0);
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((MedioBajoArray[i].MedioImg), (MedioBajoArray[i].xMedio), (MedioBajoArray[i].yMedio), 0);
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((BajoArray[i].BajoImg), (BajoArray[i].xBajo), (BajoArray[i].yBajo), 0);
            }

//         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible)    //Dibuja el arreglo si está visible
                al_draw_bitmap((BajoBajoArray[i].BajoImg), (BajoBajoArray[i].xBajo), (BajoBajoArray[i].yBajo), 0);
            }
}


// Movimiento Derecho de los marcianos
void DerechaMarcianos(){


if(BajoArray[9].xBajo >  620 ){       // Límite para dejar de bajar antes de atacar
    DibujarMarcianos();
    return;
    }

//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].xBoss += TAM_MOVIMIENTO;
                BossArray[i].xRespBoss += TAM_MOVIMIENTO;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].xMedio += TAM_MOVIMIENTO;
                MedioArray[i].xRespMedio+= TAM_MOVIMIENTO;
                }
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible){    //Dibuja el arreglo si está visible
                MedioBajoArray[i].xMedio += TAM_MOVIMIENTO;
                MedioBajoArray[i].xRespMedio+= TAM_MOVIMIENTO;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].xBajo += TAM_MOVIMIENTO;
                BajoArray[i].xRespBajo+= TAM_MOVIMIENTO;
                }
            }

            //         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoBajoArray[i].xBajo += TAM_MOVIMIENTO;
                BajoBajoArray[i].xRespBajo+= TAM_MOVIMIENTO;
                }
            }
}


// Movimiento Izquierdo de los marcianos
void IzquierdaMarcianos(){

if(BajoArray[0].xBajo < 50 ){       // Límite para dejar de bajar antes de atacar
    DibujarMarcianos();
    return;
    }

//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].xBoss -= TAM_MOVIMIENTO;
                BossArray[i].xRespBoss -= TAM_MOVIMIENTO;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].xMedio -= TAM_MOVIMIENTO;
                MedioArray[i].xRespMedio-= TAM_MOVIMIENTO;
                }
            }

//         MEDIO BAJO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioBajoArray[i].visible){    //Dibuja el arreglo si está visible
                MedioBajoArray[i].xMedio -= TAM_MOVIMIENTO;
                MedioBajoArray[i].xRespMedio-= TAM_MOVIMIENTO;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].xBajo -= TAM_MOVIMIENTO;
                BajoArray[i].xRespBajo -= TAM_MOVIMIENTO;
                }
            }

            //         BAJO BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoBajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoBajoArray[i].xBajo -= TAM_MOVIMIENTO;
                BajoBajoArray[i].xRespBajo -= TAM_MOVIMIENTO;
                }
            }
}

// Movimiento Arriba de los marcianos
void ArribaMarcianos(){


if(BossArray[0].yBoss < 50){       // Límite para dejar de subir
    DibujarMarcianos();
    return;
    }
//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].yBoss -= TAM_MOVIMIENTO;
                BossArray[i].yRespBoss -= TAM_MOVIMIENTO;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].yMedio -= TAM_MOVIMIENTO;
                MedioArray[i].yRespMedio -= TAM_MOVIMIENTO;
                MedioBajoArray[i].yMedio -= TAM_MOVIMIENTO;
                MedioBajoArray[i].yRespMedio -= TAM_MOVIMIENTO;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].yBajo -= TAM_MOVIMIENTO;
                BajoArray[i].yRespBajo-= TAM_MOVIMIENTO;
                BajoBajoArray[i].yBajo -= TAM_MOVIMIENTO;
                BajoBajoArray[i].yRespBajo -= TAM_MOVIMIENTO;
                }
            }
}


// Movimiento Arriba de los marcianos
void AbajoMarcianos(){


if(BajoArray[0].yBajo > 200 ){       // Límite para dejar de bajar antes de atacar
    DibujarMarcianos();
    return;
    }
//         BOSS
    int i=0;
    for(i; i<CANT_BOSS;i++) {//Puesta de los boss en pantalla
            if(BossArray[i].visible){    //Dibuja el arreglo si está visible
                BossArray[i].yBoss += TAM_MOVIMIENTO;
                BossArray[i].yRespBoss += TAM_MOVIMIENTO;
                }
            }

//         MEDIO
    i=0;
    for(i; i<CANT_MEDIO;i++) {//Puesta de los boss en pantalla
            if(MedioArray[i].visible){    //Dibuja el arreglo si está visible
                MedioArray[i].yMedio += TAM_MOVIMIENTO;
                MedioArray[i].yRespMedio += TAM_MOVIMIENTO;
                MedioBajoArray[i].yMedio += TAM_MOVIMIENTO;
                MedioBajoArray[i].yRespMedio += TAM_MOVIMIENTO;
                }
            }

//         BAJO
    i=0;
    for(i; i<CANT_BAJO;i++) {//Puesta de los boss en pantalla
            if(BajoArray[i].visible) {   //Dibuja el arreglo si está visible
                BajoArray[i].yBajo += TAM_MOVIMIENTO;
                BajoArray[i].yRespBajo+= TAM_MOVIMIENTO;
                BajoBajoArray[i].yBajo += TAM_MOVIMIENTO;
                BajoBajoArray[i].yRespBajo+= TAM_MOVIMIENTO;
                }
            }
}




//Dibuja la nave, segun las posiciones que tenga en el momento de llamar al metodo
void dibujarNave( DatosGlobales * datos){
    al_draw_bitmap((Nave->Imagen_Nave), (Nave->xNave), (Nave->yNave), 0);
    al_draw_bitmap((Vida1->Imagen_Nave), (Vida1->xNave), (Vida1->yNave), 0);
    al_draw_bitmap((Vida2->Imagen_Nave), (Vida2->xNave), (Vida2->yNave), 0);
    if(Nave->colisionado){
                al_draw_bitmap(explosion, (Nave->xNave-15), (Nave->yNave-15), 0);
                al_play_sample(sonido_explosion,1,0,1,ALLEGRO_PLAYMODE_ONCE,0);
                DibujarMarcianos();
                al_flip_display();
                al_rest(0.8);
                al_draw_bitmap(explosion, MAS_ALLA, MAS_ALLA, 0);
                Nave->colisionado = false;
    }
    }

void dibujarBala(){
                    al_draw_bitmap(Bala->Bala, Bala->xBala, Bala->yBala,0);
    }

/***
 *    ██╗███╗    ██╗██╗ ██████╗ ██╗   ██████╗
 *    ██║████╗   ██║██║ ██╔════╝██║ ██╔═══██╗
 *    ██║██╔██╗  ██║██║ ██║        ██║ ██║    ██║
 *    ██║██║╚██╗ ██║██║ ██║        ██║ ██║    ██║
 *    ██║██║ ╚████ ║██║ ╚██████╗ ██║╚██████╔╝
 *    ╚═╝╚═╝  ╚═══╝  ╚═╝     ╚═════╝╚═╝       ╚═════╝
 *
 */

 //Proceimiento que inicia el juego
int iniciarJuego()
{

    bool hubo_disparo;
//Iniciar allegro
    al_init();
    al_init_image_addon(); // Para cargar los bmps

    al_install_audio();         // Carga los sonidos
    al_init_acodec_addon();

    timer = al_create_timer(1.0 / FPS);
   if(!timer) {
      printf("Error en el timer");
      return -1;
   }




if (!al_install_keyboard()) {
        printf("Error en el teclado.");
        return -1;
 }
    Pantalla = al_create_display(ANCHO, ALTO);
    al_set_window_title(Pantalla, "Galaga");

    EventQueue = al_create_event_queue();
    if(!EventQueue){
        printf("Error en la cola de eventos.");
        return -1;
    }
    al_register_event_source(EventQueue, al_get_keyboard_event_source());
    al_register_event_source(EventQueue, al_get_display_event_source(Pantalla));
    al_register_event_source(EventQueue, al_get_timer_event_source(timer));

    // Inicialización de la nave
    Nave = malloc(sizeof(Nave));
    Nave->Imagen_Nave = al_load_bitmap("img/plane.bmp");
    Nave->xNave=COLUMNA_NAVE;
    Nave->yNave=FILA_NAVE;
    Nave->colisionado = false;

    // Puesto en pantalla de las imágenes de las vidas
    Vida1 = malloc(sizeof(Nave));
    Vida1->Imagen_Nave = al_load_bitmap("img/vida.bmp");
    Vida1->xNave=COLUMNA_VIDAS;
    Vida1->yNave=FILA_VIDAS;

    Vida2 = malloc(sizeof(Nave));
    Vida2->Imagen_Nave = al_load_bitmap("img/vida.bmp");
    Vida2->xNave=COLUMNA_VIDAS + 20;
    Vida2->yNave=FILA_VIDAS;

// Inicialización de la bala
Bala = malloc(sizeof(Bala));
Bala->Bala = al_load_bitmap("img/Bala.bmp");
Bala->xBala = MAS_ALLA;
Bala->yBala = MAS_ALLA;
Bala->disparada = false;

    //Inicialización de los parámetros
    JuegoDatos = malloc(sizeof (DatosGlobales)); //Datos globales del juego
    JuegoDatos->BG = al_load_bitmap("img/nube.bmp");
    JuegoDatos->jugando = true;
    JuegoDatos->fin_del_juego = false;
    JuegoDatos->cantidad_vidas = 2;
    redibujar = true;



// Sonidos del juego
    al_set_window_title(Pantalla,"Iniciando Sonido");
    al_reserve_samples(10);
    song = al_load_sample("snd/fondo.ogg");
    shot = al_load_sample("snd/disparo.wav");
    sonido_explosion = al_load_sample("snd/explosion.wav");
    songInstance= al_create_sample_instance(song);
    al_set_sample_instance_playmode(songInstance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(songInstance, al_get_default_mixer());

    al_set_window_title(Pantalla,"Galaga");


    dibujarNave(JuegoDatos);

    IniciarMarcianos();

    DibujarMarcianos();


// Creación del hilo para dibujar
    mutex = al_create_mutex();
    hilo_animacion = al_create_thread(anim_marcianos, JuegoDatos);
    al_start_thread(hilo_animacion);

// Hilo que hace que los marcianos ataquen
    hilo_ataque = al_create_thread(ataque_marcianos,JuegoDatos);
    al_start_thread(hilo_ataque);

// Hilo que espera por un disparo
hilo_bala = al_create_thread(espera_balas,JuegoDatos);
al_start_thread(hilo_bala);

    al_start_timer(timer);


    // Dibuja la explosion

    explosion = al_load_bitmap("img/explosion.bmp");

// Ciclo principal del juego
    while(JuegoDatos->jugando && !Nave->colisionado)
    {

        al_wait_for_event(EventQueue, &Event);
        if(Event.type == ALLEGRO_EVENT_TIMER)
            redibujar = true;
        else if(Event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            JuegoDatos->jugando = false;


//     Eventos del teclado
                    do{
                                     switch(Event.keyboard.keycode)
                                {
                                case ALLEGRO_KEY_LEFT:
                                    if(Nave->xNave > MIN_VENTANA){
                                        Nave->xNave-=TAM_MOVIMIENTO;
                                        redibujar = true;
                                    }
                                    break;
                                case ALLEGRO_KEY_RIGHT:
                                    if(Nave->xNave < MAX_VENTANA){
                                        Nave->xNave+=TAM_MOVIMIENTO;
                                        redibujar = true;
                                        }
                                    break;

                                case ALLEGRO_KEY_SPACE:
                                    al_play_sample(shot,1,0,1,ALLEGRO_PLAYMODE_ONCE,0);
                                    Bala->disparada = true;
                                    break;
                            }
                    } while(Event.type == ALLEGRO_KEY_DOWN);

//     Redibujar
		if(JuegoDatos->jugando){
            if(JuegoDatos->fin_del_juego){  // Protocolo del final del juego
                                al_draw_bitmap(explosion, (Nave->xNave-15), (Nave->yNave-15), 0);
                                al_play_sample(sonido_explosion,1,0,1,ALLEGRO_PLAYMODE_ONCE,0);
                                al_flip_display();
                                al_rest(0.8);
                                break;
                        }
            if(redibujar && al_is_event_queue_empty(EventQueue)){
                    al_draw_bitmap(JuegoDatos->BG, 0, 0, 0);
                    dibujarNave(JuegoDatos);
                    DibujarMarcianos(JuegoDatos);
                    dibujarBala();
                    al_flip_display();
                    al_play_sample_instance(songInstance);      //Reproducir la música
                    redibujar = false;
            }
		}

    }

    al_destroy_sample(song);
    al_destroy_sample(shot);
    al_destroy_sample_instance(songInstance);
    al_destroy_timer(timer);
    al_destroy_event_queue(EventQueue);
    al_destroy_display(Pantalla);
    return 0;
}
