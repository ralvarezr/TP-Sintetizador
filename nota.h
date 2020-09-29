#ifndef NOTA_H
#define NOTA_H

#include <stdint.h>
#include <stdbool.h>

//Tipo enumerativo para las 12 notas de la escala musical.
typedef enum{

	DO,
	DO_S,
	RE,
	RE_S,
	MI,
	FA,
	FA_S,
	SOL,
	SOL_S,
	LA,
	LA_S,
	SI

}enum_nota_t;

/* 
   Estructura de una nota. Contiene su nombre, tiempo de inicio, tiempo de duracion,
   su octava, su intensidad y un flag que indica si la nota está apagada o encendida.
*/
typedef struct{

	enum_nota_t nombre;			//Indica el nombre de la nota.
	double inicio;				//Indica el instante de tiempo en el cual la nota inicia.
	double duracion;			//Indica la duracion de la nota.
	signed char octava;			//Indica la octava de la nota.
	float intensidad;			//Indica la intesidad de la nota.
	bool apagada;				//Indica si la nota esta encendida o apagada.

}nota_t;


//Crea y devuelve una nota.
nota_t *crear_nota(enum_nota_t *nota, uint32_t tiempo, int pps, float *intensidad, signed char *octava);

//Destruye una nota.
void destruir_nota(nota_t *nota);

//Devuelve si las notas coinciden y se encuentra apagada.
bool encontrar_nota_apagada(nota_t *nota, enum_nota_t *nombre, signed char *octava);

//Agrega el tiempo de duración de una nota.
void nota_agregar_duracion(nota_t *nota, uint32_t tiempo, int pps);

//Devuelve la frecuencia de una nota.
double obtener_frecuencia_nota(nota_t *nota);

//Devuelve la intensidad de una nota.
float obtener_intensidad_nota(nota_t *nota);

//Devuelve el tiempo de inicio de una nota.
double obtener_inicio_nota(nota_t *nota);

//Devuelve la duración de una nota.
double obtener_duracion_nota(nota_t *nota);


#endif