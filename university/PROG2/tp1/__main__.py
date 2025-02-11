import sys

from src.main import main

# intentamos leer el el argumento que corresponde al lemario
try:
    lemario = sys.argv[1]
except IndexError:
    # si no fue pasado ningun argumento se termina el programa 
    sys.exit('Error: debe ingresarse la ubicacion de los archivos \'lemario\' y \'registro\' al correr el programa')

# intentamos leer el el argumento que corresponde al lemario
try:
    registro = sys.argv[2]
except IndexError:
    # si el argumento no fue pasado se toma una ubicacion por default y se notifica al usuario
    print('No se ha ingresado un archivo \'registro\'. El mismo se guardara en \'registro.txt\'\n')
    registro = 'registro.txt'

#--------------------------------------------------		
# Inicia el juego
main(lemario, registro)