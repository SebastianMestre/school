#include "expresion_infija.h"

#include "../tabla_ops.h"

#include <stdlib.h>

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
    EntradaTablaOps *op;
    Simbolo* inicial;
    Simbolo* final;
};

// simbolo matematico que, depende de su etiqueta, representa:
// un caracter (operador)
// un numero (operando)
// un termino matematico
// tiene una referencia al proximo simbolo en la expresion.
struct Simbolo {
    SimboloTag tag; // etiqueta
    char const* operador; // operador
    int valor; // numero
    Termino* termino; // termino matematico
    Simbolo* sig; // simbolo siguiente
};


static void limpiar_termino(Termino* termino) {
  if (termino) {
    Simbolo* it = termino->inicial;
    Simbolo* sig;
    while (it) {
      sig = it->sig;
      // no limpia operador
      limpiar_termino(it->termino);
      free(it);
      it = sig;
    }
    free(termino);
  }
}

static Termino* agrupar_en_terminos(Expresion* expresion) {
  Termino* termino = NULL;
  if (expresion) {
    termino = malloc(sizeof(*termino));
    // simbolo caracteristico de la expresion (numero o operador)
    Simbolo* simboloNuevo = malloc(sizeof(*simboloNuevo)); 
    switch (expresion->tag) {
    case X_NUMERO: {
      *simboloNuevo = (Simbolo){.tag = S_NUMERO, .valor = expresion->valor};
      *termino = (Termino){.inicial = simboloNuevo, .final = simboloNuevo};
    } break;
    case X_OPERACION: {
      *termino = (Termino){.op = expresion->op};
      Termino* args[2] = {
        // paso recursivo
        organizar_en_terminos(expresion->sub[0]),
        organizar_en_terminos(expresion->sub[1])};
      // simbolo del operador
      *simboloNuevo = (Simbolo){
        .tag = S_OPERADOR,
        .operador = expresion->op->simbolo};
      // chequeamos aridad
      if (expresion->op->aridad == 1) {
        // el operador va primero
        termino->inicial = simboloNuevo;
        Simbolo* operando;
        // si el operando tiene una operacion, y esta:
        // tiene menor precedencia a la nueva operacion
        // o es una operacion unaria
        // la encapsulamos en un termino independiente
        if (args[0]->op && expresion->op->precedencia > args[0]->op->precedencia || args[0]->op->aridad == 1) {
          operando = malloc(sizeof(*operando));
          // el simbolo sera un termino matematico
          *operando = (Simbolo){.tag = S_TERMINO, .termino = args[0]};
          termino->final = operando;   
        }
        // en otro caso, enlazamos la expresion y liberamos el termino operando
        else {
          operando = args[0]->inicial;
          termino->final = args[0]->final; 
          free(args[0]);
        }
        // enlazamos el operando luego del operador
        simboloNuevo->sig = operando;
      }
      else if (expresion->op->aridad == 2) {
        Simbolo* operandos[2];
        if (args[0]->op && expresion->op->precedencia > args[0]->op->precedencia || args[0]->op->aridad == 1) {
          *operandos[0] = (Simbolo){.tag = S_TERMINO, .termino = args[0]};
          // el operador va despues del primer operando
          operandos[0]->sig = simboloNuevo;
        }
        else {
          operandos[0] = args[0]->inicial;
          args[0]->final->sig = simboloNuevo;
          free(args[0]);
        }
        // primero el primer operando
        termino->inicial = operandos[0];
        if (args[1]->op && expresion->op->precedencia > args[1]->op->precedencia || args[1]->op->aridad == 1) {
          *operandos[1] = (Simbolo){.tag = S_TERMINO, .termino = args[1]};
          // el segundo operando va al final
          termino->final = operandos[1];
        }
        else {
          // enlazamos la expresion
          operandos[1] = args[1]->inicial;
          termino->final = args[1]->final;
          free(args[1]);
        }
        // el segundo operando va despues del operador
        simboloNuevo->sig = operandos[1];
      }
    } break;
    }
  }
  return termino;
}

void imprimir_expresion_infija(Expresion* expresion) {
  Termino* expresion_infija = agrupar_en_terminos(expresion);
  //TODO
  limpiar_termino(expresion_infija);
}