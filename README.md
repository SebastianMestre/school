
# Convenciones

A lo largo del trabajo, estableci estas normas para aportar consistencia al
codigo:

- Las funciones documentan si toman ownership sobre alguno de sus argumentos.
- Si no hay documentacion, se debe suponer que NO toman ownership.
- Si es necesario, generar copias de valores que se pasan como argumento, es
responsabilidad de quien llama.
- Se considera que cualquier objeto se puede 'mover' copiando los bits a un nuevo
lugar, y no volviendo a usar el objeto original.
- Si es posible, las estructuras se toman por valor.

# Patrones

Hay ciertos patrones que se repiten varias veces en el trabajo. Estos se
describen a continuacion:

## `Span` y `SPANOF`

Un Span es un par de punteros que indican un bloque de memoria contigua.
(`src/span.h`)

`SPANOF` es un macro, que evalua a un span que cubre el espacio de memoria de su
argumento. El argumento debe ser un lvalue.

```c
int x = 100;
Span xmem = SPANOF(x);
```

## `Destructor` y `Comparator`

`Destructor` y `Comparator` son punteros a funcion, aumentados con un argumento
extra, que acompanna al puntero mismo. (`src/types.h`)

En muchos casos, al usar punteros a funciones, sirve tener alguna informacion
adicional que acompanna a la funcion. Por ejemplo, si se quiere trabajar con
indices sobre un array, en vez de punteros a los elementos del array, es
necesario tener disponible la direccion del array mismo.

Para satisfacer esta necesidad hice un par de tipos que constan de un puntero a
funcion, y un puntero a datos extra. Este ultimo ha de pasarse siempre que se
invoque el puntero a funcion. Para evitar errores con esto, hay un par de
funciones que ayudan a llamar correctamente (`call_cmp`, `call_dtor`).

En particular, usamos estas estructuras para especificar comportamiento a las
estructuras de datos genericas que son parte de este trabajo.

## Copia de bits

las estructuras de datos del trabajo todas guardan sus datos directo en la
estructura, y no indirectamente, a traves de un puntero.

Por ejemplo, si se quiere guardar en un `Vector` una estructura `Punto`, definida
como:

```c
struct _Punto {
	double x;
	double y;
};
typedef struct _Punto Punto;
```

Se puede escribir este codigo:

```c
Vector v = vector_create(sizeof(Punto));

Punto p1 = {2, 3};
vector_push(&v, SPANOF(p1));

Punto p2 = {30, 40};
vector_push(&v, SPANOF(p2));
```

Y el layout en memoria del `Vector` va a ser:

```
Stack:

fp -  0 { v.data.begin    = 0xXXXX }
fp -  8 { v.data.end      = 0xYYYY }
fp - 16 { v.element_width = 16     }
fp - 24 { v.size          = 2      }
fp - 32 { v.capacity      = 4      }

Heap:

0xXXXX +  0 { .x = 2.0  }
0xXXXX +  8 { .y = 3.0  }
0xXXXX + 16 { .x = 30.0 }
0xXXXX + 24 { .y = 40.0 }
```

Para lograr esto, cuando se va a insertar un dato en alguna estructura de datos
generica, se pasa un `Span` que apunta a los bytes que se van a insertar.
