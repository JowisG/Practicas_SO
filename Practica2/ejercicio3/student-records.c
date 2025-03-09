#include <stdio.h>
#include <unistd.h> /* for getopt() */
#include <stdlib.h> /* for EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include "defs.h"


/* Assume lines in the text file are no larger than 100 chars */
#define MAXLEN_LINE_FILE 100

extern char* optarg;
extern int optind;

int print_text_file(char *path)
{
	FILE* file;
	if((file = fopen(path, "r")) == NULL){
		fprintf(stderr, path, " could not be opened");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	char line[MAXLEN_LINE_FILE];
	if(fgets(line, MAXLEN_LINE_FILE, file) == NULL){
		fprintf(stderr, "File is empty, please especify amount of entries it has\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	int cases = atoi(line);

	for(int i = 0; i < cases; i++){
		printf("[Entry #%d]\n", i);
		if(fgets(line, MAXLEN_LINE_FILE, file) == NULL){
			fprintf(stderr, "Entry Not found\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}

		char* line_ptr = line;
		char* token;
		token_id_t token_id = STUDENT_ID_IDX;
		while((token = strsep(&line_ptr, ":")) != NULL){
			printf("     ");
			char* etiq = malloc(32);
			switch(token_id){
			case STUDENT_ID_IDX:
				strcpy(etiq, "student_id");
				break;
			case NIF_IDX:
				strcpy(etiq, "NIF");
				break;
			case FIRST_NAME_IDX:
				strcpy(etiq, "first_name");
				break;
			case LAST_NAME_IDX:
				strcpy(etiq, "last_name");
				break;
			case NR_FIELDS_STUDENT:
				strcpy(etiq, "No_field_student");
				break;
			default:
				strcpy(etiq, "No_field_found");
			}
			printf("%s = ", etiq);
			printf("%s\n", token);
			token_id++;
			free(etiq);
		}
	}

	return 0;
}

int print_binary_file(char *path)
{
	/* To be completed  (part C) */
	return 0;
}


int write_binary_file(char *input_file, char *output_file)
{
	FILE* input;
	if((input = fopen(input_file, "rb")) == NULL){ // "rb abre en modo binario"
		fprintf(stderr, input_file, " could not be opened");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	student_t data;
	char line[MAXLEN_LINE_FILE];
	if(fgets(line, MAXLEN_LINE_FILE, input) == NULL){
		fprintf(stderr, "File is empty, please especify amount of entries it has\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	int cases = atoi(line);

	for (int i = 0; i < cases; i++)
	{
		if(fgets(line, MAXLEN_LINE_FILE, input) == NULL){
			fprintf(stderr, "Entry Not found\n");
			perror(NULL);
			exit(EXIT_FAILURE);
		}

		char* line_ptr = line;
		char* token;
		token_id_t token_id = STUDENT_ID_IDX;
		while((token = strsep(&line_ptr, ":")) != NULL){
			switch(token_id){
			case STUDENT_ID_IDX:
				data.student_id = atoi(token);	
				break;
			case NIF_IDX:
				//Forma que no me gusta
				for(int j = 0; j < MAX_CHARS_NIF+1; j++){
					data.NIF[j] = token[j];
				}
				// data.NIF = token; lanza un error de que no es modificable
				break;
			case FIRST_NAME_IDX:
				data.first_name = malloc(MAXLEN_LINE_FILE);
				strcpy(data.first_name, token);
				break;
			case LAST_NAME_IDX:
				data.last_name = malloc(MAXLEN_LINE_FILE);
				strcpy(data.last_name, token);
				break;
			default:
				fprintf(stderr, "No option found for this student field");
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			token_id++;
		}

		/* Escribir en binario en el archivo ouput data */
	}
	printf("Stop");
	
	free(data.first_name);
	free(data.last_name);
	return 0;
}

int main(int argc, char *argv[])
{
	int ret_code, opt;
	struct options options;

	/* Initialize default values for options */
	options.input_file = NULL;
	options.output_file = NULL;
	options.action = NONE_ACT;
	ret_code = 0;

	/* Parse command-line options (incomplete code!) */
	while ((opt = getopt(argc, argv, "hi:po:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			fprintf(stderr, "Usage: %s [ -h | -i <input_file> | -o <output_file> | -p ]\n", argv[0]);
			exit(EXIT_SUCCESS);
		case 'i':
			options.input_file = optarg;
			break;
		case 'p':
			options.action = PRINT_TEXT_ACT;
			break;
		case 'o':
			options.action = WRITE_BINARY_ACT;
			options.output_file = optarg;
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	if (options.input_file == NULL)
	{
		fprintf(stderr, "Must specify one record file as an argument of -i\n");
		exit(EXIT_FAILURE);
	}

	switch (options.action)
	{
	case NONE_ACT:
		fprintf(stderr, "Must indicate one of the following options: -p, -o, -b \n");
		ret_code = EXIT_FAILURE;
		break;
	case PRINT_TEXT_ACT:
		/* Part A */
		ret_code = print_text_file(options.input_file);
		break;
	case WRITE_BINARY_ACT:
		/* Part B */
		ret_code = write_binary_file(options.input_file, options.output_file);
		break;
	case PRINT_BINARY_ACT:
		/* Part C */
		ret_code = print_binary_file(options.input_file);
		break;
	default:
		break;
	}
	exit(ret_code);
}
