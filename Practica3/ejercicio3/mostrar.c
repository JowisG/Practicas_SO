#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern char* optarg;
extern int optind;

void display(char* input, int n, int read_end){
	struct stat stat_data;
	if(lstat(input, &stat_data) == -1){
		perror("lstat failed");
		exit(EXIT_FAILURE);
	}
	if(!S_ISREG(stat_data.st_mode)){
		perror("File is not a regular file");
		exit(EXIT_FAILURE);
	}

	int fd;
	if((fd = open(input, O_RDONLY)) == -1){
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	int err;
	if(read_end){
		printf("Option end\n");
		err = lseek(fd, -n, SEEK_END);
	}else{
		printf("Option skip\n");
		err = lseek(fd, n, SEEK_SET);
	}

	if(err == -1){
		perror("lseek failed");
		exit(EXIT_FAILURE);
	}

	err = 0;
	char* buffer = malloc(128);
	while((err = read(fd, buffer, sizeof(char))) != 0){
		if(err == -1){
			perror("read failed");
			exit(EXIT_FAILURE);
		}
		printf("%s", buffer);
	}

}

int main(int argc, char *argv[])
{
	char *input = NULL;
	int bytes_read = 0;
	int read_end = 0;
	int opt;
	while((opt = getopt(argc, argv, "hi:n:e")) != -1){
		switch (opt){
		case 'h':
			printf("Use: h for this massege | -i <file> to display | -n <bytes> to skip | -e change option -n to just read the last <bytes> in file\n");
			exit(EXIT_SUCCESS);
		case 'i':
			input = optarg;
			break;	
		case 'n':
			bytes_read = strtol(optarg, NULL, 10);
			break;
		case 'e':
			read_end = 1;
			break;
		}
	}

	if(input == NULL){
		perror("Please put option -i <file> to indicat the file to display\n");
		exit(EXIT_FAILURE);
	}

	display(input, bytes_read, read_end);

	return 0;
}
