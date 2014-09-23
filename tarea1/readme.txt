|-----------------------------------------|
|	TAREA 1 - PROGRAMACION DE SISTEMAS    |
|	   23 de Septiembre del año 2014      |
|    Profesor: Agustín Gonzalez Vidal     |
|    Autores: Pascal Sigel Olivares       |
|		      Rol: 2921055-1              |
|										  |
|		      Oscar Silva Muñoz           |
|		      Rol: 2803029-0              |
|-----------------------------------------|


El directorio actual contiene 3 archivos: el script fn2u, el archivo man manfn2u, y este mismo readme.

La descripción del script fn2u se muestra a continuación:


NOMBRE
fn2u \- Lista y modifica directorios y archivos de nombres no validos

SINOPSIS
fn2u [OPCIONES] DIRECTORIO ARGUMENTO

DESCRIPCION
Busca dentro de un directorio especifico todos los directorios y archivos con nombres que contengan caracteres acentuados, espacios, o caracteres ñ mayusculas y minusculas, y los lista o modifica de acuerdo a la opción dada por el usuario.

OPCIONES	
   -h  Muestra el texto de ayuda y termina la ejecución del script.

ARGUMENTOS
	1  Esta opción solo lista el nombre de directorios y archivos encontrados dentro del directorio especificado.
	2  Esta opción renombra todos los archivos y directorios encontrados.

EJEMPLOS
	./fn2u mi_directorio 2
Busca y renombra directorios y archivos dentro del directorio 'mi_directorio' que contengan los caracteres indicados anteriormente
	./fn2u my_directory 1
Busca y lista los directorios y archivos dentro del directorio 'mi_directorio' que contengan los caracteres insicados anteriormente

VEASE TAMBIEN
mv(1), sed(1)


Además usted puede acceder al menu man del script fn2u, donde se indicará el contenido indicado anteriormente de la siguiente forma:

man ./manfn2u
