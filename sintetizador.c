#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "sintetizador.h"

#define PI 3.141592653589793
#define EPSILON 0.5             //Valor minimo para evitar que la cantidad de muestras se trunque al valor inferior.
#define MAX_ARMONICOS 20        //Cantidad máxima de armónicos.

//Prototipos de las funciones de modulación.
float modulacion_constante(double t, float params[3]);
float modulacion_lineal(double t, float params[3]);
float modulacion_invlineal(double t, float params[3]);
float modulacion_sin(double t, float params[3]);
float modulacion_exp(double t, float params[3]);
float modulacion_invexp(double t, float params[3]);
float modulacion_quartcos(double t, float params[3]);
float modulacion_quartsin(double t, float params[3]);
float modulacion_halfcos(double t, float params[3]);
float modulacion_halfsin(double t, float params[3]);
float modulacion_log(double t, float params[3]);
float modulacion_invlog(double t, float params[3]);
float modulacion_tri(double t, float params[3]);
float modulacion_pulses(double t, float params[3]);


/*
    Estructura de las funciones de modulación.
    Posee el nombre de la función y su puntero a función.
*/
typedef struct{    

    char *nombre;               //Nombre de la función de modulación.
    modulacion_t modulacion;    //Puntero a función de la función de modulación.

} funcion_t;


//Diccionario de las funciones de modulación.
static const funcion_t funciones_mod[] = {

    {"CONSTANT", modulacion_constante},
    {"LINEAR", modulacion_lineal},
    {"INVLINEAR", modulacion_invlineal},
    {"SIN", modulacion_sin},
    {"EXP", modulacion_exp},
    {"INVEXP", modulacion_invexp},
    {"QUARTCOS", modulacion_quartcos},
    {"QUARTSIN", modulacion_quartsin},
    {"HALFCOS", modulacion_halfcos},
    {"HALFSIN", modulacion_halfsin},
    {"LOG", modulacion_log},
    {"INVLOG", modulacion_invlog},
    {"TRI", modulacion_tri},
    {"PULSES", modulacion_pulses},
   
};

/*
    Estructura del Sintetizador. Se almacena: la cantidad de armónicos, los armónicos,
    los parámetros para sintetizar la nota. El nombre de las funciones de modulación y
    un vector estático donde se almacenan los punteros a las funciones de modulación.

*/
struct synth{
    int n_armonicos;                    //Cantidad de armonicos  
    float armonicos[MAX_ARMONICOS][2];  //Armonicos
    float params[3][3];                 //Los parametros del sintetizador. [0] ataque, [1] sostenido, [2] decaimiento.
    char funciones[3][MAX_STR];         //Vector de cadenas donde se encuentran los nombres de las funciones de modularizacion. [0][x] ataque, [1][x] sostenido, [2][x] decaimiento.
    modulacion_t func_mod[3];           //Vector de punteros a funciones de modularizacion. [0] ataque, [1] sostenido, [2] decaimiento.
};


//Crea y devuelve un sintetizador vacío. Si falla devuelve NULL.
sintetizador_t *sintetizador_crear_vacio(){

    sintetizador_t *sintetizador = malloc(sizeof(sintetizador_t));
    if(sintetizador == NULL)
        return NULL;

    sintetizador->n_armonicos = 0;

    return sintetizador;

}

//Destruye el sintetizador.
void sintetizador_destruir(sintetizador_t *sintetizador){
    free(sintetizador);
}


//Sintetiza una nota y devuelve el tramo en el cual se sintetizó la nota. Si falla devuelve NULL.
tramo_t *sintetizar_nota(nota_t *nota, int f_m, sintetizador_t *sintetizador){

    double t_ataque = sintetizador->params[0][0];       //Tiempo de ataque de la nota.

    double t_decaimiento = sintetizador->params[2][0];  //Tiempo de decaimiento de la nota.

    double inicio_nota = obtener_inicio_nota(nota);

    double duracion_nota = obtener_duracion_nota(nota) + t_decaimiento;
    
    double tf =  inicio_nota + duracion_nota;

    double f = obtener_frecuencia_nota(nota);

    float intensidad = obtener_intensidad_nota(nota); 

    tramo_t *nota_sintetizada = tramo_crear_muestreo(inicio_nota, tf, f_m, f, intensidad, sintetizador->armonicos, sintetizador->n_armonicos);
    if(nota_sintetizada == NULL)
        return NULL;


    modular_tramo_sintetizado(nota_sintetizada, duracion_nota, t_ataque, t_decaimiento, sintetizador->params, sintetizador->func_mod);

    return nota_sintetizada;

}


