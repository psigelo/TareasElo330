#include "csa.h"

#define SAMPLES_PER_MILLISEC (8)
#define TOTAL_GRAPH_TIME_MILLISEC (40)

/*
 El Objetivo es crear un corrector de saturación de audio a través de polinomios de orden 4.
*/
int main(int argc, char** argv){
	pid_t pid;
	FILE * octaveFdWrite = NULL;
	int sizeOfData; 
	int16_t * datosBrutos = NULL;
	int16_t * datosSaturados = NULL;  
	int16_t * datosSuavizados = NULL;
	/*
		Comprobacion de los datos de entrada: Se analiza si la entrada con que el usuario ejecutó el programa es válida.
		CreacionOctave: Se crea el proceso que representa a octave para su uso posterior en la graficación de las señales.
		ObtencionDatos: Se obtienen los datos del archivo de audio para su posterior tratamiento
		suavizamiento de Saturacion: Se suaviza la saturación de la señal y se guarda en un arreglo diferente.
		generacionGraficos: Se crean los gráficos a través de octave.
		calculoDeError: Se calcula el error entre la señal entrante y la suavizada.
		reproducir: Si el usuario entró el parámetro p entonces se reproducen los audios.
		protocolo de Salida: Se libera la memoria utilizada y se espera a que el usuario aprete enter.
	*/
	comprobacionDatosEntrada(argc, argv);
	creacionOctave( &pid, &octaveFdWrite);
	obtencionDatos( argv[1], argv[2], &datosBrutos, &datosSaturados ,&sizeOfData); // No fue ingresado p por lo tanto no se obtendran todos los datos lo que disminuye el procesamiento
	suavizamientoDeSaturacion( &datosSaturados, &datosSuavizados, sizeOfData, argv[2]);
	generacionGraficos(&octaveFdWrite, &datosBrutos, &datosSaturados, &datosSuavizados, argv[3]);
	calculoDeError(&datosBrutos, &datosSuavizados,sizeOfData);
	if(argc == 5 && (char)*argv[4] == 'p')
		reproducir( datosBrutos, datosSaturados, datosSuavizados, sizeOfData );
	protocoloDeSalida(&octaveFdWrite, pid, &datosBrutos, &datosSaturados);
	return 0;
}

/*
Comprobacion de los datos de entrada: Se analiza si la entrada con que el usuario ejecutó el programa es válida.
*/
void comprobacionDatosEntrada(int argc, char** argv){
	if(argc < 4 ){
		printf("El correcto uso del programa tiene el siguiente prototipo:\n -> %s <ruta archivo de audio> <ganancia> <offset> [p]\n", argv[0]);
		exit(1);
	}
}

/*
CreacionOctave: Se crea el proceso que representa a octave para su uso posterior en la graficación de las señales.
*/
void creacionOctave( int * pid, FILE ** octaveFdWrite ){
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
        dup2(junk,1);
        dup2(junk, 2);  /* throw away any error message */
        execlp("octave", "octave", "-iq", (char *)0);
        perror("exec");
        exit(-1);
    }
    close(pfd[0]);
    *octaveFdWrite = fdopen(pfd[1], "w");  /* to use fprintf instead of just write */
}


/*
ObtencionDatos: Se obtienen los datos del archivo de audio para su posterior tratamiento
*/
void obtencionDatos( char * rutaAudio , char * _ganancia, int16_t ** datosBrutos , int16_t ** datosSaturados ,int * sizeOfData){
	FILE * archivoAudioRaw = fopen(rutaAudio, "rb");
	*sizeOfData = 0;
	int16_t valor;
	double ganancia = atof(_ganancia);
	
	int i=0;
	while(	fread(&valor,sizeof(valor),1,archivoAudioRaw) != 0   ){ // Primero se obtiene el largo de los datos
		i++;
	}
	*sizeOfData = i;

	archivoAudioRaw = fopen(rutaAudio, "rb");
	*datosBrutos = (int16_t *) malloc(*sizeOfData * sizeof(int16_t) );
	*datosSaturados = (int16_t *) malloc(*sizeOfData * sizeof(int16_t) );
	i=0;
	while(	fread(&valor,sizeof(valor),1,archivoAudioRaw) != 0   ){ // Se obtiene dato a dato y se calcula su version saturada
		(*datosBrutos)[i] = valor;
		(*datosSaturados)[i] = ((abs(ganancia*valor) < 0x7fff) ? valor : valor/abs(valor)*(0x7fff/ganancia))  ;
		i++;
	}

}


