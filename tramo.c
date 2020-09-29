#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>


#include "tramo.h"


#define PI 3.141592653589793
#define F_I 0			//Las frencuencias relativas se encuentran en la Columna 0.
#define A_I 1			//Las amplitudes relativas se encuentran en la Columna 1.
#define PHI 0			//En este caso la función seno no está desplazada.
#define EPSILON 0.5		//Valor minimo para evitar que la cantidad de muestras se trunque al valor inferior.

/*
	Estructura del tramo. Posee un vector dinámico de muestras y su longitud n.
	El tiempo inicial del tramo y su frecuencia de muestreo.
*/
struct s_tramo{

float *v;			//Vector dinámico de muestras.
size_t n;			//Longitud del vector dinámico.
double t0;			//Tiempo inicial del tramo.
int f_m;			//Frecuencia de muestro del tramo.

};


//Esta función se usa para calcular el valor de la onda en ti.
static double _onda(double t, float a, float f, float phi){
	return a*sin(2*PI*f*t + phi);
}

//Inicializar todos los valores en 0.
static void _inicializar_muestras(float v[], size_t n){

	for (size_t i = 0; i < n; i++){
		v[i] = 0;
	}		
		
}

//Muestreo de la senoidal
static void _muestrear_senoidal(float v[], size_t n, double t0, int f_m, float f, float a){


	for (size_t i = 0; i < n; i++)
	{
		double ti = t0 + (double)i/f_m;
		v[i] += _onda(ti,a,f,PHI);

	}

}

//Muestreo de armonicos.
static void _muestrear_armonicos(float v[], size_t n, double t0, int f_m, float f, float a, float fa[][2], size_t n_fa){

	_inicializar_muestras(v,n);  

	for (size_t i = 0; i < n_fa; i++)
	{
		float a_i = a*fa[i][A_I];
		float f_i = f*fa[i][F_I];
		_muestrear_senoidal(v,n,t0,f_m,f_i,a_i); 	
		
	}

}


//Destruye el tramo.
void tramo_destruir(tramo_t *t){

	if(t->v != NULL) {					
    	free(t->v);							
    }
    free(t);                            
}


//Funcion auxiliar, la usan las demas funciones. Crea el tramo pero no inicializa los valores de tramo->v.
static tramo_t *_tramo_crear(double t0, double tf, int f_m){

	tramo_t *t;

	if((t = malloc(sizeof(tramo_t))) == NULL)
		return NULL;
	
	t->t0 =  t0;
	t->f_m = f_m;
	t->n = f_m*(tf - t0) + 1 + EPSILON; //Le sumo un EPSILON para evitar que se trunque al valor inferior.
    if((t->v = malloc(t->n * sizeof(float))) == NULL) {
        tramo_destruir(t);       
        return NULL;
    }

	return t;
}



//Crear un nuevo tramo y luego llama a la funcion muestrear_armonicos para obtener los valores de las muestras y almacenarlos en tramo->v.
tramo_t *tramo_crear_muestreo(double t0, double tf, int f_m, float f, float a, float fa[][2], size_t n_fa){

	tramo_t *t = _tramo_crear(t0, tf, f_m);
	if (t == NULL)
		return NULL;

	_muestrear_armonicos(t->v, t->n, t0, f_m, f, a, fa, n_fa);

	return t;
}


//Agranda o achica el tramo, cambiando n y el tamaño de v. Si se agranda, las muestras nuevas las inicializa en 0.
static bool _tramo_redimensionar(tramo_t *t, double tf){

	size_t n_old = t->n;
	t->n = t->f_m*(tf - t->t0) + 1 + EPSILON; //Le sumo un EPSILON para evitar que se trunque al valor inferior.
	float *vaux = realloc(t->v, t->n * sizeof(float)); 
            if(vaux == NULL) {
            	t->n = n_old;
                return false;
            }
     
    t->v = vaux;
    if(t->n > n_old){
		for(size_t i = n_old; i < t->n; i++)
			t->v[i] = 0;
    }

    return true;

}


