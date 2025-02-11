from pytest import raises

from src.modelo import *


def test_new_registro():
    assert new_registro() == dict()

def test_new_jugador():
    assert new_jugador() == dict()

def test_new_partida():
    val = ['prueba', 'SI', '8']
    assert new_partida(val) == (True, 8)
    val = ['prueba', 'NO', '5']
    assert new_partida(val) == (False, 5)
    val = ['prueba', 'QUIZAS', '4']
    with raises(ValueError):
        new_partida(val)

def test_agregar_partida():
    registro = new_jugador()

    palabra = 'palabra'
    resultado = (False, 7)
    assert agregar_partida(registro, palabra, resultado) == {'palabra': (False, 7)}
    
    palabra = 'prueba'
    resultado = (True, 4)
    assert agregar_partida(registro, palabra, resultado) == {'palabra': (False, 7), 'prueba' : (True, 4)}

def test_interpret_resultado():
    condicion = 'SI'
    intentos = '8'
    assert interpret_resultado(condicion, intentos) == (True, 8)

    condicion = 'NO'
    intentos = '4'
    assert interpret_resultado(condicion, intentos) == (False, 4)

    condicion = 'No'
    with raises(ValueError):
        interpret_resultado(condicion, intentos)

    condicion = 'NO'
    intentos = 'c'
    with raises(ValueError):
        interpret_resultado(condicion, intentos)

def test_interpret_condicion():
    condicion = 'SI'
    assert interpret_condicion(condicion) == True

    condicion = 'NO'
    assert interpret_condicion(condicion) == False

    condicion = 'QUIZAS'
    with raises(ValueError):
        interpret_condicion(condicion)