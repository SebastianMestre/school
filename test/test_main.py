from src.main import *

def test_actualizer_registro():
	registro = {'cohete': (True, 5)}
	expected_output = {'cohete': (True, 5), 'repasador': (True, 9)}
	actualizar_registro(registro, 'repasador', (True, 9))
	assert registro == expected_output

def test_elegir_palabra():
    #TODO
    assert True
