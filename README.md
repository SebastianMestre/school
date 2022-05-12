# Apartado 1

El algoritmo está copiado de las slides, con un par de `mfence`s. Cuando lo testé, hubo una vez que dió mal el resultado. Despues de eso lo corrí varias miles de veces y no pude replicar el resultado.

El problema que hay para implementar una libreria es que el algoritmo de lamport necesita saber la cantidad de threads de antemano. Aparte, no se puede implementar la interfaz de `pthread_mutex` porque este algoritmo necesita que cada thread pase un id.

# Apartado 3

Para testear los locks se puede compilar y correr `readers_writers.c`. La variable del preprocesador `MODE` determina cuál lock se va a usar.

- `MODE=1` - lock read-preferring
- `MODE=2` - lock write-preferring
- `MODE=3` - lock que preserva el orden

Por ejemplo, para compilar con el lock justo, se puede escribir el siguiente comando

```shell
gcc -DMODE=3 rlock.c wlock.c rwlock.c queue.c readers_writers.c -o readers_writers
```

# Apartado 8

# Apartado 10