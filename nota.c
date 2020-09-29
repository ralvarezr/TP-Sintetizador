#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "nota.h"

#define FREC_REF 440	//FRECUENCIA DE REFERENCIA
#define NOTA_REF LA 	//NOTA DE REFERENCIA
#define OCT_REF  4		//OCTAVA DE REFERENCIA

//Crea una nota y la devuelve inicializada con los parámetros pasados a la función y pone el flag apagada en falso.
nota_t *crear_nota(enum_nota_t *nota, uint32_t tiempo, int pps, float *intensidad, signed char *octava){

	nota_t *nota_nueva = malloc(sizeof(nota_t));
	if(nota_nueva == NULL)
		return NULL;

	nota_nueva->nombre = *nota;
	nota_nueva->inicio = (double)tiempo/pps;
	nota_nueva->intensidad = *intensidad;
	nota_nueva->octava = *octava;
	nota_nueva->apagada = false;

	return nota_nueva;
}

//Destruye una nota.
void destruir_nota(nota_t *nota){
	free(nota);
}


//Devuelve si las notas coinciden y se encuentra apagada.
bool encontrar_nota_apagada(nota_t *nota, enum_nota_t *nombre, signed char *octava){

	return (nota->nombre == *nombre && nota->octava == *octava && nota->apagada == false);
}

//Agrega el tiempo de duración de la nota y enciende el flag de apagada.
void nota_agregar_duracion(nota_t *nota, uint32_t tiempo, int pps){

	nota->duracion = (double)tiempo/pps - nota->inicio;
	nota->apagada = true;
}

//Devuelve la frecuencia de la nota. Obtenida mediante un cálculo que depende de una nota y octava de referencia.
double obtener_frecuencia_nota(nota_t *nota){
	int o = nota->octava - OCT_REF; 
	int s =  nota->nombre - NOTA_REF;

	double exponente = o + (double)s/12;

	return FREC_REF*pow(2,exponente);

}

//Devuelve la intensidad de la nota.
float obtener_intensidad_nota(nota_t *nota){
	return nota->intensidad;
}

//Devuelve el tiempo de inicio de la nota.
double obtener_inicio_nota(nota_t *nota){
	return nota->inicio;
}

//Devuelve el tiempo de duración de la nota.
double obtener_duracion_nota(nota_t *nota){
	return nota->duracion;
}
