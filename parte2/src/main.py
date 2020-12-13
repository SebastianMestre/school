import copy

import src.archivos as archivos

def recursion_sopa_de_letras(dimension, tablero, palabras, indice):
	if indice == len(palabras):
		return tablero

	# Estas listas indican la diferencia de posicion entre letras consecutivas
	# de una palabra en la sopa de letras, dependiendo de la direccion
	# (arriba es `y negativo`, abajo es `y positivo`)
	# (izquierda es `x negativo`, derecha es `x positivo`)
	dx = [1, -1, 0, 0, 1, 1]
	dy = [0, 0, 1, -1, 1, -1]

	(palabra, direccion) = palabras[indice]

	# probamos todas las posiciones iniciales para una palabra
	for x0 in range(dimension):
		for y0 in range(dimension):

			# calculamos la posicion final de la palabra
			x1 = x0 + (len(palabra) - 1) * dx[direccion]
			y1 = y0 + (len(palabra) - 1) * dy[direccion]

			# si la posicion final de la palabra queda fuera del tamanno de la
			# sopa de letras, no consideramos (x0, y0) como una posicion valida
			if x1 < 0 or x1 >= dimension or y1 < 0 or y1 >= dimension:
				continue

			# ahora, vamos a intentar escribir la palabra sobre el tablero.

			# primero copiamos el tablero para evitar sobreescribir los datos
			# que vamos a necesitar al intentar usar otra posicion inicial
			mi_tablero = copy.deepcopy(tablero)

			# iteramos por las posiciones que va a ocupar la palabra, viendo si
			# es posible escribir cada letra en la posicion en la que va
			(x, y) = (x0, y0)
			posible = True
			for letra in palabra:
				# podemos poner la palabra sobre el tablero si las letras
				# coinciden, o no hay nada escrito (usamos '?' para indicarlo)
				if mi_tablero[y][x] == letra or mi_tablero[y][x] == '?':
					mi_tablero[y][x] = letra
				else:
					posible = False
					break

				x += dx[direccion]
				y += dy[direccion]

			# si en el loop anterior vemos que la posicion que elegimos no es
			# viable, pasamos a la siguiente posibilidad
			if not posible:
				continue

			# si resulta ser viable, llegamos a este punto, e intentamos poner
			# mas palabras sobre el tablero.

			resultado = recursion_sopa_de_letras(dimension, mi_tablero, palabras, indice+1)

			# Si recibimos un tablero, entonces es posible posicionar todas las
			# palabras sobre el tablero.
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
