
DIR_2 := parte2
FILES_2 := __main__.py src/main.py src/archivos.py

DIR_1 := parte1
FILES_1 := src/main.c

LEMARIO := $(DIR_2)/lemario.txt
ENTRADA := $(DIR_1)/entrada.txt

sopa: $(LEMARIO) salida $(FILES_2:%=$(DIR_2)/%)
	python3 $(DIR_2)/__main__.py $(LEMARIO) salida sopa

salida: filtrar $(ENTRADA)
	./filtrar < $(ENTRADA)

filtrar: $(FILES_1:%=$(DIR_1)/%)
	$(CC) -o $@ $^

