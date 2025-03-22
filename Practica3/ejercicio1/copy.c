#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // Necesario para las constantes O_*
#include <sys/types.h> //Necesario para mode_t
#include <sys/stat.h> //Necesario para mode_t
#include <unistd.h>

extern char* optarg;
extern int optind;


void copy(int fdo, int fdd)
{
	char* buffer = malloc(128);
	int err;
	while((err = read(fdo, buffer, sizeof(char))) != 0){
		if(err == -1){
			perror("You suck reading");
			close(fdo);
			close(fdd);
			exit(EXIT_FAILURE);
		}
		if(write(fdd, buffer, sizeof(char)) == -1){
			perror("You suck writting");
			close(fdo);
			close(fdd);
			exit(EXIT_FAILURE);
		}
	}

	close(fdo);
	close(fdd);
	free(buffer);
}

int main(int argc, char *argv[])
{
	char* input, *output;
	int opt;
	while((opt = getopt(argc, argv, "i:o:")) != -1){
		switch(opt){
		case 'i':
			input = optarg; 
			break;
		case 'o':
			output = optarg;
			break;
		}
	}

	if(input == NULL || output ==  NULL){
		perror("Please be kind and write the input and output :(");
		exit(EXIT_FAILURE);
	}

	copy(open(input, O_RDONLY), open(output, O_WRONLY | O_CREAT | O_TRUNC, 0664));

	return 0;
}