/*
 suavizamiento de Saturacion: Se suaviza la saturación de la señal y se guarda en un arreglo diferente.
*/
void suavizamientoDeSaturacion( int16_t ** datosSaturados,  int16_t ** datosSuavizados, int sizeOfData, char * _ganancia ){
	*datosSuavizados = (int16_t *) malloc(sizeOfData * sizeof(int16_t) );
	int i;
	//double ganancia = atof(_ganancia);
	double ganancia = atof(_ganancia);
	int16_t saturado_superior = (ganancia > 1 ) ? 0x7fff/ganancia : 0x7fff;
	int16_t saturado_inferior = (ganancia > 1 ) ? -0x7fff/ganancia : -0x7fff;




	for (i = 0; i < sizeOfData; ++i)
	{
		if(  ((*datosSaturados)[i] != saturado_superior) && ((*datosSaturados)[i] != saturado_inferior) ){
			(*datosSuavizados)[i] = (int16_t)((*datosSaturados)[i]);
		}
		else{
			int j=0; // Representa la cantidad de datos saturados en el bloque - 1
			while( i+j < sizeOfData ) // En realidad no es ese el limite
			{
				if ( (*datosSuavizados)[i+j] == saturado_superior || (*datosSuavizados)[i+j] == saturado_inferior ){
					j++;
				}
				else{
					break;
				}
			}
			if(i+j == sizeOfData){
				// Si el bloque de saturados posee al ultimo valor tambien entonces no se puede hacer nada
				int k = i;
				for ( ; k <= i+j; ++k)
				{
					(*datosSuavizados)[k] = (int16_t)((*datosSaturados)[i]);
				}
			}
			else{
				int16_t valoresAnterioresDeMuestra[3];
				int16_t valoresPosterioresDeMuestra[2];
				valoresAnterioresDeMuestra[0] = (*datosSuavizados)[i-3];
				valoresAnterioresDeMuestra[1] = (*datosSuavizados)[i-2];
				valoresAnterioresDeMuestra[2] = (*datosSuavizados)[i-1];
				valoresPosterioresDeMuestra[0] = (*datosSaturados)[i+j+1];
				valoresPosterioresDeMuestra[1] = (*datosSaturados)[i+j+2];

				// Se realizara una aproximacion polinomial de lagrange como se aprende en analisis numerico

				int k = i;
				for ( ; k <= i+j; ++k)
				{
					(*datosSuavizados)[k] = valoresAnterioresDeMuestra[0]*( k - (i-2) )*( k - (i-1) )*( k - (i + j + 1))*( k - (i + j + 2))/(( (i-3) - (i-2) )*( (i-3) - (i-1) )*( (i-3) - (i + j + 1))*( (i-3) - (i + j + 2))) + 
											valoresAnterioresDeMuestra[1]*( k - (i-3) )*( k - (i-1) )*( k - (i + j + 1))*( k - (i + j + 2))/(( (i-2) - (i-3) )*( (i-2) - (i-1) )*( (i-2) - (i + j + 1))*( (i-2) - (i + j + 2))) +
											valoresAnterioresDeMuestra[2]*( k - (i-3) )*( k - (i-2) )*( k - (i + j + 1))*( k - (i + j + 2))/(( (i-1) - (i-3) )*( (i-1) - (i-2) )*( (i-1) - (i + j + 1))*( (i-1) - (i + j + 2))) +
											valoresPosterioresDeMuestra[0]*( k - (i-3) )*( k - (i-2) )*( k - (i-1) )*( k - (i + j + 2))/(( (i+j+1) - (i-3) )*( (i+j+1) - (i-2) )*( (i+j+1) - (i-1) )*( (i+j+1) - (i + j + 2))) +
											valoresPosterioresDeMuestra[1]*( k - (i-3) )*( k - (i-2) )*( k - (i-1) )*( k - (i + j + 1))/(( (i+j+2) - (i-3) )*( (i+j+2) - (i-2) )*( (i+j+2) - (i-1) )*( (i+j+2) - (i + j + 1))) ;
				}
			}
			i = i + j;
		}
	}
}
/*
	generacionGraficos: Se crean los gráficos a través de octave.
*/
void generacionGraficos(FILE ** octaveFdWrite, int16_t ** datosBrutos, int16_t ** datosSaturados, int16_t ** datosSuavizados,  char * _offset ){ 

	int i;

	int offset = atoi(_offset);
	//========================================
	fprintf(*octaveFdWrite, "datosBrutos = [ ");
	//int i = 0;
	for (i= offset * SAMPLES_PER_MILLISEC; i < offset * SAMPLES_PER_MILLISEC + TOTAL_GRAPH_TIME_MILLISEC*SAMPLES_PER_MILLISEC; ++i)
	{
		fprintf(*octaveFdWrite, "%" PRId16 " ", (*datosBrutos)[i]); // Eso de "%" PRId16 " " es por que el int16_t es un formato especial.
															// "%" PRId16 es el formato para imprimir int16_t luego se debe volver a abrir " " para escribir el espacio que hay que darle a octave.
	}
	fprintf(*octaveFdWrite, " ];\n" ); 
	fflush(*octaveFdWrite);
	
	//============================================
	fprintf(*octaveFdWrite, "datosSaturados = [ ");
	for (i= offset * SAMPLES_PER_MILLISEC; i < offset * SAMPLES_PER_MILLISEC + TOTAL_GRAPH_TIME_MILLISEC * SAMPLES_PER_MILLISEC; ++i)
	{
		fprintf(*octaveFdWrite, "%" PRId16 " ", (*datosSaturados)[i]);
	}
	fprintf(*octaveFdWrite, " ];\n" ); 
	fflush(*octaveFdWrite);
	//============================================
	fprintf(*octaveFdWrite, "datosSuavizados = [ ");
	for (i= offset * SAMPLES_PER_MILLISEC; i < offset * SAMPLES_PER_MILLISEC + TOTAL_GRAPH_TIME_MILLISEC * SAMPLES_PER_MILLISEC; ++i)
	{
		fprintf(*octaveFdWrite, "%" PRId16 " ", (*datosSuavizados)[i]);
	}
	fprintf(*octaveFdWrite, " ];\n" ); 
	fflush(*octaveFdWrite);
	//===========================================
	fprintf(*octaveFdWrite, "tiempo = [ ");
	for (i = offset * SAMPLES_PER_MILLISEC; i < offset * SAMPLES_PER_MILLISEC + TOTAL_GRAPH_TIME_MILLISEC * SAMPLES_PER_MILLISEC; ++i)
	{
		fprintf(*octaveFdWrite, "%f ", i/8000.0);
	}
	fprintf(*octaveFdWrite, " ];\n" ); fflush(*octaveFdWrite);
	//===========================================

	fprintf(*octaveFdWrite, "subplot(3,1,1);\n");fflush(*octaveFdWrite);
	fprintf(*octaveFdWrite, "plot(tiempo,datosBrutos);\n");fflush(*octaveFdWrite);
	fprintf(*octaveFdWrite, "subplot(3,1,2);\n");fflush(*octaveFdWrite);
	fprintf(*octaveFdWrite, "plot(tiempo,datosSaturados);\n");fflush(*octaveFdWrite);
	fprintf(*octaveFdWrite, "subplot(3,1,3);\n");fflush(*octaveFdWrite);
	fprintf(*octaveFdWrite, "plot(tiempo,datosSuavizados);\n");fflush(*octaveFdWrite);
}

