#ifndef SCANNER_H
#define SCANNER_H

void sincronizar_directorios(const char *origen, const char *destino, int pipe_escritura);


int necesita_sincronizacion(const char *ruta_origen, const char *ruta_destino);


int copiar_archivo(const char *ruta_origen, const char *ruta_destino);

#endif