//Sintetiza las notas de un contenedor y las almacena en el tramo.
bool sintetizar_notas(contenedor_t *contenedor, tramo_t *tramo, int f_m, sintetizador_t *sintetizador){

    size_t n = contenedor_obtener_tamagno(contenedor);

    for (size_t i = 0; i < n; i++){                         

        nota_t *nota = contenedor_obtener_nota(contenedor, i);

        tramo_t *nota_sintetizada = sintetizar_nota(nota, f_m, sintetizador);
        if(nota_sintetizada == NULL)
            return false;


        if(! tramo_extender(tramo, nota_sintetizada)){
            tramo_destruir(nota_sintetizada);
            return false;
        }
    
        tramo_destruir(nota_sintetizada);
    }

    return true;
}


//**** Inicio de Funciones de Modulación. ****
float modulacion_constante(double t, float params[3]) {
        return 1;
}

float modulacion_lineal(double t, float params[3]){
    return t/params[0];
}

float modulacion_invlineal(double t, float params[3]){
    float resultado =  (1 - t/params[0]);
    if(resultado < 0){
        return 0;
    }else return resultado;
}

float modulacion_sin(double t, float params[3]){
    return (1 + params[0]*sin(params[1]*t)) ;
}

float modulacion_exp(double t, float params[3]){
    double exponente = (5*(t - params[0]))/params[0];
    return exp(exponente);
}

float modulacion_invexp(double t, float params[3]){
    double exponente = (-5*t)/params[0];
    return exp(exponente);
}

float modulacion_quartcos(double t, float params[3]){
    return cos((PI*t)/(2*params[0]));
}

float modulacion_quartsin(double t, float params[3]){
    return sin((PI*t)/(2*params[0]));
}

float modulacion_halfcos(double t, float params[3]){
    return (1 + cos((PI*t)/params[0]))/2;
}

float modulacion_halfsin(double t, float params[3]){
    return (1 + sin((PI*((t/params[0]) - 1/2))))/2;
}

float modulacion_log(double t, float params[3]){
    return log((9*t/params[0]) + 1);
}

float modulacion_invlog(double t, float params[3]){
    if(t < params[0]){
        return log((-9*t/params[0]) + 10);
    }else return 0;
}

float modulacion_tri(double t, float params[3]){
    if(t < params[1]){
        double num = (t*params[2]/params[1]);
        return num;
    }
    if (t > params[1]){
        double num = ((t - params[1])/(params[1] - params[0]))*(params[2] - 1) + params[2];
        return num;
    }else return 0;
}

float modulacion_pulses(double t, float params[3]){
    
    float t_prima = params[0]*(t/params[0] - (floor(t/params[0])));
    int valor_absoluto = abs(((1-params[2])/params[1])*(t_prima - params[0] + params[1]));

    float f_prima = valor_absoluto + params[2];

    if(f_prima < 1)
        return f_prima;

    return 1;

}

//**** Fin de Funciones de Modulación. ****


//Lee el archivo de texto del sintetizador y llena el sintetizador con los parámetros encontrados en el archivo.
//OJO: NO VALIDA STRTOD NI STRTOL. SE ASUME QUE VIENEN CORRECTOS EN EL ARCHIVO SINTETIZADOR.
bool leer_sintetizador(FILE *f, sintetizador_t *sintetizador){

    char aux[MAX_STR];
    char *endptr;
    if(fgets(aux, MAX_STR, f) == NULL)
        return false;
    

    sintetizador->n_armonicos = strtol(aux, &endptr, 10);
    if(sintetizador->n_armonicos <= 0)
        return false;

    for (size_t i = 0; i < sintetizador->n_armonicos; i++)
    {
        if(fgets(aux, MAX_STR, f) == NULL)
            return false;
        

        sintetizador->armonicos[i][0] = strtod(aux, &endptr);
        if(sintetizador->armonicos[i][0] < 0)
            return false;
        

        sintetizador->armonicos[i][1] = strtod(endptr, NULL);
        if(sintetizador->armonicos[i][1] < 0)
            return false;
        

    }
     
     char *ptr;
     char *ptr2;
     size_t contador = 0;

    for (size_t j = 0; j < 3; j++){

        if(fgets(aux, MAX_STR, f) == NULL)
            return false;
        

        for (; isalpha(aux[contador]); contador++){     


            sintetizador->funciones[j][contador] = aux[contador];      
        }

        sintetizador->funciones[j][contador] = '\0';

        //Almaceno las funciones en el sintetizador usando el diccionario.
        size_t i;
        for(i = 0; i < sizeof(funciones_mod) / sizeof(funciones_mod[0]); i++) {     
            if(! strcmp(funciones_mod[i].nombre, sintetizador->funciones[j])) {

               sintetizador->func_mod[j] = funciones_mod[i].modulacion;

                break;
            }
        }


        if(i == sizeof(funciones_mod) / sizeof(funciones_mod[0])) {
            return false;
        }


        contador++;

        sintetizador->params[j][0] = strtod(&aux[contador], &ptr);

        sintetizador->params[j][1] = strtod(ptr, &ptr2);

        sintetizador->params[j][2] = strtod(ptr2, NULL);

        contador = 0;



    }

    return true;

}