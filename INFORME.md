# Dificultades

Me resulto problematico el tener que referirse al mismo contacto en distintas
estructuras de datos. En particular, mantenerlos sincronizados y hacer las copias
y destrucciones necesarias me parecio muy complejo.

Para evitar estos problemas, tome estas medidas:

- Centralizar el almacenamiento de los contactos en una sola estructura, lo cual
simplifico el manejo de memoria significativamente.

- Usar handles numericos en vez de punteros para referirse a los contactos, lo
cual da mas flexibilidad con como se puede implementar el almacenamiento.

- Interpretar la modificacion de un contacto como su borrado, seguido de la
creacion de un contacto nuevo, con los cambios solicitados.

De estos puntos salen otras consecuencias:

- Los contactos pueden ser inmutables una vez que se crean.

- Es posible tener una representacion unica para las operaciones (un par de
handles opcionales)

# Modulos

- io: helpers para entrada/salida por consola
- string: helpers para el uso de strings
- types: tipos helpers, por ejemplo para simplificar el uso de punteros a funcion
- contact: la definicion del tipo de dato Contact
- history: un contenedor de operaciones, se usa para implementar deshacer/rehacer
- index: una estructura de datos para la busqueda eficiente por nombre y apellido
- database: un objeto que maneja las operaciones y el deshacer/rehacer
- storage: un contenedor de contactos. Maneja la memoria de cada contacto.
- estructuras de datos: span, vector, circular\_buffer, bst, slot\_map

# Estructuras de datos

## Span: un puntero a un bloque de memoria contigua

## Vector: un array dinamico

## CircularBuffer: una cola circular

## Bst: un arbol AVL

## SlotMap: un diccionario, con claves autogeneradas

SlotMap esta implementado con tres vectores.

Storage es un almacenamiento de contactos, en el cual pueden existir repetidos, por lo que no el nombre y apellido no es suficiente para identificar un contacto de manera unica.

# Algoritmos


## deshacer/rehacer

Use una cola circular, que guarda las operaciones que se realizaron. Al deshacer,
se ejecuta la operacion inversa.

Si la operacion fue de creacion, se ejecuta el borrado, y viceversa.

## guardar ordenado

Use quicksort sobre un array.

## buscar por suma de edades

Use programacion dinamica. El problema es muy parecido al conocido 'subset sum'.

# Compilacion/Invocacion

# Bibliografia

Para la implementacion del AVL, me base en la implementacion de la Catedra,
algunas observaciones personales, y [1]. Como referencia, use las manpages de
Ubuntu y [2]. Aparte, tome varias ideas de estilo de [3].

[1] https://www.geeksforgeeks.org/avl-tree-set-2-deletion/
[2] https://en.cppreference.com/
[3] https://floooh.github.io/2019/09/27/modern-c-for-cpp-peeps
