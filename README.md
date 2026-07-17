# MiniSync - Sistema de Sincronización de Archivos
Mateo Esteban Guamaní  Barahona  

MiniSync es un programa desarrollado en C para sistemas Linux. Funciona como un motor de sincronización unidireccional parecido a un sistema de almacenamiento como Google Drive y otros parecidos, manteniendo una carpeta de respaldo (Destino) como un reflejo de una carpeta de trabajo (Origen).

## Características Principales
Gestión de Procesos: Se utiliza la llamada fork() para instanciar un "Pool" de 4 procesos Workers y desvincula un proceso Demonio independiente "Logger". Además, gestiona su ciclo de vida interceptando señales "SIGINT" para un apagado correcto.

Gestión de Entrada/Salida (E/S): Se optimiza las transferencias físicas en el disco repartiendo la carga de lectura/escritura entre los Workers paralelos, operando a bajo nivel con llamadas read() y write().

Gestión de Metadatos de Archivos (i-nodos): Utiliza la system call stat() para inspeccionar directamente los i-nodos de los archivos. El cual va a tomar decisiones de sincronización comparando únicamente tamaños y fechas de modificación, sin necesidad de abrir su contenido.

Recorrido Recursivo de Directorios: Explora automáticamente por árboles de directorios y subcarpetas (dirent.h), replicando la estructura en el destino y detectando eliminaciones (unlink, rmdir) para mantener un espejo estricto.

Comunicación Inter-Procesos (IPC): Emplea Tuberías sin nombres (pipes) para la distribución de tareas desde el proceso Monitor hacia los Workers, y Tuberías con nombres (FIFOs) para la transmisión asíncrona de eventos al Logger.

Sincronización y Memoria Compartida: Proyecta un bloque de memoria RAM en el espacio de direcciones virtuales de los procesos (shm_open, mmap) para mantener estadísticas globales. La exclusión mutua se garantiza a tráves de Semáforos POSIX para evitar condiciones de carrera.

## Estructura del Proyecto

Para mantener todo organizado y evitar el código acoplado, el sistema está dividido en varios archivos modulares según su responsabilidad:

monitor.c: Inicializa la memoria, crea a los trabajadores, lanza el Logger y se queda en un bucle infinito vigilando. También atrapa el Ctrl+C para el apagado seguro.

scanner.c: Contiene la lógica recursiva. Entra a las carpetas, lee los metadatos (stat) y decide si un archivo debe copiarse, ignorarse o eliminarse.

worker.c : Contiene el código de los procesos paralelos. Están dormidos hasta que reciben una ruta por la tubería, entonces la copian en bloques usando read() y write().

ipc.c (Memoria y Sincronización): Maneja la Memoria Compartida y los Semáforos para que los Workers no choquen entre sí al actualizar las estadísticas globales.

logger.c: El proceso Deamon. Se desvincula del sistema y solo se dedica a escuchar su propia tubería para escribir todo lo que sucede en un archivo .log.

cabeceras/ (*.h): Contiene los archivos de cabecera que definen las estructuras y sirven como "contrato" para conectar los archivos .c entre sí.



## Requisitos del Sistema

Un entorno Linux o a su vez Windows Subsystem for Linux (WSL).

Compilador gcc.

Herramienta make.

## Compilación

El proyecto incluye un Makefile para facilitar la ejecución. Simplemente se abre la terminal en la raíz del proyecto y se ejecuta:

```make clean```

```make```

Esto creara el ejecutable dentro de la carpeta bin.

## Instrucciones de Ejecución

Para iniciar el monitor de sincronización, se ejecuta el binario pasando como argumentos la ruta del directorio de origen y el de destino:

```./bin/minisync <directorio_origen> <directorio_destino>```


Ejemplo:

```./bin/minisync ../OperativeSystems ../directoriosincro```


## Comportamiento Esperado:

El programa iniciará el Pool de Workers y quedará vigilando en segundo plano.

Si se crea, modifica o se elimina algo en la carpeta de origen, el programa reaccionará en los siguientes 5 segundos. 


El historial de acciones se guardará en historial_sincronizacion.log.

## Detener el programa

Dado que es un ciclo infinito diseñado para correr en segundo plano, se debe detenerlo manualmente presionando:
```Ctrl + C```
El manejador de señales atrapará el comando, cerrará los procesos de forma segura y liberará la memoria.
