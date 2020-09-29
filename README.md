# FIUBA - 95.11 Algoritmos y Programación 1.

## Trabajo Práctico - Sintetizador. 

_El presente trabajo práctico es la implementación de un sintetizador de audio capaz de convertir archivos MIDI en archivos WAVE._

### Compilación

_Para compilar el programa se debe ejectuar el comando "make" en la terminal de Linux, lo cual creará el archivo "sintetizador"._

### Ejecución

_El archivo "sintetizador" se debe ejectuar de la siguiente manera:_

```
$ ./sintetizador -s <sintetizador.txt> -i <entrada.mid> -o <salida.wav> [-c <canal>] [-f <frecuencia>] [-r <pulsosporsegundo>]
```

_Donde los parámetros representan:_

_<sintetizador.txt> es el nombre del archivo donde se encuentran los parámetros del sintetizador._
_<entrada.mid> es el nombre del archivo MIDI a convertir._
_<salida.wav> es el nombre que tendrá el archivo WAV._
_<canal> es el número del canal MIDI a convertir. Este parámetros es opcional._
_<frecuencia> es la frecuencia de muestreo. Este parámetros es opcional._
_<pulsosporsegundo> son los pulsos por segundos al cual se convertirá el archivo MIDI. Este parámetros es opcional._



_Se proporcionan los archivos: "sintetizador.txt", "sintetizador_burning.txt", "sintetizador_piano.txt", "sintetizador_flauta.txt", "sintetizador_organo.txt", "sintetizador_telefono.txt", "sintetizador_violin.txt" y "nothing_else_matters.mid" para poder ejecutar el programa. Un ejemplo de ejecución sería:_

```
$ ./sintetizador -s sintetizador_piano.txt -i nothing_else_matters.mid -o test.wav -c 1 -f 44100 -r 130
```