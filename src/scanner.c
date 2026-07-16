#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>  
#include "scanner.h"
#include "worker.h"


#define BUFFER_TAMAÑO 4096

int copiar_archivo(const char *ruta_origen, const char *ruta_destino) {
    int fd_origen, fd_destino;
    ssize_t bytes_leidos, bytes_escritos;
    char buffer[BUFFER_TAMAÑO];

    // Open 
    fd_origen = open(ruta_origen, O_RDONLY);
    if (fd_origen == -1) {
        perror("Error al abrir archivo de origen");
        return -1;
    }
    
    fd_destino = open(ruta_destino, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_destino == -1) {
        perror("Error al crear archivo de destino");
        close(fd_origen);
        return -1;
    }

    while ((bytes_leidos = read(fd_origen, buffer, BUFFER_TAMAÑO)) > 0) {
        bytes_escritos = write(fd_destino, buffer, bytes_leidos);
        if (bytes_escritos != bytes_leidos) {
            perror("Error al escribir en el destino");
            close(fd_origen);
            close(fd_destino);
            return -1;
        }
    }

    close(fd_origen);
    close(fd_destino);

    return 0; 
}
int necesita_sincronizacion(const char *ruta_origen, const char *ruta_destino) {
    struct stat stat_origen, stat_destino;

    if (stat(ruta_origen, &stat_origen) != 0) return 0;
    if (stat(ruta_destino, &stat_destino) != 0) return 1;
    if (stat_origen.st_mtime > stat_destino.st_mtime) return 1;
    if (stat_origen.st_size != stat_destino.st_size) return 1;

    return 0;
}
void sincronizar_directorios(const char *origen, const char *destino, int pipe_escritura) {
    DIR *dir;
    struct dirent *entry;
    char ruta_origen[1024];
    char ruta_destino[1024];

    dir = opendir(origen);
    if (dir == NULL) {
        perror("Error al abrir el directorio origen");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; 
        }

        // Rutas completas para el archivo de orign y destino
        snprintf(ruta_origen, sizeof(ruta_origen), "%s/%s", origen, entry->d_name);
        snprintf(ruta_destino, sizeof(ruta_destino), "%s/%s", destino, entry->d_name);

        if (necesita_sincronizacion(ruta_origen, ruta_destino)) {
           printf("Se detecto '%s'. Enviando por el pipe\n", entry->d_name);
            TareaSincronizacion nueva_tarea;
            strncpy(nueva_tarea.ruta_origen, ruta_origen, sizeof(nueva_tarea.ruta_origen));
            strncpy(nueva_tarea.ruta_destino, ruta_destino, sizeof(nueva_tarea.ruta_destino));
            
            write(pipe_escritura, &nueva_tarea, sizeof(TareaSincronizacion));

        }
    }
    closedir(dir);
}