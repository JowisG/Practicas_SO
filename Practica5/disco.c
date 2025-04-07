#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 5
#define NUM_HILOS 50
#define VIPSTR(vip) ((vip) ? "  vip  " : "not vip")

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

void enter_normal_client(cliente_t cliente)
{
	pthread_mutex_lock(mutex); // !cerrar mutex
	
	n_normales++;

	// sala de espera
	while(n_pista >= CAPACITY || n_vips > 0 || turno_act_n != cliente.turno){
		printf("Hilo de cliente normal %d ha entrado\n", cliente.id);
		pthread_cond_wait(&normales, mutex);
	}

	// *se vienen cositas
	n_pista++;
	n_normales--;
	turno_act_n++;

	pthread_mutex_unlock(mutex); // ?abrir mutex
}

void enter_vip_client(cliente_t cliente)
{
	pthread_mutex_lock(mutex); // !cerrar mutex
	
	n_vips++;

	// sala de espera
	while(n_pista >= CAPACITY || turno_act_vip != cliente.turno){
		printf("Hilo de cliente vip %d ha entrado\n", cliente.id);
		pthread_cond_wait(&vips, mutex);
	}

	// *se vienen cositas
	n_pista++;
	n_vips--;
	turno_act_vip++;

	pthread_mutex_unlock(mutex); // ?abrir mutex
}

void dance(int id, int isvip)
{
	printf("Client %2d (%s) dancing in disco\n", id, VIPSTR(isvip));
	sleep((rand() % 3) + 1);
}

void disco_exit(int id, int isvip)
{
	pthread_mutex_lock(mutex); // !cerrar mutex
	n_pista--;

	if(n_vips > 0)
		pthread_cond_signal(&vips);
	else
		pthread_cond_signal(&normales);

	pthread_mutex_unlock(mutex); // ?abrir mutex
}

void *client(void *arg)
{
	cliente_t cliente = *(cliente_t*)arg;
	cliente.id = pthread_self();

	if (cliente.vip){
		enter_vip_client(cliente);
	}
	else{
		enter_normal_client(cliente);
	}

	dance(cliente.id, cliente.vip);

	disco_exit(cliente.id, cliente.vip);
}

int main(int argc, char *argv[])
{
	// #incializar memoria global y mutex
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


	// ?Abrir archivo
	FILE* file;
	file = fopen("ejemplo.txt", "r");
	if(file == NULL){
		perror("file does not like u");
		exit(EXIT_FAILURE);
	}

	// Leer archivo
	int num_personas;
	if(fscanf(file, "%d", &num_personas) == 0){
		perror("You never learnt how to read");
		exit(EXIT_FAILURE);
	}

	l_pid = malloc(num_personas*sizeof(pthread_t));

	for(int i = 0; i < num_personas; i++){
		int vip;
		if(fscanf(file, "%d", &vip) == 0){
			perror("You never learnt how to read");
			exit(EXIT_FAILURE);
		}
		// !Crear hilo


		int cont_n = -1;
		int cont_v = -1;
		if(vip == 1)
			cont_v++;
		else
			cont_n++;
		cliente_t cliente;
		cliente.vip = vip;
		if(vip == 1)
			cliente.turno = cont_v;
		else
			cliente.turno = cont_n;
		if(pthread_create(&l_pid[i], NULL, client, &cliente) != 0){
			perror("error creating thread");
			exit(EXIT_FAILURE);
		}
	}

	// *Esperar a los hilos
	for(int i = 0; i < num_personas; i++){
		pthread_join(l_pid[i], NULL);
	}


	// *Liberacion de memoria
	free(l_pid);
	pthread_mutex_destroy(mutex);
	return 0;
}
