import sys

from src.main import main

try:
	lemario = sys.argv[1]
except IndexError:
	sys.exit('Error: debe ingresarse la ubicacion del lemario, del archivo de entrada y del archivo de salida al correr el programa')

try:
	entrada = sys.argv[2]
except IndexError:
	sys.exit('Error: debe ingresarse la ubicacion del archivo de entrada y del archivo de salida al correr el programa')

try:
	salida = sys.argv[3]
except IndexError:
	sys.exit('Error: debe ingresarse la ubicacion del archivo de salida al correr el programa')

main(lemario, entrada, salida)
