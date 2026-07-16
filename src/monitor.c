#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "scanner.h"
#include "worker.h"
#include "ipc.h"
#include "logger.h"

//Función para validar ruta 
int main (int argc, char *argv[]){
    if (argc !=3){
        printf("No se puede sincronizar, Usar %s <directorio_a_sincronizar><directorio_destino>\n", argv[0]);
        return 1;

    }

iniciar_logger_demonio();
enviar_log("SISTEMA INICIADO: Comienzo de monitorización.");

stats_t *stats = inicializar_memoria_compartida();
int tuberia[2];
if (pipe(tuberia) == -1) {
    perror("Error al crear la tubería");
    return 1;
}
    char*dir_origen = argv[1];
    char*dir_destino = argv[2];
    printf("Mini Sistema de Sincronizacion de Archivos\n");
    printf("Directorio origen: %s\n", dir_origen);
    printf("Directorio destino: %s\n", dir_destino);
    printf("\n");
    //forks
    pid_t pid = fork();
    if (pid == -1){
        perror("Error al crear el proceso hijo");
        exit(1);
    }
    if (pid == 0){
        
        close(tuberia[1]); // Cerrar escritura
        iniciar_worker(tuberia[0], stats);
        exit(0);
    } else {
        close (tuberia[0]);
        printf("Monitor de directorio iniciado. | Monitor PID: %d | Worker PID: %d\n", getpid(), pid);
        while (1){
            sincronizar_directorios(dir_origen, dir_destino, tuberia[1]);
            sleep(5);
        }
    }
    return 0;
}




   