import copy
import random

import src.archivos as archivos

def palabra_entra_en_tablero(dimension, palabra, x0, y0, dx, dy):
	x1 = x0 + (len(palabra) - 1) * dx
	y1 = y0 + (len(palabra) - 1) * dy
	return x1 >= 0 and x1 < dimension and y1 >= 0 and y1 < dimension

def es_palindromo(palabra):
	return palabra == palabra[::-1]

def escanear(tablero, palabra, x, y, dx, dy):
	for letra in palabra:
		if tablero[y][x] != letra and tablero[y][x] != '?':
			return False
		x += dx
		y += dy
	return True

def escribir(tablero, palabra, x, y, dx, dy):
	for letra in palabra:
		tablero[y][x] = letra
		x += dx
		y += dy

def es_tablero_valido(dimension, tablero, palabras):
	for palabra, direccion in palabras:
		palabra_es_palindromo = es_palindromo(palabra)

		direcciones = [(1,0), (-1,0), (0,1), (0,-1), (1,1), (1,-1)]
		apariciones_por_direccion = [0, 0, 0, 0, 0, 0]

		for i, (dx, dy) in enumerate(direcciones):
			for x0 in range(dimension):
				for y0 in range(dimension):
					if not palabra_entra_en_tablero(dimension, palabra, x0, y0, dx, dy):
						continue
					apariciones_por_direccion[i] += escanear(tablero, palabra, x0, y0, dx, dy)

		if apariciones_por_direccion[direccion] != 1:
			return False

		if palabra_es_palindromo:
			apariciones_por_direccion[1] = 0
			apariciones_por_direccion[3] = 0

		apariciones_totales = sum(apariciones_por_direccion)

		if apariciones_totales != 1:
			return False

	return True

def recursion_rellenar_posiciones_vacias(dimension, tablero, palabras, x, y):
	# si nos fuimos por abajo, ya terminamos
	if y >= dimension:
		if es_tablero_valido(dimension, tablero, palabras):
			return tablero
		else:
			return None

	# si nos fuimos por el costado, pasamos a la siguiente fila
	if x >= dimension:
		return recursion_rellenar_posiciones_vacias(dimension, tablero, palabras, 0, y+1)

	# si la posicion actual ya esta rellena, pasamos a la siguiente
	if tablero[x][y] != '?':
		return recursion_rellenar_posiciones_vacias(dimension, tablero, palabras, x+1, y)

	opciones = list("abcdefghijklmnopqrstuvwxyz")
	random.shuffle(opciones)

	for letra in opciones:
		tablero[x][y] = letra
		resultado = recursion_rellenar_posiciones_vacias(dimension, tablero, palabras, x+1, y)
		if resultado == None:
			continue
		else:
			return resultado

	return None

def rellenar_posiciones_vacias(dimension, tablero, palabras):
	return recursion_rellenar_posiciones_vacias(dimension, tablero, palabras, 0, 0)

# recursion_sopa_de_letras : Int Tablero [PalabrasConDireccion] Int -> (Tablero | None)
# Toma un tablero incompleto y le agrega las palabras en la lista dada, a partir
# del indice dado.
# Una vez hecho esto, rellena los lugares vacios con caracteres aleatorios,
# evitando repetir palabras de la sopa de letras.
# Si esto no es posible, devuelve None
#
# Terminacion:
# En cada llamada recursiva sumamos 1 a indice, hasta que alcanza la longitud de
# la lista de palabras, por lo que la profundidad de la recursion esta acotada.
# Dentro de la funcion, iteramos hasta dimension dos veces, de forma anidada, y
# dentro de esta iteracion, iteramos por cada letra de una palabra. Todas estas
# cosas estan acotadas, por lo que la iteracion en si esta acotada.
# Entonces, la funcion termina.
def recursion_sopa_de_letras(dimension, tablero, palabras, indice):
	if indice >= len(palabras):
		# esto significa que logramos posicionar todas las palabras. Ahora
		# pasamos a rellenar las posiciones vacias.
		return rellenar_posiciones_vacias(dimension, tablero, palabras)

	# Estas listas indican la diferencia de posicion entre letras consecutivas
	# de una palabra en la sopa de letras, dependiendo de la direccion
	# (arriba es `y negativo`, abajo es `y positivo`)
	# (izquierda es `x negativo`, derecha es `x positivo`)
	direcciones = [(1,0), (-1,0), (0,1), (0,-1), (1,1), (1,-1)]

	(palabra, direccion) = palabras[indice]

	# probamos todas las posiciones iniciales para una palabra
	for x0 in range(dimension):
		for y0 in range(dimension):

			dx, dy = direcciones[direccion]

			# Si la palabra se sale del tablero, no la podemos poner en este lugar
			if not palabra_entra_en_tablero(dimension, palabra, x0, y0, dx, dy):
				continue

			# ahora, vamos a intentar escribir la palabra sobre el tablero.

			# primero copiamos el tablero para evitar sobreescribir los datos
			# que vamos a necesitar al intentar usar otra posicion inicial
			mi_tablero = copy.deepcopy(tablero)

			# vemos si es posible poner la palabra en el tablero
			posible = escanear(mi_tablero, palabra, x0, y0, dx, dy)

			# si vemos que la posicion que elegimos no es viable, pasamos a la
			# siguiente posibilidad
			if not posible:
				continue

			# si resulta ser viable, ponemos la palabra sobre el tablero, y
			# pasamos a la siguiente palabra. (haciendo recursion en indice+1)
			escribir(mi_tablero, palabra, x0, y0, dx, dy)
			resultado = recursion_sopa_de_letras(dimension, mi_tablero, palabras, indice+1)

			# Si recibimos un tablero, entonces es posible armar una sopa completa
			if resultado != None:
				return resultado

	return None

def generar_sopa_de_letras(dimension, palabras):

	sopa = []
	for i in range(dimension):
		sopa.append(['?'] * dimension)

	return recursion_sopa_de_letras(dimension, sopa, palabras, 0)

def validar_entrada(lemario, palabras):
	return palabras

def main(camino_lemario, camino_entrada, camino_salida):
	lemario = archivos.leer_lineas(camino_lemario, 'latin1')
	(dimension, palabras) = archivos.leer_entrada(camino_entrada)
	palabras_validas = validar_entrada(lemario, palabras)
	sopa_de_letras = generar_sopa_de_letras(dimension, palabras_validas)
	archivos.escribir_sopa_de_letras(camino_salida, sopa_de_letras)
