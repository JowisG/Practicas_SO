#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>  // Necesario para las constantes O_*
#include <sys/types.h> //Necesario para mode_t
#include <sys/stat.h> //Necesario para mode_t
#include <unistd.h>

extern char* optarg;
extern int optind;


void copy(int fdo, int fdd) {
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

void copy_regular(char *orig, char *dest)
{

}

void copy_link(char *orig, char *dest) {
	struct stat stat_data;
	if(lstat(orig, &stat_data) != 0){
		perror("lstat failed");
		exit(EXIT_FAILURE);
	}
	int size = stat_data.st_size;

	char* buffer = malloc(size+1);
	if(readlink(orig, buffer, size) == -1){
		perror("Error al usar readlink");
        exit(EXIT_FAILURE);
	}
	buffer[size] = '\0';

	if (symlink(buffer, dest) == -1){
		perror("Error al usar symlink");
        exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
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

	struct stat stat_data;
	if(lstat(input, &stat_data) != 0){
		perror("lstat failed");
		exit(EXIT_FAILURE);
	}

	if(S_ISREG(stat_data.st_mode)){
		printf("Normal copy\n");
		copy(open(input, O_RDONLY), open(output, O_WRONLY | O_CREAT | O_TRUNC, 0664));
	}else if(S_ISLNK(stat_data.st_mode)){
		printf("Link copy\n");
		copy_link(input, output);
	}else{
		perror("The input selected is neither a link or a regular file");
		exit(EXIT_FAILURE);
	}

	return 0;
}
