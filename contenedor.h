#ifndef CONTENEDOR_H
#define CONTENEDOR_H

#include <stdbool.h>
#include <stdint.h>

#include "nota.h"

//Declaración de la estructura del contenedor.
typedef struct contenedor contenedor_t;


//Crear un contenedor vacío.
contenedor_t *contenedor_crear_vacio();

//Agregar una nota al contenedor.
bool contenedor_agregar_nota(contenedor_t *contenedor, nota_t *nota_nueva);

//Agrega la duracion a una nota ya almacenada en el contenedor.
void contenedor_agregar_duracion_nota(contenedor_t *contenedor, enum_nota_t *nota, signed char *octava, uint32_t tf, int pps);

//Destruye el contenedor.
void contenedor_destruir(contenedor_t *contenedor);

//Devuelve el tamaño del contenedor.
size_t contenedor_obtener_tamagno(contenedor_t *contenedor);

//Devuelve la nota en la posición n del contenedor.
nota_t *contenedor_obtener_nota(contenedor_t *contenedor, size_t n);

//Devuelve el tiempo de inicio de la primera nota del contenedor.
double contenedor_obtener_inicio_primera_nota(contenedor_t *contenedor);


#endif