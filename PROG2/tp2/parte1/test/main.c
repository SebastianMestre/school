#include "../src/implementacion.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// stralloc : String -> String
// copia el string dado a un nuevo buffer en el heap
char* stralloc(char const* str) {
	char* buffer = malloc(strlen(str) + 1);
	strcpy(buffer, str);
	return buffer;
}

int main () {

	{
		printf("TESTS DE STRINGS...");

		{
			char str[] = "YARE YARE DAZE";
			hacer_minuscula(str);
			assert(strcmp(str, "yare yare daze") == 0);
		}

		{
			char str[] = "Hola Amigos De YouTube";
			hacer_minuscula(str);
			assert(strcmp(str, "hola amigos de youtube") == 0);

		}

		assert(palabras_iguales("hola", "hola"));
		assert(palabras_iguales("aloh", "hola"));
		assert(palabras_iguales("hola", "aloh"));
		assert(palabras_iguales("HOLA", "aloh"));
		assert(!palabras_iguales("Sebastian", "Mestre"));
		assert(!palabras_iguales("Matematica", "Completa"));
		printf("EXITO.\n");
	}

	{
		printf("TESTS DE LISTA...");
		struct lista* palabras = NULL;
		lista_insertar(&palabras, (struct palabra_con_direccion){stralloc("iknom"), 0});
		assert(lista_contiene_palabra(palabras, "MONKI"));
		assert(!lista_contiene_palabra(palabras, "FLIP"));
		lista_insertar(&palabras, (struct palabra_con_direccion){stralloc("flip"), 0});
		assert(lista_contiene_palabra(palabras, "PILF"));
		lista_liberar(palabras);
		printf("EXITO.\n");
	}

}
