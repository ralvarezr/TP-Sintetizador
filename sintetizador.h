#ifndef SINTETIZADOR_H
#define SINTETIZADOR_H

#include <stdbool.h>
#include <stdio.h>


//Declaración del puntero a función de las funciones de modulación.
typedef float (*modulacion_t)(double, float [3]);

#include "nota.h"
#include "contenedor.h"
#include "tramo.h"

#define MAX_STR	 100


//Declaración de la estructura del sintetizado.
typedef struct synth sintetizador_t;


//Crea un sintetzador vacio.
sintetizador_t *sintetizador_crear_vacio();

//Destruye el sintetizador.
void sintetizador_destruir(sintetizador_t *sintetizador);

//Sintetiza una nota y devuelve el tramo en el cual se sintetizó la nota.
tramo_t *sintetizar_nota(nota_t *nota, int f_m, sintetizador_t *sintetizador);

//Sintetiza las notas de un contenedor y las almacena en el tramo.
bool sintetizar_notas(contenedor_t *contenedor, tramo_t *tramo, int f_m, sintetizador_t *sintetizador);

//Lee el archivo de texto del sintetizador y llena el sintetizador con los parámetros encontrados en el archivo.
bool leer_sintetizador(FILE *f, sintetizador_t *sintetizador);



#endif