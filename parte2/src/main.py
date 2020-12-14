import src.archivos as archivos
import src.generar as generar


def validar_entrada(lemario, palabras):
	return palabras

def main(camino_lemario, camino_entrada, camino_salida):
	lemario = archivos.leer_lineas(camino_lemario, 'latin1')
	(dimension, palabras) = archivos.leer_entrada(camino_entrada)
	palabras_validas = validar_entrada(lemario, palabras)
	sopa_de_letras = generar.generar_sopa_de_letras(dimension, palabras_validas)
	archivos.escribir_sopa_de_letras(camino_salida, sopa_de_letras)
