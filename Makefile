
interprete: build/main.o build/interpretar.o build/tabla_ops.o build/operadores.o build/expresion.o
	gcc -o $@ $^

clean:
	rm -rf build/
	rm -f interprete
.PHONY: clean

build/main.o:        src/main.c src/interpretar.h src/tabla_ops.h src/funcion_evaluacion.h src/operadores.h
build/interpretar.o: src/interpretar.c src/interpretar.h src/tabla_ops.h src/funcion_evaluacion.h
build/tabla_ops.o:   src/tabla_ops.c src/tabla_ops.h src/funcion_evaluacion.h
build/operadores.o:  src/operadores.c src/operadores.h
build/expresion.o:   src/expresion.c src/expresion.h

build/%.o: src/%.c
	mkdir -p build
	gcc -c -o $@ $<
