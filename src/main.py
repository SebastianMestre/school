import random

import src.archivos as archivos
import src.modelo as modelo
from src.ahorcado_v1 import jugar


# () -> String
# Pide el nombre del jugador por consola
def pedir_nombre():
	nombre = input("Escriba su nombre...")
	return nombre

# Set(String) RegistroJugador -> String
# Toma una lista de palabras y un registro de jugador, y elije una palabra no jugada.
# En caso de haberse utilizado todas las palabras, eleva un ValueError
def elegir_palabra(palabras, registro):
	palabras_usadas = set(registro.keys())	# toma las palabras usadas del registro y las guarda en un conjunto
	palabras_tuple = tuple(palabras)	# crea una tupla con el lemario para poder elegir palabras aleatoriamente

	# si todas las palabras del lemario han sido utilizadas, elevamos un error
	if palabras <= palabras_usadas:
		raise ValueError
		# Solucion alternativa: el programa termina.
		# sys.exit('Todas las palabras del lemario ya han sido jugadas. El juego terminara ahora.')

	palabra = random.choice(palabras_tuple)
	while palabra in palabras_usadas:
		palabra = random.choice(palabras_tuple)
	return palabra

# String Set(String) RegistroCompleto -> String
# Prepara el registro apropiadamente, y garantiza la eleccion de una palabra para jugar
# Asume que la lista de palabras no esta vacia
def preparar_partida(nombre, palabras, registro):
	if len(palabras) == 0:
		raise ValueError

	if nombre not in registro:
		registro[nombre] = modelo.new_jugador()

	try:
		palabra = elegir_palabra(palabras, registro[nombre])
	except ValueError:
		#  No quedan palabras por jugar: vaciamos el registro del jugador y
		# volvemos a elegir una palabra
		#   Notar que, como la lista de palabras no esta vacia, elegir_palabra
		# siempre devuelve una palabra despues de limpiar el registro del jugador
		registro[nombre] = modelo.new_jugador()
		palabra = elegir_palabra(palabras, registro[jugador])
	
	return palabra

# RegistroJugador String ResultadoPartida -> None
# Muta el registro: agrega nueva partida al registro del jugador
def actualizar_registro(registro, palabra, resultado_del_juego):
	registro[palabra] = resultado_del_juego


# String Set(String) RegistroCompleto -> None
# Muta el registro: da comienzo a una partida y inicializa/actualiza el registro del jugador
def partida(nombre, palabras, registro):
	palabra = preparar_partida(nombre, palabras, registro)
	resultado_del_juego = jugar(palabra)
	actualizar_registro(registro[nombre], palabra, resultado_del_juego)

# Archivo Archivo -> None
# Funcion principal del juego. Hace E/S salida con archivos, toma input del jugador por consola
# y da comienzo al juego
def main(camino_al_lemario, camino_al_registro):
	lemario = archivos.leer_lemario(camino_al_lemario)
	registro = archivos.leer_registro(camino_al_registro)
	decision = input("Quiere jugar? (si/*)...")
	if decision == "si":
		nombre = pedir_nombre()
		partida(nombre, lemario, registro)
		archivos.escribir_registro(camino_al_registro, registro)

