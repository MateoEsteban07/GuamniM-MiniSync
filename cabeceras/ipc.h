#ifndef IPC_H
#define IPC_H

#include <semaphore.h> 

typedef struct {
    long archivos_copiados;
    long bytes_copiados;
    long errores;
    sem_t semaforo; //Semáforo para sincronización
} stats_t;

#define SHM_NAME "/minisync_shm"

stats_t* inicializar_memoria_compartida();
void cerrar_memoria_compartida(stats_t *stats);

#endif