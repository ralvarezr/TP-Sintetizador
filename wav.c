#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "wav.h"



//MÁSCARAS
#define MASK_BYTE_0 0x000000ff
#define MASK_BYTE_1 0x0000ff00
#define MASK_BYTE_2 0x00ff0000
#define MASK_BYTE_3 0xff000000


#define MAX_CHAR    4

//Escribe un dato uint32 en formato little endian en el archivo.
bool escribir_uint32_little_endian(FILE *f, uint32_t *dato){

	uint8_t v[4];

	v[0] = (*dato & MASK_BYTE_0);
	v[1] = (*dato & MASK_BYTE_1) >> 8;
	v[2] = (*dato & MASK_BYTE_2) >> 16;
 	v[3] = (*dato & MASK_BYTE_3) >> 24;
 	
 	for (size_t i = 0; i < 4; ++i)
 	{
 		if(fwrite(&(v[i]), sizeof(uint8_t), 1, f) != 1)
 			return false;
 	}
    
    return true;
}

//Escribe un dato uint16 en formato little endian en el archivo.
bool escribir_uint16_little_endian(FILE *f, uint16_t *dato){

	uint8_t v[2];

	v[0] = (*dato & MASK_BYTE_0);
	v[1] = (*dato & MASK_BYTE_1) >> 8;
 	
 	for (size_t i = 0; i < 2; ++i)
 	{
 		if(fwrite(&(v[i]), sizeof(uint8_t), 1, f) != 1)
 			return false;
 	}
    
    return true;
}

//Escribe un dato int16 en formato little endian en el archivo.
bool escribir_int16_little_endian(FILE *f, int16_t *dato){

	int8_t v[2];

	v[0] = (*dato & MASK_BYTE_0);
	v[1] = (*dato & MASK_BYTE_1) >> 8;
 	
 	for (size_t i = 0; i < 2; ++i)
 	{
 		if(fwrite(&(v[i]), sizeof(int8_t), 1, f) != 1)
 			return false;
 	}
    
    return true;
}

//Escribe una n cantidad de caracteres en formato little endian en el archivo.
bool escribir_chars_little_endian(FILE *f, char *data, size_t n){
 	
 	for (size_t i = 0; i < n; ++i)
 	{
 		if(fwrite(&(data[i]), sizeof(char), 1, f) != 1)
 			return false;
 	}
    
    return true;
}

//Escribe el archivo WAV.
bool escribir_wav(FILE *f, tramo_t *tramo){

    char chunkid[MAX_CHAR] = "RIFF";
      if(!escribir_chars_little_endian(f, chunkid, MAX_CHAR))
        return false;

    size_t n = tramo_obtener_tamagno(tramo);
    uint32_t chunksize = 36 + 2*n;
    if(!escribir_uint32_little_endian(f, &chunksize))
        return false;

    char format[MAX_CHAR] = "WAVE";
    if(!escribir_chars_little_endian(f, format, MAX_CHAR))
        return false;

    char subchunk1id[MAX_CHAR] = "fmt ";
    if(!escribir_chars_little_endian(f, subchunk1id, MAX_CHAR))
        return false;


    uint32_t subchunk1size = 16;
    if(!escribir_uint32_little_endian(f, &subchunk1size))
        return false;


    uint16_t audioformat = 1;
    if(!escribir_uint16_little_endian(f, &audioformat))
        return false;

    uint16_t numchannels = 1;
    if(!escribir_uint16_little_endian(f, &numchannels))
        return false;

    uint32_t samplerate = tramo_obtener_frecuencia(tramo);
    if(!escribir_uint32_little_endian(f, &samplerate))
        return false;

    uint32_t byterate = 2*samplerate;
    if(!escribir_uint32_little_endian(f, &byterate))
        return false;

    uint16_t blockalign = 2;
    if(!escribir_uint16_little_endian(f, &blockalign))
        return false;

    uint16_t bitpersample = 16;
    if(!escribir_uint16_little_endian(f, &bitpersample))
       return false;

    char subchunk2id[MAX_CHAR] = "data";
    if(!escribir_chars_little_endian(f, subchunk2id, MAX_CHAR))
        return false;


    uint32_t subchunk2size = 2*n;                    
    if(!escribir_uint32_little_endian(f, &subchunk2size))
        return false;


    int16_t samples = 0;
    float muestra_maxima =  tramo_obtener_muestra_maxima(tramo);

    for (size_t i = 0; i < n; i++)
    {
        samples = tramo_normalizar_muestra(tramo, i, muestra_maxima);
        if(!escribir_int16_little_endian(f, &samples))
            return false;
   
    }
    
    return true;

}