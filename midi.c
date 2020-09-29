#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#include "midi.h"
#include "nota.h"


#define CANTIDAD_FORMATO	3
#define CANTIDAD_NOTAS		128

#define MASK_EVENT_ON 0x80  // 1000 0000
#define MASK_EVENT    0x70  // 0111 0000
#define MASK_CHANNEL  0x0F  // 0000 1111

#define SHIFT_EVENT   4
#define SHIFT_CHANNEL 0

#define HEADER_ID 0x4D546864
#define HEADER_SIZE 6

#define TRACK_ID 0x4D54726B

#define MASK_TIEMPO 0x7F // 0111 1111

#define LONG_MAX_TIEMPO 4

#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10


//Enumeativo de la posicion la nota y su velocidad en el mensaje leido.
enum {EVNOTA_NOTA, EVNOTA_VELOCIDAD};

//Enumerativo de la posicion del tipo y longitud de metaevento en el mensaje leido.
enum {METAEVENTO_TIPO, METAEVENTO_LONGITUD};


//Tipo enumerativo de los diferentes formatos.
typedef enum{

	PISTA_UNICA,
	MULTIPISTAS_SINCRONICAS,
	MULTIPISTAS_ASINCRONICAS,

}formato_t;

//Tipo enumerativo de los diferentes eventos.
typedef enum{
	
	NOTA_APAGADA,
	NOTA_ENCENDIDA,
	NOTA_DURANTE,
	CAMBIO_DE_CONTROL,
	CAMBIO_DE_PROGRAMA,
	VARIAR_CANAL,
	CAMBIO_DE_PITCH,
	METAEVENTO

}evento_t;

//Diccionario de la longitud de los eventos.
static const int longitud_eventos[] = {

	[NOTA_APAGADA] =  2,
	[NOTA_ENCENDIDA] = 2,
	[NOTA_DURANTE] = 2,
	[CAMBIO_DE_CONTROL] = 2,
	[CAMBIO_DE_PROGRAMA] = 1,
	[VARIAR_CANAL] = 1,
	[CAMBIO_DE_PITCH] = 2,
	[METAEVENTO] = 2

};


//Decodifica el formato y lo almacena.
static bool decodificar_formato(uint16_t valor, formato_t *formato){

    if(valor >= CANTIDAD_FORMATO) 
        return false;
    
    *formato = valor;
	return true;
}

//Decodifica un evento y almacena el tipo de evento, el canal y la longitud.
static bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud){ 

	if(!(valor & MASK_EVENT_ON))
		return false;

	*evento = (valor & MASK_EVENT) >> SHIFT_EVENT;
	*canal = (valor & MASK_CHANNEL) >> SHIFT_CHANNEL;
	*longitud = longitud_eventos[*evento];
	return true;
	
}

//Decodifica una nota en su nombre y ocatava.
static bool decodificar_nota(uint8_t valor, enum_nota_t *nota, signed char *octava){

	if(valor & MASK_EVENT_ON)
		return false;

	*nota = valor%12;
	*octava = (valor/12) - 1;
	return true;
		
}

//Lee un uint8_t en formato big endian.
static uint8_t leer_uint8_t(FILE *f){
	
	uint8_t v[1];
	fread(v, sizeof(uint8_t), 1, f);

	return v[0];
}

//Lee un uint16_t en formato big endian.
static uint16_t leer_uint16_big_endian(FILE *f){

	uint8_t v[2];
	fread(v, sizeof(uint8_t), 2, f);

	return v[0] << 8 | v[1];
}

//Lee un uint32_t en formato big endian.
static uint32_t leer_uint32_big_endian(FILE *f){

	uint8_t v[4];
	fread(v, sizeof(uint8_t), 4, f);

	return v[0] << 24 | v[1] << 16 | v[2] << 8 | v[3];
}

//Lee el encabezado y almacena el formato, el numero de pistas y los pulsos por negra.
static bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulsos_negra){

	uint32_t aux_32;
	uint16_t aux_16;

	aux_32 = leer_uint32_big_endian(f);
	if(aux_32 != HEADER_ID)
		return false;

	aux_32 = leer_uint32_big_endian(f);
	if(aux_32 != HEADER_SIZE)
		return false;

	aux_16 = leer_uint16_big_endian(f);
	if(!decodificar_formato(aux_16, formato)){
		return false;
	}

	*numero_pistas = leer_uint16_big_endian(f);
	*pulsos_negra = leer_uint16_big_endian(f);

	return true;
}

//Lee la pista y almacena el tamaño.
static bool leer_pista(FILE *f, uint32_t *tamagno){

	uint32_t aux_32;

	aux_32 = leer_uint32_big_endian(f);
	if(aux_32 != TRACK_ID)
		return false;

	*tamagno = leer_uint32_big_endian(f);

	return true;
}

