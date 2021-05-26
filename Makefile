
interprete: build/main.o build/interpretar.o build/tabla_ops.o build/operadores.o
	gcc -o $@ $^

clean:
	rm -rf build/
	rm -f interprete
.PHONY: clean

build/main.o:        src/main.c
build/interpretar.o: src/interpretar.c src/interpretar.h src/tabla_ops.h
build/tabla_ops.o:   src/tabla_ops.c src/tabla_ops.h
build/operadores.o:  src/operadores.c src/operadores.h

build/%.o: src/%.c
	mkdir -p build
	gcc -c -o $@ $<
