#ifndef TRAMO_H
#define TRAMO_H

#include <stdbool.h>
#include <stdint.h>

//Declaro la estructura del tramo.
typedef struct s_tramo tramo_t;


#include "sintetizador.h"


//Destruye un tramo.
void tramo_destruir(tramo_t *t);

//Crear un tramo y muestrea los armónicos.
tramo_t *tramo_crear_muestreo(double t0, double tf, int f_m, float f, float a, float fa[][2], size_t n_fa);

//Suma dos tramos.
bool tramo_extender(tramo_t *destino, const tramo_t *extension);

//Devuelve el tamaño del tramo.
size_t tramo_obtener_tamagno(tramo_t *tramo);

//Devuelve la muestra máxima del tramo.
float tramo_obtener_muestra_maxima(tramo_t *tramo);

//Normaliza la muestra a int16.
int16_t tramo_normalizar_muestra(tramo_t *tramo, size_t pos, float amplitud_maxima);

//Crea y devuelve un tramo base.
tramo_t *tramo_crear_base(double t0, double tf, int f_m); 

//Modula un tramo.
void modular_tramo_sintetizado(tramo_t *tramo, double duracion_nota, double t_ataque, double t_decaimiento, float params[][3], modulacion_t funcion[3]);

int tramo_obtener_frecuencia(tramo_t *tramo);

#endif