//Lee el tiempo y lo almacena.
static bool leer_tiempo(FILE *f, uint32_t *tiempo){  

	uint8_t aux[LONG_MAX_TIEMPO];

	aux[0] = leer_uint8_t(f);
	if(!(aux[0] & ~MASK_TIEMPO)){ //Si es el ultimo.
		*tiempo = aux[0];
		return true;
	} 

	aux[1] = leer_uint8_t(f);
	if(!(aux[1] & ~MASK_TIEMPO)){ //Si es el ultimo.
		*tiempo = (aux[0] & MASK_TIEMPO) << 7 | aux[1];
		return true;
	} 

	aux[2] = leer_uint8_t(f);
	if(!(aux[2] & ~MASK_TIEMPO)){ //Si es el ultimo.
		*tiempo = (aux[0] & MASK_TIEMPO) << 14 | (aux[1] & MASK_TIEMPO) << 7 | aux[2];
		return true;
	} 

	aux[3] = leer_uint8_t(f);
	if(aux[3] & ~MASK_TIEMPO){ //Si no es el ultimo.
		return false;
	} 

	*tiempo = (aux[0] & MASK_TIEMPO) << 21 | (aux[1] & MASK_TIEMPO) << 14 | (aux[2] & MASK_TIEMPO) << 7 | aux[3];
		return true;

}

//Lee un evento y almacena el evento, el canal, el mensaje y su longitud.
static bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]){

	uint8_t aux = leer_uint8_t(f);
	if(decodificar_evento(aux, evento, canal, longitud)){
		for(int i = 0; i < *longitud; i++){
			mensaje[i] = leer_uint8_t(f);
		}

		return true;
	}

	mensaje[0] = aux;
	for(int i = 1; i < *longitud; i++){
			mensaje[i] = leer_uint8_t(f);
	}

	return true;
}

//Descarta un metaevento.
static void descartar_metaevento(FILE *f, uint8_t tamagno){

	for(uint8_t i = 0; i < tamagno; i++){
		leer_uint8_t(f);
	}
}

//Lee un archivo MIDI y llena el contenedor con las notas encontrada en el canal.
bool leer_midi(FILE *f, contenedor_t *contenedor, signed char *canal, int pps){


    // LECTURA DEL ENCABEZADO:
    formato_t formato;
    uint16_t numero_pistas;
    uint16_t pulsos_negra;    

    if(! leer_encabezado(f, &formato, &numero_pistas, &pulsos_negra))
        return false;
    
    // ITERAMOS LAS PISTAS:
    bool canal_encontrado = false;

    for(uint16_t pista = 0; pista < numero_pistas; pista++) {
        // LECTURA ENCABEZADO DE LA PISTA:
        uint32_t tamagno_pista;
        if(! leer_pista(f, &tamagno_pista))
            return false;
        
        evento_t evento;
        char canal_aux;
        int longitud;
        uint32_t tiempo = 0;

        // ITERAMOS LOS EVENTOS:
        while(1) {
            uint32_t delta_tiempo;
            leer_tiempo(f, &delta_tiempo);
            tiempo += delta_tiempo;

            // LECTURA DEL EVENTO:
            uint8_t buffer[EVENTO_MAX_LONG];
            if(! leer_evento(f, &evento, &canal_aux, &longitud, buffer)) {
                return false;
            }

            // PROCESAMOS EL EVENTO:
            if(evento == METAEVENTO && canal_aux == 0xF) {
                // METAEVENTO:
                if(buffer[METAEVENTO_TIPO] == METAEVENTO_FIN_DE_PISTA) { 
                    break;
                }

                descartar_metaevento(f, buffer[METAEVENTO_LONGITUD]);
            }
            else if (canal_aux == *canal) {

                canal_encontrado =  true;	//Si se encuentra el canal, cambio el flag.

                // NOTA:
                enum_nota_t nota;
                signed char octava;
                
                
                if(! decodificar_nota(buffer[EVNOTA_NOTA], &nota, &octava)) {
                    return false;
                }

                if(evento == NOTA_ENCENDIDA &&  buffer[EVNOTA_VELOCIDAD] != 0){

                    float intensidad = buffer[EVNOTA_VELOCIDAD];

                    nota_t *nota_nueva = crear_nota(&nota, tiempo, pps, &intensidad, &octava);
                    if(nota_nueva == NULL)
                    	return false;

                   if(! contenedor_agregar_nota(contenedor, nota_nueva)){ 
                    	destruir_nota(nota_nueva);
                        return false; 
                    }

                    destruir_nota(nota_nueva);
                    

                }
                
                if((evento == NOTA_ENCENDIDA &&  buffer[EVNOTA_VELOCIDAD] == 0) || (evento == NOTA_APAGADA)){
                	contenedor_agregar_duracion_nota(contenedor, &nota, &octava, tiempo, pps);
                       
                } 

                    
                
            }
            
        }

    }


    return canal_encontrado;
}