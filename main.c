#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "tramo.h"
#include "nota.h"
#include "midi.h"
#include "contenedor.h"
#include "sintetizador.h"
#include "wav.h"


#define MAX_STR  100

//VALORES PREDETERMINADOS.
#define FRECUENCIA_PREDET 44100
#define CANAL_PREDET 0
#define PPS_PREDET 60

//Imprime el mensaje de uso del programa.
void uso(const char *argv[]);

//Procesa los parámetros de entrada y los almacena en las variables correspondientes.
bool procesar_parametros(size_t n, char const *argv[], char nombre_synth[], char nombre_mid[], char nombre_wav[], signed char *canal, int *frecuencia, int *pulsosporsegundo);



int main(int argc, char const *argv[])
{
    
    char nombre_synth[MAX_STR];     //Nombre del archivo del Sintetizador.
    char nombre_mid[MAX_STR];       //Nombre del archvo MIDI.
    char nombre_wav[MAX_STR];       //Nombre del archivo WAV.

    //Se inicializan con los valores por defecto.
    signed char canal = CANAL_PREDET;       //Canal
    int frecuencia = FRECUENCIA_PREDET;     //Frecuencia de muestreo.
    int pulsosporsegundo = PPS_PREDET;      //Pulsos por segundo.

    //PROCESO LOS PARAMETROS.
    if(!procesar_parametros(argc, argv, nombre_synth, nombre_mid, nombre_wav, &canal, &frecuencia, &pulsosporsegundo)){
        uso(argv);
        return 1;
    }


    //ABRO EL ARCHIVO DEL SINTETIZADOR
    FILE *fs = fopen(nombre_synth, "rt");
    if(fs == NULL){
        fprintf(stderr, "No se pudo abrir el archivo %s\n",nombre_synth);
        return 1;
    }


    //CREO EL SINTETIZADOR.
    sintetizador_t *sintetizador = sintetizador_crear_vacio();
    if(sintetizador == NULL){
        fprintf(stderr,"No se pudo crear el sintetizador\n");
        fclose(fs);
        return 1;
    }

    //LEO EL ARCHIVO DEL SINTETIZADOR Y ALMACENO SUS PARÁMETROS EN EL SINTETIZADOR.
    if(!leer_sintetizador(fs, sintetizador)){
        fprintf(stderr,"Fallo en la lectura del archivo sintetizador\n");
        sintetizador_destruir(sintetizador);
        fclose(fs);
        return 1;
    }

    //CIERRO EL ARCHIVO DEL SINTETIZADOR.
    fclose(fs);


    //ABRO EL ARCHIVO MIDI
    FILE *fm = fopen(nombre_mid, "rb");
    if(fm == NULL){
        fprintf(stderr, "No se pudo abrir el archivo %s\n",nombre_mid);
        sintetizador_destruir(sintetizador);
        return 1;
    }

    //CREO EL CONTENEDOR DE NOTAS.
    contenedor_t *contenedor = contenedor_crear_vacio();
    if(contenedor == NULL){
        fprintf(stderr, "No se pudo crear el contenedor de notas\n");
        sintetizador_destruir(sintetizador);
        fclose(fm);
        return 1;
    }

    //LEO EL ARCHIVO MIDI Y ALMACENO LAS NOTAS DEL CANAL EN EL CONTENEDOR.
    if(! leer_midi(fm, contenedor, &canal, pulsosporsegundo)){
        fprintf(stderr, "Error leyendo el archivo MIDI\n");
        fclose(fm);
        sintetizador_destruir(sintetizador);
        contenedor_destruir(contenedor);
        return 1;
    }

    //CIERRO EL ARCHIVO MIDI.
    fclose(fm);
    

    //CREO EL TRAMO BASE CON EL T0 DE LA PRIMERA NOTA DEL CONTENEDOR.
    double t0_base = contenedor_obtener_inicio_primera_nota(contenedor);
    double tf_base = t0_base + 0.00001;

    tramo_t *tramo = tramo_crear_base(t0_base, tf_base, frecuencia);
    if (tramo == NULL)
    {
        fprintf(stderr, "Error creando el tramo base\n");
        sintetizador_destruir(sintetizador);
        contenedor_destruir(contenedor);
        return 1;
    }

    //SINTETIZO LAS NOTAS DEL CONTENEDOR Y LAS ALMACENO EN EL TRAMO.
    if (! sintetizar_notas(contenedor, tramo, frecuencia, sintetizador)){
        fprintf(stderr, "Error al sintetizar\n");
        sintetizador_destruir(sintetizador);
        contenedor_destruir(contenedor);
        tramo_destruir(tramo);
        return 1;
    }

    //ABRO EL ARCHIVO WAV
    FILE *fw = fopen(nombre_wav, "wb");
    if(fw == NULL){
        fprintf(stderr, "No se pudo abrir el archivo %s\n",nombre_wav);
        contenedor_destruir(contenedor);
        sintetizador_destruir(sintetizador);
        tramo_destruir(tramo);
        return 1;
    }

    
    //ESCRIBO EL ARCHIVO WAV
    if(! escribir_wav(fw, tramo)){
        fprintf(stderr, "No se pudo escribir el archivo %s\n",nombre_wav);
        fclose(fw);
        contenedor_destruir(contenedor);
        sintetizador_destruir(sintetizador);
        tramo_destruir(tramo);
        return 1;
    }

    //DESTRUYO EL TRAMO, EL CONTENEDOR Y EL SINTETIZADOR.
    contenedor_destruir(contenedor);
    tramo_destruir(tramo);
    sintetizador_destruir(sintetizador);

    //CIERRO EL ARCHIVO WAV.
    fclose(fw);


    return 0;
}




