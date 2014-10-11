#ifndef CSA_H
#define CSA_H
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <math.h>
void comprobacionDatosEntrada(int argc, char** argv);
void creacionOctave( int * pid, FILE ** octaveFd );
void obtencionDatos( char * rutaAudio , char * _ganancia, int16_t ** datosBrutos , int16_t ** datosSaturados, double ** sim_time ,int * sizeOfData);
void protocoloDeSalida(FILE ** octaveFd, pid_t pid, int16_t ** datosBrutos, int16_t ** datosSaturados);
void generacionGraficos(FILE ** octaveFd, int16_t ** datosBrutos, int16_t ** datosSaturados, int16_t ** datosSuavizados, char * offset );
void suavizamientoDeSaturacion(FILE ** octaveFd, int16_t ** datosSuavizados , int16_t ** datosSaturados, double ** sim_time, float ganancia ,int sizeOfData);
void reproducir(int16_t * datosBrutos, int16_t * datosSaturados, int16_t * datosSuavizados, int sizeOfData);
#endif