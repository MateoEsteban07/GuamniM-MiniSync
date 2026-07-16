#ifndef LOGGER_H
#define LOGGER_H

#define FIFO_RUTA "/tmp/minisync_fifo"

void iniciar_logger_demonio();

void enviar_log(const char *mensaje);

#endif