void uso(const char *argv[]) {
    fprintf(stderr, "El programa convierte un archivo MIDI en un archivo WAV.\n");
    fprintf(stderr, "Uso\n\t%s -s <sintetizador.txt> -i <entrada.mid> -o <salida.wav> [-c <canal>] [-f <frecuencia>] [-r <pulsosporsegundo>]\n", argv[0]);
}


bool procesar_parametros(size_t n, char const *argv[], char nombre_synth[], char nombre_mid[], char nombre_wav[], signed char *canal, int *frecuencia, int *pulsosporsegundo){

    //Verifico la cantidad de parametros ingresados
    if(n < 7) {
        return false; 
    }
    
    bool flag_s = false;
    bool flag_i = false;
    bool flag_o = false;
    bool flag_c = false;
    bool flag_f = false;
    bool flag_r = false;

    char *endptr;

     for (size_t i = 0; i < n; i++)
    {

        if ((!strcmp(argv[i], "-s")) && (!flag_s))
        {
            if(++i >= n)
                return false;

            strcpy(nombre_synth,argv[i]);
            flag_s = true; 
        }

        if ((!strcmp(argv[i], "-i")) && (!flag_i))
        {
            if(++i >= n)
                return false;

            strcpy(nombre_mid,argv[i]);
            flag_i = true; 
        }

        if ((!strcmp(argv[i], "-o")) && (!flag_o))
        {
            if(++i >= n)
                return false;

            strcpy(nombre_wav,argv[i]);
            flag_o = true; 
        }

        if ((!strcmp(argv[i], "-c")) && (!flag_c))
        {
            
            if(++i >= n)
                return false;

            *canal = strtol(argv[i],&endptr,10);

            if(endptr == argv[i])
                return false;
            

            if(*endptr != '\0' || *canal < 0)
                return false;

            flag_c = true; 
        }

        if ((!strcmp(argv[i], "-f")) && (!flag_f))
        {
            if(++i >= n)
                return false;

            *frecuencia = strtol(argv[i],&endptr,10);

            if(endptr == argv[i])
                return false;

            if(*endptr != '\0' || *frecuencia <= 0)
                return false;
            
            flag_f = true; 
        }

        if ((!strcmp(argv[i], "-r")) && (!flag_r))
        {
            if(++i >= n)
                return false;

           *pulsosporsegundo = strtol(argv[i],&endptr,10);

           if(endptr == argv[i])
                return false;

            if(*endptr != '\0' || *pulsosporsegundo <= 0)
                return false;
            
            flag_r = true; 
        }

    }

    if(flag_s == false || flag_i == false || flag_o == false)
        return false;
    
    return true;

}