/*
calculoDeError: Se calcula el error entre la señal entrante y la suavizada.
*/
void calculoDeError(int16_t ** datosBrutos, int16_t ** datosSuavizados,int sizeOfData){
	long int errorSum=0;
	int i;
	for (i = 0; i < sizeOfData; ++i)
	{
		errorSum += abs( (*datosBrutos)[i] - (*datosSuavizados)[i])* ( (*datosBrutos)[i] - (*datosSuavizados)[i]);
	}
	printf("La suma de error es: %ld\n", errorSum);
}

/*
reproducir: Si el usuario entró el parámetro p entonces se reproducen los audios.
*/
void reproducir(int16_t * datosBrutos, int16_t * datosSaturados, int16_t * datosSuavizados, int sizeOfData){
	FILE * audioNormal = fopen("./audioNormal.raw", "wb");
	FILE * audioSaturado = fopen("./audioSaturado.raw", "wb");
	FILE * audioSuavizado = fopen("./audioSuavizado.raw", "wb");
	int i;
	for (i = 0; i < sizeOfData; ++i)
	{
		fwrite(&(datosBrutos[i]), sizeof(int16_t), 1, audioNormal);
		fwrite(&(datosSaturados[i]), sizeof(int16_t), 1, audioSaturado);
		fwrite(&(datosSuavizados[i]), sizeof(int16_t), 1, audioSuavizado);
	}
	system("aplay --format=S16_LE -t raw audioNormal.raw");
	system("aplay --format=S16_LE -t raw audioSaturado.raw");
	system("aplay --format=S16_LE -t raw audioSuavizado.raw");
}


/*
protocolo de Salida: Se libera la memoria utilizada y se espera a que el usuario aprete enter.
*/
void protocoloDeSalida(FILE ** octaveFdWrite, int pid, int16_t ** datosBrutos, int16_t ** datosSaturados){
	printf("Ingrese cualquier tecla para terminar\n"); fflush(stdout);
	getchar();
    fprintf(*octaveFdWrite, "\n exit\n"); 
	fflush(*octaveFdWrite);
	waitpid(pid, NULL, 0);
    fclose(*octaveFdWrite);
    free(*datosBrutos);
    free(*datosSaturados);
    printf("\n");fflush(stdout);
}