#include "expresion_infija.h"

#include "../tabla_ops.h"

typedef struct Termino Termino;
typedef struct Simbolo Simbolo;
typedef enum {
  S_TERMINO,
  S_OPERADOR,
  S_NUMERO 
} SimboloTag;


// termino matematico independiente
// tiene una operacion principal y una lista de simbolos matematicos
struct Termino {
    EntradaTablaOps* op;
    Simbolo* inicio;
    Simbolo* final;
};

// simbolo matematico que, depende de su etiqueta, representa
// un caracter (numero, operador) o un termino matematico
// tiene una referencia al proximo simbolo en la expresion.
struct Simbolo {
    SimboloTag tag; // etiqueta
    char const* operador; // operador
    int valor; // si es numero, su valor. si es operador, su largo
    Termino* termino; // termino matematico
    Simbolo* sig; // simbolo siguiente
};

