#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>


/* Forward declaration */
int get_size_dir(char *fname, size_t *blocks);

/* Gets in the blocks buffer the size of file fname using lstat. If fname is a
 * directory get_size_dir is called to add the size of its contents.
 */
int get_size(char *fname, size_t *blocks)
{
	int err;
	struct stat stat_data;
	if(lstat(fname, &stat_data) == -1){
		perror("lstat failed");
		return -1;
	}

	if(S_ISREG(stat_data.st_mode)){
		printf("File\n");
		*blocks += stat_data.st_blocks;
	}else if(S_ISDIR(stat_data.st_mode)){
		printf("Directory\n");
		if(get_size_dir(fname, blocks) == -1){
			return -1;
		}
	}else{
		perror("The input selected is neither a link or a regular file");
		return -1;
	}
	return 0;
}


/* Gets the total number of blocks occupied by all the files in a directory. If
 * a contained file is a directory a recursive call to get_size_dir is
 * performed. Entries . and .. are conveniently ignored.
 */
int get_size_dir(char *dname, size_t *blocks)
{	
	int err;
	DIR* dir;
	if((dir = opendir(dname)) == NULL){
		perror("opendir failed");
		return -1;
	}

	struct dirent* dir_data;
	while((dir_data = readdir(dir)) != NULL){
		if(strcmp(dir_data->d_name, ".") == 0 || strcmp(dir_data->d_name, "..") == 0);
		else{
			struct stat tmp;
			if(lstat(dir_data->d_name, &tmp) == -1){
				perror("lstat failed");
				closedir(dir);
				return -1;
			}

			if(S_ISREG(tmp.st_mode)){
				printf("File\n");
				*blocks += tmp.st_blocks;
			}else if(S_ISDIR(tmp.st_mode)){
				printf("Directory\n");
				if(get_size_dir(dir_data->d_name, blocks) == -1){
					closedir(dir);
					return -1;
				}
			}else{
				perror("The input selected is neither a link or a regular file");
				closedir(dir);
				return -1;
			}
		}
	}
	closedir(dir);
	return 0;
}

/* Processes all the files in the command line calling get_size on them to
 * obtain the number of 512 B blocks they occupy and prints the total size in
 * kilobytes on the standard output
 */
int main(int argc, char *argv[])
{
	char* entrada;
	size_t* tam;
	*tam = 0;
	int res;
	for(int i = 1; i < argc; i++){
		entrada = argv[i];
		res = get_size(entrada, tam);
		if (res == -1)
			exit(EXIT_FAILURE);
		printf("%s: %dB\n", entrada, (int)*tam);
	}
	return 0;
}
