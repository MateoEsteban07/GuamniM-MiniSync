#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "worker.h"
#include "scanner.h" // Para poder llamar a la función copiar_archivo()

void iniciar_worker(int pipe_lectura, stats_t *stats) {
    TareaSincronizacion  tarea;

    // Bucle infinito para recibir las tareas del monitor
    // Si no hay nada en la tubería, se duerme al worker hasta que llegue algo.
    while (read(pipe_lectura, &tarea, sizeof(TareaSincronizacion)) > 0) {
        printf("Worker PID: %d / Tarea . Copiando: %s\n", getpid(), tarea.ruta_origen);
        
        int resultado = copiar_archivo(tarea.ruta_origen, tarea.ruta_destino);
        // Memoria Compartida
        sem_wait(&stats->semaforo); //Semaforo rojo
        
        if (resultado == 0) {
            stats->archivos_copiados++;
            printf("Copia realizada. Total archivos copiados globalmente: %ld\n", stats->archivos_copiados);
        } else {
            stats->errores++;
        }

        sem_post(&stats->semaforo); //Proceso terminado 
    }

    
    printf("Worker PID: %d Tubería cerrada. Terminando proceso.\n", getpid());
    exit(0);
}