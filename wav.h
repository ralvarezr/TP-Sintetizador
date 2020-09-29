#ifndef WAV_H
#define WAV_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "tramo.h"

//Escribe un dato uint32 en formato little endian en el archivo.
bool escribir_uint32_little_endian(FILE *f, uint32_t *dato);

//Escribe un dato uint16 en formato little endian en el archivo.
bool escribir_uint16_little_endian(FILE *f, uint16_t *dato);

//Escribe un dato int16 en formato little endian en el archivo.
bool escribir_int16_little_endian(FILE *f, int16_t *dato);

//Escribe una n cantidad de caracteres en formato little endian en el archivo.
bool escribir_chars_little_endian(FILE *f, char *data, size_t n);

//Escribe el archivo WAV.
bool escribir_wav(FILE *f, tramo_t *tramo);


#endif