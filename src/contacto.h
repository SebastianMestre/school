#ifndef CONTACTO_H
#define CONTACTO_H

struct _Contacto {
	char* nombre;
	char* apellido;
	unsigned edad;
	char* telefono;
};
typedef struct _Contacto* Contacto;

#endif // CONTACTO_H