//Suma los tramos.
bool tramo_extender(tramo_t *destino, const tramo_t *extension){

	if(destino->t0 > extension->t0 || destino->f_m != extension->f_m) 
		return false;

	//Guardo los valores de tiempo final para compararlos.
	double tf_extension = extension->t0 + (extension->n - 1) / (double)(extension->f_m);
	double tf_destino = destino->t0 + (destino->n - 1) / (double)(destino->f_m);

	size_t delta_n = 0;

	if (tf_destino < tf_extension)	//Si el tiempo final del destino es menor al de la extension debo redimensionar el tramo.
	{
		if(!_tramo_redimensionar(destino, tf_extension))  
				return false;
			 
	}

	if(destino->t0 < extension->t0){//Evalúo si el tiempo inicial del destino es menor al tiempo inicial de la extension.
		//Calculo el delta n entre los t0 de ambos tramos para asi saber desde donde empezar a sumar los tramos.
		delta_n = ((double)destino->f_m*(extension->t0 - destino->t0) + EPSILON); //Le sumo un EPSILON para evitar que se trunque al valor inferior.
	} 

	for (size_t i = 0; i < extension->n; i++)
		destino->v[delta_n + i] += extension->v[i];

	

	return true;
}

//Devuelve el tamaño del tramo.
size_t tramo_obtener_tamagno(tramo_t *tramo){
	return tramo->n;
}

//Devuelve la muestra máxima del tramo.
float tramo_obtener_muestra_maxima(tramo_t *tramo){
	float max = 0;

	for (size_t i = 0; i < tramo->n; i++)
	{
		if(tramo->v[i] > max){
			max = tramo->v[i];
		}
	}

	return max;
}

//Normaliza las muestras del tramo a int16_t
int16_t tramo_normalizar_muestra(tramo_t *tramo, size_t pos, float muestra_maxima){

	return tramo->v[pos] * (INT16_MAX/muestra_maxima);

}

//Crea y devuelve un tramo base.
tramo_t *tramo_crear_base(double t0, double tf, int f_m){

	float armonicos[2][2] = {0};

	tramo_t *tramo = tramo_crear_muestreo(t0, tf, f_m, 0, 0, armonicos,0);
	if (tramo == NULL)
		return NULL;

	return tramo;
}

//Modula un tramo sintetizado.
void modular_tramo_sintetizado(tramo_t *tramo, double duracion_nota, double t_ataque, double t_decaimeinto, float params[][3], modulacion_t funcion[3]){
	

    int f_m = tramo->f_m;

    //Ataque
    size_t n_a = f_m*(t_ataque) + EPSILON; 
    if(n_a > tramo->n)
        n_a = tramo->n;

    //Sostenido
    double t_sostenido = duracion_nota - t_decaimeinto - t_ataque;
    if(t_sostenido < 0)
        t_sostenido = 0;	
    
    size_t n_s = f_m*(t_sostenido) + EPSILON; 

    //Decaimiento
    size_t n_d = f_m*(t_decaimeinto) + 1 + EPSILON;

    size_t n_total = n_d + n_a + n_s;
    if( n_total > tramo->n){
        n_d -= n_total - tramo->n;
    }

    
    //Sintetizo el ataque.
    for (size_t j = 0; j < n_a; j++)
    {
        double ti = (double)j/f_m;	
        tramo->v[j] *= (*funcion[0])(ti,params[0]);
                
    }

    //Sintetizo el sostenido.
    for (size_t j = 0; j < n_s; j++)
    {
        size_t pos = j + n_a;
        double ti = (double)j/f_m;	
        		
        tramo->v[pos] *= (*funcion[1])(ti,params[1]);

    }
    
    //Sintetizo el decaimiento.
    for (size_t j = 0; j < n_d; j++)
    {
        size_t pos = j + n_a + n_s;
        double ti = (double)j/f_m;	

        tramo->v[pos] *= (*funcion[2])(ti,params[2]);
    }


}

int tramo_obtener_frecuencia(tramo_t *tramo){
	return tramo->f_m;
}