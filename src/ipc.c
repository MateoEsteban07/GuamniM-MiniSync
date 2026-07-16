#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      
#include <sys/mman.h>   
#include <unistd.h>     
#include "ipc.h"

stats_t* inicializar_memoria_compartida() {
    // Espacio en la ram
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error al crear la memoria compartida (shm_open)");
        exit(1);
    }

    // Asignar un tamaño a la memoria compartida
    if (ftruncate(shm_fd, sizeof(stats_t)) == -1) {
        perror("Error al asignar tamaño a la memoria (ftruncate)");
        exit(1);
    }

    // Mapear ese espacio 
    stats_t *stats = mmap(0, sizeof(stats_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (stats == MAP_FAILED) {
        perror("Error al mapear");
        exit(1);
    }

    //Inicio en 0 
    stats->archivos_copiados = 0;
    stats->bytes_copiados = 0;
    stats->errores = 0;

    //  semforo
    // El 1 varios procesos
    // 1 final (desbloqueado)
    if (sem_init(&stats->semaforo, 1, 1) == -1) {
        perror("Error inicializando el semáforo");
        exit(1);
    }

    return stats;
}

void cerrar_memoria_compartida(stats_t *stats) {
    sem_destroy(&stats->semaforo);
    
    munmap(stats, sizeof(stats_t));
    
    shm_unlink(SHM_NAME);
}