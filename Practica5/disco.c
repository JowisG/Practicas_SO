#include <stdio.h> //* Standart input/output library functions fgets, fopen...
#include <stdlib.h> // Para cosas estandar como malloc
#include <unistd.h> //! Para la usar sleep y read, write, close y getopt
#include <pthread.h> //IMP Para los hilos (threads)
//\ #include <string.h> //! Para las funciones de string: strcpy, strdup, strlen, strsep...
//\ #include <ctype.h> // Pra incluir más cosas para los char funciones de toupper y cosas asi
//\ #include <fcntl> //IMP Para las constantes de los ficheros O_
/*
* 	O_RDONLY: Solo lectura.
*	O_WRONLY: Solo escritura.
*	O_CREAT: Crea el archivo si no existe.
*	O_TRUNC: Si el archivo existe y se abre para escritura, lo trunca (borra su contenido).
*	O_APPEND: Escritura, pero añadiendo al final.
*	O_RDWR: Lectura y escritura. 
 */
//\ #include <sys/types.h> //# para el mode_t
//\ #inclued <sys/stat.h> //# para el mode_t y stat
/*
!    ino_t     st_ino;     // Número de inodo
!    mode_t    st_mode;    // Permisos y tipo de archivo
!    nlink_t   st_nlink;   // Número de enlaces duros
!    off_t     st_size;    // Tamaño total, en bytes
!    blksize_t st_blksize; // Tamaño de bloque para E/S
!    blkcnt_t  st_blocks;  // Número de bloques asignados
*/
//\ #include <dirent.h> //* Para el manejo de DIR

#define CAPACITY 2
#define NUM_HILOS 50
#define VIPSTR(vip) ((vip) ? "  VIP  " : "not vip")

/**
 * @brief Estructura que define un cliente, siendo este vip o no, con un turno y un id
 * @param turno Es el turno el cual el cliente tiene para poder entarr en la discoteca (Se define añ crearse el cliente)
 * @param vip Define si es vip (1) o si es normal (0)
 * @param id Es el id del hilo
 */
typedef struct{
	int turno;
	int vip;
	int id;
}cliente_t;

// !threads y mutex
pthread_mutex_t* mutex;
// ?var condicionales
pthread_cond_t normales, vips;

int n_normales, n_vips, turno_act_n, turno_act_vip, n_pista;

/**
 * @brief Cola de los clientes normales. Tiene una sala de espera la cual esta en una sección critica con una variable condicional:
 * @brief - Solo salen si la pista tiene espacio
 * @brief - Si no hay vips
 * @brief - Si es el turno del cliente que quiere salir
 * 
 * Una vez salen actualizan el turno y la cantidad de gente en la pista y la cantidad de gente en la cola.
 * @param cliente cliente_t
 * @return
 */
void enter_normal_client(cliente_t cliente)
{
	pthread_mutex_lock(mutex); // !cerrar mutex
	
	n_normales++;

	//? sala de espera
	while(n_pista >= CAPACITY || n_vips > 0 || turno_act_n != cliente.turno){ //* Condiciones para salir de la sala de espera
		pthread_cond_wait(&normales, mutex);
	}

	//* Cuando sale de la cola
	printf("NORMAL with turn: %d finished QEUEU. Turns of NORMAL: %d\n", cliente.turno, turno_act_vip);
	n_pista++;
	n_normales--;
	turno_act_n++;

	pthread_mutex_unlock(mutex); // ?abrir mutex
}

/**
 * @brief Cola de los vips. Tiene una sala de espera la cual esta en una sección critica con una variable condicional:
 * @brief - Solo salen si la pista tiene espacio
 * @brief - Si es el turno del vip que quiere salir
 * 
 * Una vez salen actualizan el turno y la cantidad de gente en la pista y la cantidad de gente en la cola.
 * @param cliente cliente_t
 * @return
 */
void enter_vip_client(cliente_t cliente)
{
	pthread_mutex_lock(mutex); // !cerrar mutex
	
	n_vips++;

	//? sala de espera
	while(n_pista >= CAPACITY || turno_act_vip != cliente.turno){ //* Condiciones para salir de la sala de espera
		pthread_cond_wait(&vips, mutex); //? Sala de espera
	}

	//* Cuando sale de la cola
	printf("VIP with turn: %d finished QEUEU. Turns of VIP: %d\n", cliente.turno, turno_act_vip);
	n_pista++;
	n_vips--;
	turno_act_vip++;

	pthread_mutex_unlock(mutex); // ?abrir mutex
}
/**
 * @brief Simula que los clientes estan bailando durante un tiempo aleatorio
 * @param id id del cliente
 * @param isvip si es vip el cliente
 * @return
 */
void dance(int id, int isvip)
{
	int time = (rand() % 5) + 1;
	printf("Client %d (%s) dancing in disco. Time dacing: %d\n", id, VIPSTR(isvip), time);
	sleep(time);
}

