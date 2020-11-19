from src.archivos import *
from test.helper import get_content

def test_leer_lemario():
    path = r'test\lemario_sample.txt'

    expected_output = {'aaronico', 'aaronita', 'aba', 'ababa', 'ababillarse'}
    assert leer_lemario(path) == expected_output

def test_leer_registro():
    path = r'test\registro_sample.txt'

    expected_output = {'Federico': {'cohete': (True, 5), 'repasador': (True, 9)}, 'Carlos': {'abecedario': (False, 8)}}
    assert leer_registro(path) == expected_output

def test_escribir_registro():
    path = r'test\blank.txt'
    
    registro = {'Federico': {'cohete': (True, 5), 'repasador': (True, 9)}, 'Carlos': {'abecedario': (False, 8)}}
    escribir_registro(path, registro)
    content = get_content(path)

    expected_output = "Federico\ncohete,SI,5\nrepasador,SI,9\nCarlos\nabecedario,NO,8\n"
    assert content == expected_output

def test_interpret_registro():
    lineas = ['Federico', 'cohete,SI, 5', 'repasador,SI, 9', 'Carlos', 'abecedario,NO, 8']
    expected_output = {'Federico': {'cohete': (True, 5), 'repasador': (True, 9)}, 'Carlos': {'abecedario': (False, 8)}}
    assert interpret_registro(lineas) == expected_output

def test_leer_lineas():
    path = r'test\lemario_sample.txt'
    expected_output = ['aaronico', 'aaronita', 'aba', 'ababa', 'ababillarse']
    assert leer_lineas(path) == expected_output

    path = r'este_archivo_no_existe.txt'
    expected_output = []
    assert leer_lineas(path) == []

def test_es_nombre_jugador():
    nombre = 'Sebas'
    assert es_nombre_jugador(nombre)

    nombre = 'Mestre, Sebas'
    assert not es_nombre_jugador(nombre)

def test_format_registro_jugador():
    jugador = 'Sebas'
    partidas = {'prueba': (True, 4), 'cohete': (False, 3)}

    expected_output = ['Sebas\n', 'prueba,SI,4\n', 'cohete,NO,3\n']
    assert format_registro_jugador(jugador, partidas) == expected_output

def test_format_partida():
    palabra = 'prueba'
    resultado = (True, 4)

    expected_output = 'prueba,SI,4'
    assert format_partida(palabra, resultado) == expected_output

def test_format_condicion():
    condicion = True
    assert format_condicion(condicion) == 'SI'
    condicion = False
    assert format_condicion(condicion) == 'NO'