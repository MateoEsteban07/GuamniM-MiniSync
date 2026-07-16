#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "logger.h"

void iniciar_logger_demonio() {
    mkfifo(FIFO_RUTA, 0666);

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error creando el proceso Logger");
        exit(1);
    }

    if (pid > 0) {
        return; 
    }
    setsid();

    FILE *archivo_log = fopen("historial_sincronizacion.log", "a");
    if (archivo_log == NULL) exit(1);

    int fd_fifo = open(FIFO_RUTA, O_RDONLY);
    char buffer[1024];

    while (1) {
    ssize_t bytes = read(fd_fifo, buffer, sizeof(buffer) - 1);
        
    if (bytes > 0) {
    buffer[bytes] = '\0'; 
            
        time_t ahora = time(NULL);
        struct tm *t = localtime(&ahora);
            
            fprintf(archivo_log, "[%02d-%02d-%d %02d:%02d:%02d] %s\n",
                    t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                    t->tm_hour, t->tm_min, t->tm_sec, buffer);
            
            fflush(archivo_log); 
        } else if (bytes == 0) {

            close(fd_fifo);
            fd_fifo = open(FIFO_RUTA, O_RDONLY);
        }
    }
}

void enviar_log(const char *mensaje) {
    int fd_fifo = open(FIFO_RUTA, O_WRONLY | O_NONBLOCK);
    if (fd_fifo != -1) {
        write(fd_fifo, mensaje, strlen(mensaje));
        close(fd_fifo);
    }
}