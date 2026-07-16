#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include "scanner.h"
#include "worker.h"
#include "ipc.h"

//Función para validar ruta 
int main (int argc, char *argv[]){
    if (argc !=3){
        printf("No se puede sincronizar, Usar %s <directorio_a_sincronizar><directorio_destino>\n", argv[0]);
        return 1;

    }

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
        printf("Monitor de directorios iniciado. | Monitor PID: %d | Worker PID: %d\n", getpid(), pid);
        while (1){
            sincronizar_directorios(dir_origen, dir_destino, tuberia[1]);
            sleep(5);
        }
    }
    return 0;
}




   /*  DIR *dir; // puntero de directorio
    struct dirent *entry; //puntero a la estructura de entrada del directorio
    //System call
    dir = opendir(dir_origen);

    if (dir == NULL){
        perror("Error al abrir el directorio de origen");
        return 1; 
    }
    printf("Files de '%s':\n", dir_origen);
    printf("______________________________________________\n");
   
    struct stat file_stat;
    char ruta[1024]; // guardar ruta temporal
   
    while ((entry = readdir (dir)) != NULL){
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") == 0){
        continue;
        }
            snprintf(ruta, sizeof(ruta), "%s/%s", dir_origen, entry->d_name);
            snprintf(ruta, sizeof(ruta), "%s/%s", dir_origen, entry->d_name);
            if (stat(ruta, &file_stat) == 0){
                printf("Archivo: %-15s | Tamaño: %6ld | bytes\n I-nodo: %lu\n", entry->d_name, file_stat.st_size, file_stat.st_ino);
            } else {
                perror("Error al obtener información del archivo");
            }
            printf("______________________________________________\n"); */