import random

import src.archivos as archivos
import src.modelo as modelo
from src.ahorcado_v1 import jugar


# () -> String
def pedir_nombre():
	nombre = input("Escriba su nombre...")
	return nombre

# [String] RegistroJugador -> String
def elegir_palabra(palabras, registro):
	palabras_usadas = set(registro.keys())	# toma las palabras usadas del registro y las guarda en un conjunto
	palabras_tuple = tuple(palabras)	# crea una tupla con el lemario para poder elegir palabras aleatoriamente

	if palabras <= palabras_usadas:
		# si todas las palabras del lemario han sido utilizadas, se elige cualquiera y se sobreescribe el registro
		palabras_usadas = {} 

		# Solucion alternativa: el programa termina.
		# sys.exit('Todas las palabras del lemario ya han sido jugadas. El juego terminara ahora.')
	
	palabra = random.choice(palabras_tuple)
	while palabra in palabras_usadas:
		palabra = random.choice(palabras_tuple)
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
		registro[nombre] = modelo.new_jugador()

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

