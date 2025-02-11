# EDyA I -- TP1

# Caracteristicas del proyecto.
### Estructura del proyecto.
- Dentro del directorio raiz encontramos un archivo **Makefile**
    encargado de compilar el proyecto, 
    archivos referentes al **[testeo](#tests)** del programa,
    y este mismo **README**.
- Dentro del subdirectorio **src** encontramos al **main** junto a archivos
    fuente destinados a manejar el nivel mas superficial del programa, donde 
    ingresamos los operadores correspondientes y damos comienzo a la parte
    interactiva.
- Dentro de **src** tenemos el subdirectorio **interpretar**, dentro del cual
    se encuentran los archivos fuente correspondientes a la parte interactiva del
    programa. El archivo **interpretar.c** inicia la sesion con el usuario y utiliza
    **parsear.c** para interpretar el input ingresado.
### Caracteristicas relevantes.
- A la carga de operadores se le añade un parametro adicional correspondiente a la
  **precedencia** de la operacion ingresada. Su funcionalidad responde a flexibilizar el comando
  `imprimir`. Tambien nos da la posibilidad de formular una expresion infija con operaciones arbitrarias.
- A la hora de cargar un alias, existe la posibilidad de ingresar un alias distinto como valor.
  A la hora de evaluar, se eleva un error si este alias no esta definido.
  En la impresion se prevee esta eventualidad y se adecua la forma de la expresion final.
- Para un mejor funcionamiento del programa, no esta permitido ingresar expresiones vacias.
  El alias sera descartado y se elevara un error.
- Si se carga un alias ya existente, este se reescribira y la expresion anterior sera descartada.
- El programa unicamente terminara cuando se ingrese el comando `salir`. Los errores detectados seran
  informados y se permitira continuar con la ejecucion del programa.
    


# Compilado y ejecucion.
Para compilar el programa basta correr `make` sobre el directorio raiz.
Esto dara como output un subdirectorio `build/`, con archivos intermediarios
para el compilado, y un ejecutable `interprete`. Para limpiar el output basta
correr `make clean`. 

### Ejemplo de ejecucion.

```shell
$ make
$ ./interpretar
> a = cargar 2 2 +
> evaluar a
4
> imprimir a
2 + 2
> salir
$ make clean
```


# Tests. <a name = tests></a>

En la carpeta `tests/` hay algunos archivos llamados `test*` y `memory_test*`.
Estos son inputs que se pueden dar el interprete por stdin.

Los llamados `test*` van acompannados de su correspondiente archivo `salida*`.
Este contiene la respuesta que debe dar el interprete al interpretar el test.

Los llamados `memory_test*` no tienen una salida asociada, existen unicamente
para verificar el manejo de memoria. Para esto, se puede utilizar Valgrind.

Para correr los tests, y verificar que producen la salida correcta, se puede usar
el script `run_tests.sh`/.

Para correr los tests de memoria en valgrind, se puede usar
`run_memory_tests.sh`. Esto tambien corre los otros tests bajo Valgrind.

> Notar que se debe tener instalado Valgrind y un shell UNIX-compatible.
> Aparte, se debe compilar el programa previamente para poder correr los tests.

- - - -
# Informe de dificultades.
Nuestra principal dificultad fue lograr organizarnos bien como grupo. Los primeros
dias no tuvimos una dinamica muy fluida y nos falto tiempo dedicado a discutir y 
planear mejor el proyecyo. Decididamente, una mayor coordinacion entre nosotros 
y un acuerdo mutuo en la direccion en la que ibamos a llevar el trabajo habrian 
resultado en menores dificultades para completar la tarea. 
