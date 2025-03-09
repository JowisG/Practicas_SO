#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor
 *
 * The loadstr() function must allocate memory from the heap to store
 * the contents of the string read from the FILE.
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc())
 *
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{
	int c; // Caracter
	int size = 0; // Longitud del string (supongo)
	int capacidad = 10; // Capacidad maxima inicial del string
	char* str = malloc(capacidad); // String de memoria dinámica que inicia con tamaño maximo de capacidad

	if(!str){ // Error del malloc
		return NULL;
	}
	
	long int original_pos = ftell(file); // ftell dice donde esta el puntero en el archivo su posición

	while((c = getc(file)) != EOF && c != '\0'){
		size++;
		if(size >= capacidad){ // No cabe en el str aumentamos capacidad
			capacidad *= 2;
			char* tmp = realloc(str, capacidad);
			if(!tmp){ // No se relocalizo bien str a tmp
				free(str);
				return NULL;
			}
			str = tmp; // str tiene más capacidad
		}
	}

	if(fseek(file, original_pos, SEEK_SET)){ // Si no hace bien fseek
		free(str);
		return NULL;
	}

	int i = 0;
	while((c = getc(file)) != EOF && c != '\0'){
		str[i++] = (char) c; // str[i] y luego i+1 eso es lo que hace la linea
	}
	str[i] = '\0';

	return (i == 0)? NULL : str;	
}

int main(int argc, char *argv[])
{
	FILE* file;
	char* str; // Pûntero que apuntara al inicio de un "string"

	if (argc != 2){ // Comprobamos si tiene al menos un argumento (argc == 2)
		fprintf(stderr, "Uso: %s <file_name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Abrimos file en modo r
	file = fopen(argv[1], "r");
	if(file == NULL){
		perror("No se pudo abrir el archivo: " + *argv[1]);
		exit(EXIT_FAILURE);
	}

	while((str = loadstr(file)) != NULL){
		printf("%s\n", str);
		free(str); // Por que es dinamico supongo
	}

	// Para los errores
	if(ferror(file)){
		perror("Error al leer el archivo\n");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	fclose(file);

	exit(EXIT_SUCCESS);
}