/**
 * @brief La salida de la psita de la discoteca. Cuando el cliente sale disminuye la cantidad de clientes la disco luego procede
 * a ver si quedan vips para llamar 1 a 1 (si se cambia broadcast llama a todos) para que pruebe si puede entar en la dico.
 * @param id id del cliente
 * @param isvip si es vip el cliente
 * @return
 */
void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(mutex); //! cerrar mutex
	
	n_pista--;
	printf("\tThe cliente %d (%s) exits the disco.\n", id, VIPSTR(isvip));
	// Pascal dijo que sería mejor si usamos broadcast, aunque me funciona con signal
	if(n_vips > 0)
		pthread_cond_signal(&vips); //# llamamos a la sala vip de 1 en 1 (con broadcast se levantan todos)
	else
		pthread_cond_signal(&normales); //# llamamos a la sala normal de 1 en 1 (con broadcast se levantan todos)

	pthread_mutex_unlock(mutex); //? abrir mutex
}

/**
 * @brief IMPORTANT funcion anonima. Se llama despues de crear el hilo del cliente se encarga de dirigir a los normales
 * a la cola de los normales y los vips a la cola de los vips. Tabien se encarga de desreferenciar la entrada args a cliente_t
 * y le pone el id al cliente con el id del thread.
 * @param *args void: puntero que deberá venir con un cliente_t
 * @return NULL
 */
void *client(void *arg)
{
	//b* Desreferenciamos el argumento
	cliente_t* cliente = (cliente_t*)arg;
	cliente->id = pthread_self();

	// Miramos si es un vip
	if (cliente->vip){
		enter_vip_client(*cliente); //! Entramos en la cola de los vips
	}
	else{
		enter_normal_client(*cliente);//! Entramos en la cola de los normales
	}

	dance(cliente->id, cliente->vip); //? Bailamos al salir de la cola

	disco_exit(cliente->id, cliente->vip); //? Salimos de la sicoteca

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	//# incializar memoria global y mutex
	mutex = malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(mutex, NULL) != 0){
		perror("mutex init error");                                        
    	exit(EXIT_FAILURE);   
	}
	if(pthread_cond_init(&normales, NULL) != 0){
		perror("cond_init normales() error");                                        
    	exit(EXIT_FAILURE);   
	}
	if(pthread_cond_init(&vips, NULL) != 0){
		perror("cond_init vips() error");                                        
    	exit(EXIT_FAILURE);   
	}
	n_normales = 0;
	n_vips = 0;
	turno_act_n = 0;
	turno_act_vip = 0;
	n_pista = 0;
	pthread_t* l_pid;


	//? Abrir archivo
	char* file_name = argv[1]; // cojemos el argumento
	if(file_name == NULL){
		perror("Needs the file name or path as an argument: ./name_of_program <file>\n");
		exit(EXIT_FAILURE);
	}

	//! Abrimos el archivo
	FILE* file;
	file = fopen(file_name, "r");
	if(file == NULL){
		perror("file does not like u");
		exit(EXIT_FAILURE);
	}

	//b* Leemos el archivo
	int num_personas; // leemos la cantidad de clientes
	if(fscanf(file, "%d", &num_personas) == 0){
		perror("You never learnt how to read");
		exit(EXIT_FAILURE);
	}

	//! reservamos memoria para los ids de los procesos
	l_pid = malloc(num_personas*sizeof(pthread_t));

	//* Comenzamos 2 contadores para los turnos de los vips y normales
	int cont_normales = 0;
	int cont_vips = 0;
	cliente_t clientes[num_personas];
	for (int i = 0; i < num_personas; i++){
		//b* Leemos sie es un vip o no
		int vip;
		if(fscanf(file, "%d", &vip) == 0){
			perror("You never learnt how to read");
			exit(EXIT_FAILURE);
		}

		//imp Creamos el cliente
		cliente_t cliente;
		cliente.vip = vip;
		if (vip) {
			cliente.turno = cont_vips;
			cont_vips++;
		}else{
			cliente.turno = cont_normales;
			cont_normales++;
		}
		clientes[i] = cliente;
	}

	for(int i = 0; i < num_personas; i++){
		
		//# Creamos el hilo con la información correpondiente
		if(pthread_create(&l_pid[i], NULL, client, &clientes[i]) != 0){
			perror("error creating thread");
			exit(EXIT_FAILURE);
		}
	}

	//* Esperar a los hilos
	for(int i = 0; i < num_personas; i++){
		pthread_join(l_pid[i], NULL);
	}

	printf("\n----Fin del programa: todos los hilos han terminado----\n");

	//* Liberacion de memoria
	free(l_pid);
	pthread_mutex_destroy(mutex);
	pthread_cond_destroy(&normales);
	pthread_cond_destroy(&vips);
	return 0;
}
