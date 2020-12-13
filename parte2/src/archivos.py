
# String -> [String]
# Toma un camino y devuelve las lineas del mismo (sin incluir el final de linea)
def leer_lineas(camino, encoding='utf-8'):
	try:
		f = open(camino, "r", encoding=encoding)
		lineas = [linea[:-1] for linea in f.readlines()]
		f.close()
		return lineas
	except FileNotFoundError:
		return []

def leer_entrada(camino):
	lineas = leer_lineas(camino)
	return parse_entrada(lineas)

def parse_entrada(lineas):
	if lineas[0] != "DIMENSION":
		raise ValueError
	dimension = int(lineas[1])
	if lineas[2] != "PALABRAS":
		raise ValueError
	palabras_con_direccion = [parse_palabra_con_direccion(linea) for linea in lineas[3:]]
	return (dimension, palabras_con_direccion)

def parse_palabra_con_direccion(linea):
	partes = linea.split()
	return (partes[0], int(partes[1]))

def escribir_sopa_de_letras(camino, sopa):
	f = open(camino, 'w')
	lineas = [''.join(fila) + '\n' for fila in sopa]
	f.writelines(lineas)
	f.close()

