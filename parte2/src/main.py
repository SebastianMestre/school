import random

import src.archivos as archivos
import src.generar as generar

# validar_entrada : Int [String] [(String, Int)] -> [(String, Int)]
# Devuelve una lista de palabras con direccion, reemplazando las palabras
# demasiado largas o repetidas con palabras aleatorias del lemario.
# Mantiene las direcciones.
def validar_entrada(dimension, lemario, palabras):
	palabras_sin_validar = set([palabra for palabra, direccion in palabras])

	palabras_validas = set()
	palabras_dirigidas_validas = []

	for palabra, direccion in palabras:
		eleccion = palabra
		# decimos que una palabra es invalida si es demasiado larga, o esta repetida
		# si esta repetida, mantenemos la primera aparicion, el resto son reemplazadas
		if len(palabra) > dimension or palabra in palabras_validas:
			eleccion = random.choice(lemario)
			# al elegir palabras aleatorias, agregamos la condicion de que la
			# palabra no este en la entrada, para evitar elegirla en este bucle
			# y posiblemente selecionarla con una direccion distinta
			while len(eleccion) > dimension or eleccion in palabras_validas or eleccion in palabras_sin_validar:
				eleccion = random.choice(lemario)

		palabras_validas.add(eleccion)
		palabras_dirigidas_validas.append((eleccion, direccion))

	return palabras_dirigidas_validas

def main(camino_lemario, camino_entrada, camino_salida):
	lemario = archivos.leer_lineas(camino_lemario, 'latin1')
	(dimension, palabras) = archivos.leer_entrada(camino_entrada)
	palabras_validas = validar_entrada(dimension, lemario, palabras)
	sopa_de_letras = generar.generar_sopa_de_letras(dimension, palabras_validas)
	archivos.escribir_sopa_de_letras(camino_salida, sopa_de_letras)
