#ifndef MIDI_H
#define MIDI_H

#include <stdbool.h>
#include <stdio.h>


#include "contenedor.h"

//Lee un archivo MIDI y llena el contenedor con las notas encontrada en el canal especificado.
bool leer_midi(FILE *f, contenedor_t *contenedor, signed char *canal, int pps);




#endif