import random
import sys

import archivos
from ahorcado_v1 import jugar


# () -> String
def pedir_nombre():
	nombre = input("Escriba su nombre...")
	return nombre

# [String] RegistroJugador -> String
def elegir_palabra(palabras, registro):
	palabra = random.choice(palabras)
	while palabra in registro:
		palabra = random.choice(palabras)
	return palabra

# RegistroJugador String ResultadoPartida -> None
# Muta el registro
def actualizar_registro(registro, palabra, resultado_del_juego):
	registro[palabra] = resultado_del_juego

# [String] RegistroCompleto -> None
# Muta el registro
def partida(palabras, registro):

	nombre = pedir_nombre()
	if nombre not in registro:
		registro[nombre] = dict()

	palabra = elegir_palabra(palabras, registro[nombre])
	resultado_del_juego = jugar(palabra)
	actualizar_registro(registro[nombre], palabra, resultado_del_juego)

# Archivo Archivo -> None
# Hace E/S con archivos
def main(camino_al_lemario, camino_al_registro):
	lemario = archivos.leer_lemario(camino_al_lemario)
	registro = archivos.leer_registro(camino_al_registro)
	decision = input("Quiere jugar? (si/*)...")
	if decision == "si":
		partida(lemario, registro)
		archivos.escribir_registro(camino_al_registro, registro)

main(sys.argv[1], sys.argv[2])

