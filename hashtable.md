
Este documento describe el funcionamiento de la estructura de datos usada para
almacenar los pares 'clave valor'. Las operaciones soportada son las siguientes:

- `get(clave)`        - devuelve una copia del valor asociado a la clave
- `put(clave, valor)` - asocia el valor con la clave (reemplaza si ya existe)
- `del(clave)`        - elimina la asociacion de la clave
- `take(clave)`       - elimina y devuelve el valor asociado a la clave atomicamente
- `evict()`           - elimina una entrada

Internamente se mantiene una lista de las entradas en orden LRU: tras el uso de
una entrada, esa entrada se mueve al principio. Un uso de una entrada consiste de
una operacion `get` o `put` con la clave de esa entrada. Si es exitoso, `evict`
siempre elimina una de las primeras 10 entradas en esa lista. Este valor está
definido mediante una constante del preprocesador y se puede modificar sin
problemas.

La estructura es una tabla hash con encadenamiento. Cada fila de la tabla tiene
un lock (mutex). Al operar sobre una fila, se toma su lock.

La lista LRU también tiene un lock (mutex) que se toma al operar sobre ella.

Tanto los nodos de las listas de probing como los nodos de la lista LRU se
guardan de forma intrusiva sobre una sola estructura, llamada `nodo`. Esta
también contiene los datos (clave y valor) de una entrada.
