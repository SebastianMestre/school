# Trabajo practico final

## Instrucciones de uso

Pasos:

 - Abrir una consola en la raiz del proyecto
 - Compilar la parte en C: `cc parte1/src/main.c -o filtrar` (esto produce el archivo ejecutable `filtrar`)
 - Ejecutar la parte en C: `./filtrar` (este programa interactivo produce el archivo de texto `salida`)
 - Ejecutar la parte en Python: `python3 parte2/__main__.py parte2/lemario.txt salida sopa` (esto produce el archivo de texto `sopa`)

Alternativamente, usando Linux o WSL:

 - Abrir una consola en la raiz del proyecto
 - Ejecutar el resto de los pasos: `make` (la entrada al programa en C se toma desde el archivo `parte1/entrada.txt`, no de forma interactiva)

## Programa en C

### Instrucciones

El programa no toma argumentos por linea de comando.

Al iniciar, lee un entero por entrada estandar (la dimension).

Luego, toma palabras seguidas de un entero, hasta que se ingresa la cadena 'BASTA'.

Finalmente, escribe la salida al archivo `salida`

### Tests

Los tests se encuentran en el subdirectorio `parte1/test/`. Para correrlos, se
deben compilar usando `cd parte1/test; cc main.c ../src/implementacion.c -o tests`
(O simplemente `cd parte1/test; make` si se esta en Linux o WSL). Luego, se
pueden correr con `./tests`

## Programa en Python

### Instrucciones

El programa toma 3 (tres) argumentos por linea de comando. Estos son: camino al
lemario, camino a la entrada (la salida del programa anterior), camino a la
salida (alli se guardara la sopa de letras)

Lee la informacion necesaria y luego escribe la sopa de letras al archivo indicado.

(se puede encontrar informacion sobre el modelo de datos en `parte2/modelo.txt`)

### Tests

los tests se encuentran en el subdirectorio `parte2/test/`. Para correr los
tests, se debe correr PyTest en el subdirectorio `parte2/`. (`cd parte2; python3 -m pytest`)
