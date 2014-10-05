#include "csa.h"

#define SAMPLES_PER_MILLISEC (8)
#define TOTAL_GRAPH_TIME_MILLISEC (40)

int main(int argc, char** argv){
	pid_t pid;
	FILE * octaveFd = NULL;
	int sizeOfData; 
	int16_t * datosBrutos = NULL;
	int16_t * datosSaturados = NULL;  
	int16_t * datosSuavizados = NULL;

	comprobacionDatosEntrada(argc, argv);
	creacionOctave( &pid, &octaveFd );
	obtencionDatos( argv[1], argv[2], &datosBrutos, &datosSaturados ,&sizeOfData); // No fue ingresado p por lo tanto no se obtendran todos los datos lo que disminuye el procesamiento
	//suavizamientoDeSaturacion( &datosSuavizados, sizeOfData );
	generacionGraficos(&octaveFd, &datosBrutos, &datosSaturados, argv[3]);
	if(argc == 5 && (char)*argv[4] == 'p')
		reproducir( datosBrutos, datosSaturados, datosSuavizados, sizeOfData );

	protocoloDeSalida(&octaveFd, pid, &datosBrutos, &datosSaturados);
	return 0;
}


void comprobacionDatosEntrada(int argc, char** argv){
	if(argc != 5 ){
		printf("El correcto uso del programa tiene el siguiente prototipo:\n -> %s <ruta archivo de audio> <ganancia> <offset> [p]\n", argv[0]);
		exit(1);
	}
}


void creacionOctave( int * pid, FILE ** octaveFd ){
    int pfd[2];
	/*
     * Create a pipe.
     */
    if (pipe(pfd) < 0) {
        perror("pipe");
        exit(1);
    }
    /*
     * Create a child process.
     */
    if ((*pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    /*
     * The child process executes "octave" with -i option.
     */
    if (*pid == 0) {
      int junk;
        /*
         * Attach standard input of this child process to read from the pipe.
         */
        dup2(pfd[0], 0);
        close(pfd[1]);  /* close the write end of the pipe */
        junk = open("/dev/null", O_WRONLY);
        dup2(junk, 1);  /* throw away any message sent to screen*/
        dup2(junk, 2);  /* throw away any error message */
        execlp("octave", "octave", "-i", (char *)0);
        perror("exec");
        exit(-1);
    }
    close(pfd[0]);
    *octaveFd = fdopen(pfd[1], "w");  /* to use fprintf instead of just write */
    
}


void obtencionDatos( char * rutaAudio , char * _ganancia, int16_t ** datosBrutos , int16_t ** datosSaturados ,int * sizeOfData){
	FILE * archivoAudioRaw = fopen(rutaAudio, "rb");
	*sizeOfData = 0;
	int16_t valor;
	double ganancia = atof(_ganancia);
	
	int i=0;
	while(	fread(&valor,sizeof(valor),1,archivoAudioRaw) != 0   ){
		i++;
	}
	*sizeOfData = i;

	archivoAudioRaw = fopen(rutaAudio, "rb");
	*datosBrutos = (int16_t *) malloc(*sizeOfData * sizeof(int16_t) );
	*datosSaturados = (int16_t *) malloc(*sizeOfData * sizeof(int16_t) );
	i=0;
	while(	fread(&valor,sizeof(valor),1,archivoAudioRaw) != 0   ){
		(*datosBrutos)[i] = valor;
		int16_t saturados = ganancia*valor;
		(*datosSaturados)[i] = saturados/ganancia;
		i++;
	}

}


void protocoloDeSalida(FILE ** octaveFd, int pid, int16_t ** datosBrutos, int16_t ** datosSaturados){
	printf("Ingrese cualquier tecla para terminar\n"); fflush(stdout);
	getchar();
    fprintf(*octaveFd, "\n exit\n"); 
	fflush(*octaveFd);
	waitpid(pid, NULL, 0);
    fclose(*octaveFd);
    free(*datosBrutos);
    free(*datosSaturados);
    printf("\n");fflush(stdout);
}

void generacionGraficos(FILE ** octaveFd, int16_t ** datosBrutos, int16_t ** datosSaturados, char * _offset ){ 

	int i;

	// FALTA EL GRAFICO CON SUAVIZAMIENTO

	int offset = atoi(_offset);
	//========================================
	fprintf(*octaveFd, "datosBrutos = [ ");
	//int i = 0;
	for (i= offset * SAMPLES_PER_MILLISEC; i < offset * SAMPLES_PER_MILLISEC + TOTAL_GRAPH_TIME_MILLISEC*SAMPLES_PER_MILLISEC; ++i)
	{
		fprintf(*octaveFd, "%" PRId16 " ", (*datosBrutos)[i]); // Eso de "%" PRId16 " " es por que el int16_t es un formato especial.
															// "%" PRId16 es el formato para imprimir int16_t luego se debe volver a abrir " " para escribir el espacio que hay que darle a octave.
	}
	fprintf(*octaveFd, " ]\n" ); 
	fflush(*octaveFd);
	
	//============================================
	fprintf(*octaveFd, "datosSaturados = [ ");
	for (i= offset * SAMPLES_PER_MILLISEC; i < offset * SAMPLES_PER_MILLISEC + TOTAL_GRAPH_TIME_MILLISEC * SAMPLES_PER_MILLISEC; ++i)
	{
		fprintf(*octaveFd, "%" PRId16 " ", (*datosSaturados)[i]);
	}
	fprintf(*octaveFd, " ]\n" ); 
	fflush(*octaveFd);
	//===========================================
	fprintf(*octaveFd, "tiempo = [ ");
	for (i = offset * SAMPLES_PER_MILLISEC; i < offset * SAMPLES_PER_MILLISEC + TOTAL_GRAPH_TIME_MILLISEC * SAMPLES_PER_MILLISEC; ++i)
	{
		fprintf(*octaveFd, "%f ", i/8000.0);
	}
	fprintf(*octaveFd, " ]\n" ); fflush(*octaveFd);
	//===========================================

	fprintf(*octaveFd, "subplot(2,1,1);\n");fflush(*octaveFd);
    fprintf(*octaveFd, "plot(tiempo,datosBrutos);\n");fflush(*octaveFd);
    fprintf(*octaveFd, "subplot(2,1,2);\n");fflush(*octaveFd);
	fprintf(*octaveFd, "plot(tiempo,datosSaturados)\n");fflush(*octaveFd);
}


void suavizamientoDeSaturacion(/*  ..., int16_t ** datosSuavizados, ... */){
	// Deberia consistir en dos metodos
	// obtenerPosicionesDeSaturaciones( ... )
	// obtenerDatosSuavizados( ..., int16_t ** datosSuavizados, ... )
}



void reproducir(int16_t * datosBrutos, int16_t * datosSaturados, int16_t * datosSuavizados, int sizeOfData){
	// Falta con el audio filtrado
	FILE * audioNormal = fopen("./Audio/audioNormal.raw", "wb");
	FILE * audioSaturado = fopen("./Audio/audioSaturado.raw", "wb");
	int i;
	for (i = 0; i < sizeOfData; ++i)
	{
		fwrite(&(datosBrutos[i]), sizeof(int16_t), 1, audioNormal);
		fwrite(&(datosSaturados[i]), sizeof(int16_t), 1, audioSaturado);
	}

	system("aplay --format=S16_LE -t raw Audio/audioNormal.raw");
	system("aplay --format=S16_LE -t raw Audio/audioSaturado.raw");
}