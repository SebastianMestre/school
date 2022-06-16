# tests del modulo generar

import copy

import src.generar as generar

def test_palabra_entra_en_tablero():
	assert not generar.palabra_entra_en_tablero(5, "pepe", 0, 5, 1, -1)
	assert generar.palabra_entra_en_tablero(10, "pepe", 0, 0, 1, 1)
	assert not generar.palabra_entra_en_tablero(10, "pepe", 9, 0, 1, 1)
	assert generar.palabra_entra_en_tablero(5, "perro", 0, 0, 1, 0)
	assert not generar.palabra_entra_en_tablero(4, "perro", 0, 0, 1, 0)
	assert generar.palabra_entra_en_tablero(8, "amigdala", 0, 7, 1, -1)

def test_es_palindromo():
	assert generar.es_palindromo("epe")
	assert not generar.es_palindromo("maria")
	assert generar.es_palindromo("abalaba")
	assert not generar.es_palindromo("alababa")
	assert generar.es_palindromo("reconocer")

def test_escanear():
	tablero = [
		['b','m','o','n','i','t','o','r'],
		['a','c','k','t','h','c','n','s'],
		['a','i','r','a','m','o','o','t'],
		['b','r','d','c','n','h','r','w'],
		['d','y','g','f','d','e','b','g'],
		['s','u','l','d','k','t','i','z'],
		['w','l','d','p','w','e','f','u'],
		['s','a','l','t','e','l','a','i']
	]
	assert generar.escanear(tablero, "monitor", 1, 0, 1, 0)
	assert generar.escanear(tablero, "cohete",  5, 1, 0, 1)
	assert generar.escanear(tablero, "tela",    3, 7, 1, 0)
	assert generar.escanear(tablero, "maria",   4, 2,-1, 0)
	assert generar.escanear(tablero, "fibron",  6, 6, 0,-1)

def test_es_tablero_valido():
	palabras = [
		("maria", 1),
		("cohete", 2),
		("tela", 0),
		("fibron", 3),
		("monitor", 0)
	]

	assert generar.es_tablero_valido(8, [
		['b','m','o','n','i','t','o','r'],
		['a','c','k','t','h','c','n','s'],
		['a','i','r','a','m','o','o','t'],
		['b','r','d','c','n','h','r','w'],
		['d','y','g','f','d','e','b','g'],
		['s','u','l','d','k','t','i','z'],
		['w','l','d','p','w','e','f','u'],
		['s','a','l','t','e','l','a','i']
	], palabras)

	assert generar.es_tablero_valido(8, [
		['c','a','a','i','r','a','m','g'],
		['o','r','q','w','e','u','n','h'],
		['h','q','w','e','r','c','o','j'],
		['e','d','s','f','d','s','r','b'],
		['t','e','l','a','z','z','b','a'],
		['e','r','q','w','e','r','i','h'],
		['m','v','z','n','m','z','f','l'],
		['m','o','n','i','t','o','r','p']
	], palabras)

	assert not generar.es_tablero_valido(8, [
		['c','a','m','a','r','i','a','t'],
		['o','r','q','w','e','u','f','e'],
		['h','q','w','e','r','c','i','l'],
		['e','d','s','f','d','s','b','a'],
		['t','e','l','a','z','z','r','a'],
		['e','r','q','w','e','r','o','h'],
		['m','v','z','n','m','z','n','l'],
		['m','o','n','i','t','o','r','p']
	], palabras)

def test_rellenar_posiciones_vacias():
	palabras = [("ab", 2), ("cb", 0)]
	tablero = [
		['?', 'a'],
		['c', 'b']
	]

	# si rellenar_posiciones_vacias fuera simplemente aleatorio, esto correria
	# sin disparar el assert con probablidad (25/26)^1000 =~ 10^-17
	for i in range(1000):
		resultado = generar.rellenar_posiciones_vacias(2, copy.deepcopy(tablero), palabras)
		assert resultado[0][0] != '?'
		assert resultado[0][0] != 'b'

