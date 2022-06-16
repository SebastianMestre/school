# tests del modulo archivos

import os

import src.archivos as archivos

def test_leer_lineas():
	camino = os.path.join("test", "samples", "lineas_sample.txt")
	resultado  = ["hola", "federico", "soy", "sebastian"]
	assert archivos.leer_lineas(camino) == resultado 

def test_parse_palabra_con_direccion():
	valor = "church 0"
	resultado = ("church", 0)
	assert archivos.parse_palabra_con_direccion(valor) == resultado

	valor = "turing 1"
	resultado = ("turing", 1)
	assert archivos.parse_palabra_con_direccion(valor) == resultado

	valor = "godel 5"
	resultado = ("godel", 5)
	assert archivos.parse_palabra_con_direccion(valor) == resultado

	valor = "hilbert 3"
	resultado = ("hilbert", 3)
	assert archivos.parse_palabra_con_direccion(valor) == resultado

def test_parse_entrada():
	valor = ["DIMENSION", "5", "PALABRAS", "pepe 1", "pipo 4"]
	resultado = (5, [("pepe", 1), ("pipo", 4)])
	assert archivos.parse_entrada(valor) == resultado
