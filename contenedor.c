#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>



#include "contenedor.h"

/* Estructura del contenedor de notas. Posee un vector dinámico de notas y un tamaño n. 
   Si el tamaño es 0, el vector apunta a NULL.
*/
struct contenedor{
	nota_t *notas;				//Vector dinamico de notas.
	size_t n;					//Tamaño del vector. 
};


//Crea y devuelve un contenedor vacío. En caso de error devuelve NULL.
contenedor_t *contenedor_crear_vacio(){

	contenedor_t *contenedor = malloc(sizeof(contenedor_t));
    if(contenedor == NULL)
        return NULL;

    contenedor->notas = NULL;

    contenedor->n = 0;

    return contenedor;

}

//Agrega una nota al cotenedor. En caso de fallo devuelve false.
bool contenedor_agregar_nota(contenedor_t *contenedor, nota_t *nota_nueva){

	nota_t *aux = realloc(contenedor->notas, (contenedor->n + 1) * sizeof(nota_t));
	if(aux == NULL)
		return false;

	contenedor->notas = aux;

	contenedor->notas[contenedor->n] = *nota_nueva;

	contenedor->n++;
	
	return true;

}

//Devuelve la posicion donde se encuentra la nota en el contenedor.
static size_t _contenedor_obtener_posicion_nota(contenedor_t *contenedor, enum_nota_t *nota, signed char *octava){

	size_t n = 0;

	for(;n < contenedor->n; n++){

		if(encontrar_nota_apagada(&contenedor->notas[n], nota, octava)){
			break;
		}

	}
	
	return n;

}

//Agrega la duración a una nota que se encuentra en el contenedor.
void contenedor_agregar_duracion_nota(contenedor_t *contenedor, enum_nota_t *nota, signed char *octava, uint32_t tf, int pps){

	size_t n = _contenedor_obtener_posicion_nota(contenedor, nota, octava);
	nota_agregar_duracion(&contenedor->notas[n], tf, pps);

}



//Destruye el contenedor.
void contenedor_destruir(contenedor_t *contenedor){

	destruir_nota(contenedor->notas);
	free(contenedor);

}

//Devuelve el tamaño del contenedor.
size_t contenedor_obtener_tamagno(contenedor_t *contenedor){
	return contenedor->n;
}


//Devuelve la nota en la posición n del contenedor.
nota_t *contenedor_obtener_nota(contenedor_t *contenedor, size_t n){
	return &contenedor->notas[n];
}

//Devuelve el tiempo de inicio de la primera nota del contenedor.
double contenedor_obtener_inicio_primera_nota(contenedor_t *contenedor){
	
	nota_t *primera = contenedor_obtener_nota(contenedor, 0);

	return obtener_inicio_nota(primera);

}
