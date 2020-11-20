from src.main import *

import pytest

def test_actualizer_registro():
	registro = {'cohete': (True, 5)}
	expected_output = {'cohete': (True, 5), 'repasador': (True, 9)}
	actualizar_registro(registro, 'repasador', (True, 9))
	assert registro == expected_output

def test_elegir_palabra():
	palabras = set(['cohete', 'repasador'])
	registro = {'cohete': (True, 5)}

	palabra = elegir_palabra(palabras, registro)
	assert palabra == 'repasador'

	palabras = set(['cohete'])
	with pytest.raises(ValueError):
		elegir_palabra(palabras, registro)

def test_preparar_partida():
	# TODO
	assert True
