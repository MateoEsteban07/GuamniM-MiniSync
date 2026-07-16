#ifndef WORKER_H
#define WORKER_H

#include "ipc.h"

typedef struct {
	char ruta_origen[1024];
	char ruta_destino[1024];
} TareaSincronizacion;

void iniciar_worker(int pipe_lectura, stats_t *stats);

#endif