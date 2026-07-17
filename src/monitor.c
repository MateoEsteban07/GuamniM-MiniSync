#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "scanner.h"
#include "worker.h"
#include "ipc.h"
#include "logger.h"


#define NUM_WORKERS 4

stats_t *stats_global;
pid_t worker_pids[NUM_WORKERS];

void manejador_signit(int sig){
    printf("Monitor / Presionar Crtl+C para salir\n");
    printf("......................\n");
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (worker_pids[i] > 0) {
            kill(worker_pids[i], SIGTERM);
            printf("Señal SIGTERM enviada al worker con PID: %d\n", worker_pids[i]);
        }
    }
    if ( stats_global != NULL) {
        cerrar_memoria_compartida(stats_global);
        printf("La memoria compartida ha sido cerrada.\n");
    }

    enviar_log("Sistema finalizado \n");
    printf("Monitor / Sistema finalizado \n");
    exit (0);
}


int main (int argc, char *argv[]){
    if (argc !=3){
        printf("No se puede sincronizar, Usar %s <directorio_a_sincronizar><directorio_destino>\n", argv[0]);
        return 1;

    }
    signal(SIGINT, manejador_signit);
    
iniciar_logger_demonio();
enviar_log("SISTEMA INICIADO.");

stats_global = inicializar_memoria_compartida();
int tuberia[2];
if (pipe(tuberia) == -1) {
    perror("Error al crear la tubería");
    return 1;
}
    char*dir_origen = argv[1];
    char*dir_destino = argv[2];
    printf("Mini Sistema de Sincronizacion de Archivos(Pool de %d Workers)\n", NUM_WORKERS);
    printf("Directorio origen: %s\n", dir_origen);
    printf("Directorio destino: %s\n", dir_destino);
    printf("___________________________________\n");
    
for (int i = 0; i < NUM_WORKERS; i++){
    pid_t pid = fork();
    if (pid == -1){
        perror("Error al crear el proceso hijo");
        exit(1);
    }
    if (pid == 0){
        close(tuberia[1]);
        iniciar_worker(tuberia[0], stats_global);
        exit(0);
    } else {
        worker_pids[i] = pid;
        printf("Worker %d creado con PID: %d\n", i, pid); 
    }
}
close (tuberia[0]);
printf("Monitor / Iniciando escaneo de directorio: %d\n", getpid());

while(1){
    sincronizar_directorios(dir_origen, dir_destino, tuberia[1]);
    sleep(5); 
    }
    return 0;
}