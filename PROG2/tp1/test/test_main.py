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
	palabras = set(['cohete', 'repasador'])
	registro = {'Sebas': {'repasador': (False, 10)}}

	# agrega un jugador que no esta al registro
	preparar_partida('Santi', palabras, registro)

	expected_output = {'Santi': {}, 'Sebas': {'repasador': (False, 10)}}
	assert registro == expected_output

	registro = {'Santi': {'cohete': (True, 5), 'repasador': (True, 9)}, 'Sebas': {'repasador': (False, 10)}}

	# limpia el registro de un jugador si ya jugo todas las palabras
	preparar_partida('Santi', palabras, registro)
	expected_output = {'Santi': {}, 'Sebas': {'repasador': (False, 10)}}
	assert registro == expected_output
