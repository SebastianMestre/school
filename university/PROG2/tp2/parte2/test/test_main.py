# tests del modulo main

import src.main as modulo

def test_validar_entrada_quitar_palabras_que_no_estan_en_el_lemario():
	lemario = ["aaa", "bbb"]
	palabras = [("aaa", 0), ("ggg", 0)]

	output = modulo.validar_entrada(1000, lemario, palabras)
	comparar = set([palabra for palabra, direccion in output])

	assert comparar == set(["aaa", "bbb"])

def test_validar_entrada_quitar_palabras_muy_largas():
	lemario = ["aaa", "bbb", "dddd"]
	palabras = [("aaa", 0), ("dddd", 0)]

	output = modulo.validar_entrada(3, lemario, palabras)
	comparar = set([palabra for palabra, direccion in output])

	assert comparar == set(["aaa", "bbb"])

def test_validar_entrada_mantenter_direcciones():
	lemario = ["aaa", "bbb", "ccc", "dddd", "eeee", "ffff"]
	palabras = [("aaa", 0), ("ggg", 1), ("dddd", 4)]

	output = modulo.validar_entrada(3, lemario, palabras)
	comparar = [direccion for palabra, direccion in output]

	assert comparar == [0, 1, 4]

