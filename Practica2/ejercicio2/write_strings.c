#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
	FILE* file;
	int i;

	if(argc < 2){
		fprintf(stderr, "Uso: %s <filenme>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	file = fopen(argv[1], "w");
	if(file == NULL){
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	for(i = 2; i < argc; i++){
		fwrite(argv[i], sizeof(char), strlen(argv[i]), file);
		fwrite("\0", sizeof(char), 1, file);
	}
	fclose(file);

	return EXIT_SUCCESS;
}
