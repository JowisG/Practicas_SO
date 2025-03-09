#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	char buffer[BUFFER_SIZE];
	size_t bytes_read;

	if (argc!=2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "rb")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
		/* Print byte to stdout */
		if (fwrite(buffer, sizeof(char), bytes_read, stdout) != bytes_read){
			perror("Error al escribir la salida estandar");
			fclose(file);
			exit(EXIT_FAILURE);
		}
	}
	printf("\n");
	if (ferror(file)) {
        perror("Error al leer el archivo");
        fclose(file);
        exit(EXIT_FAILURE);
    }

	fclose(file);
	return 0;
}
