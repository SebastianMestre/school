
# Instrucciones

Para compilar el servidor, se puede invocar `make`

Para correr el servidor en los puertos 888 y 889, se debe ejecutar `./start`.

Para correr el servidor en los puertos 8000 y 8001, se debe ejecutar `./build/server`.

Por defecto, el límite de memoria es 1GiB.

# Lugares de interés en el programa

- `server/bind.c` implementa la reducción de privilegios
- `server/text_client.c` implementa el manejo de clientes en modo texto
- `server/biny_client.c` implementa el manejo de clientes en modo binario
- `server/hashtable.c` implementa una tabla hash concurrente
- en `server/main.c` se puede configurar el límite de memoria y el número máximo de threads

# Almacenamiento de claves y valores

Diseñamos un objeto concurrente para almacenar los pares (clave, valor). Las
operaciones soportadas son las siguientes:

- `get(clave)`        - devuelve una copia del valor asociado a la clave
- `put(clave, valor)` - asocia el valor con la clave (reemplaza si ya existe)
- `del(clave)`        - elimina la asociacion de la clave
- `take(clave)`       - elimina y devuelve el valor asociado a la clave atomicamente
- `evict()`           - elimina una entrada
- `size()`            - devuelve la cantidad de entradas que hay

La implementación puede encontrarse en `server/hashtable.c`

## Politica de desalojo

Aplicamos una politica LRU aproximada: siempre se desaloja una de las 10 ultimas
entradas en el orden LRU. Elegimos esta politica porque es fácil de entender e
implementar. Aparte, el código es corto y se puede expresar en términos de
operaciones comunes sobre listas.

## Estructura de datos

Implementamos una tabla hash con encadenamiento. Cada fila de la tabla tiene
un lock (mutex). Al operar sobre una fila, se toma su lock.

Elegimos esta estructura porque resulta sencillo trabajar en distintas filas
en paralelo.

Internamente se mantiene una lista de las entradas en orden LRU: tras el uso de
una entrada, esa entrada se mueve al principio. Un uso de una entrada consiste de
una operacion `get` o `put` con la clave de esa entrada. Si es exitoso, `evict`
siempre elimina una de las primeras 10 entradas en esa lista. Este valor está
definido mediante una constante del preprocesador y se puede modificar sin
problemas.

La lista LRU también tiene un lock (mutex) que se toma al operar sobre ella.

Tanto las listas de probing como la lista LRU se implementan con listas
doblemente circulares de forma intrusiva sobre una sola estructura, llamada
`nodo`. Esta también contiene los datos (clave y valor) de una entrada.

# Notas sobre manejo de errores

Si una operación de un cliente en modo binario produce un OOM, se responde con
el código adecuado y se patea a ese cliente.

