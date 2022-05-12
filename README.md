# Aclaraciones Sobre Apartado 1

El algoritmo está copiado de las slides, con un par de `mfence`s agregados.

El problema que hay para implementar una libreria es que el algoritmo de lamport necesita saber la cantidad de threads de antemano. Aparte, no se puede implementar la interfaz de `pthread_mutex` porque este algoritmo necesita que cada thread pase un id.

# Aclaraciones Sobre Apartado 3

Para testear los locks se puede compilar y correr `readers_writers.c`. La variable del preprocesador `MODE` determina cuál lock se va a usar.

- `MODE=0` - lock read-preferring
- `MODE=1` - lock write-preferring
- `MODE=2` - lock que preserva el orden

Por ejemplo, para compilar con el lock justo, se puede escribir el siguiente comando

```shell
gcc -DMODE=2 rlock.c wlock.c rwlock.c queue.c readers_writers.c -pthread -o readers_writers
```
