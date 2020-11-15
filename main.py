import random

#   El resultado de una partida esta modelado por una tupla
# Tuple(Bool, Int)
#   Esta contiene la condicion de exito, y la cantidad de
# intentos

#   El registro de juego de un jugador esta modelado por un
# Dict(Bool, Int)
#   Por cada partida del jugador, se asocia la palabra de la
# partida con el resultado de la misma

#   El registro completo esta modelado por un diccionario de
# registros de jugador. Eso es, un Dict(Dict(Tuple(Bool, Int)))
#   El nombre de un jugador se asocia con su registro

#   De aca en adelante, usamos los siguientes alias:
#
# ResultadoPartida = Tuple(Bool, Int)
# RegistroJugador  = Dict(ResultadoPartida)
# RegistroCompleto = Dict(RegistroJugador)

# () -> String
def pedir_nombre():
	nombre = input("Escriba su nombre...")
	return nombre

# [String] RegistroJugador -> String
def elegir_palabra(palabras, registro):
	palabra = random.choice(palabras)
	while palabra in registro:
		palabra = random.choice(palabras)
	return palabra

# String -> ResultadoPartida
# Hace E/S por pantalla
def jugar(palabra):
	# TODO: implementar
	return (False, 5)

# RegistroJugador String ResultadoPartida -> None
# Muta el registro
def actualizar_registro(registro, palabra, resultado_del_juego):
	registro[palabra] = resultado_del_juego

# [String] RegistroCompleto -> None
# Muta el registro
def partida(palabras, registro):

	nombre = pedir_nombre()
	if nombre not in registro:
		registro[nombre] = dict()

	palabra = elegir_palabra(palabras, registro[nombre])
	resultado_del_juego = jugar(palabra)
	actualizar_registro(registro[nombre], palabra, resultado_del_juego)

# String String -> None
# Hace E/S con archivos
def main(camino_al_lemario, camino_al_registro):
	lemario = leer_lemario(camino_al_lemario)
	registro = leer_registro(camino_al_registro)
	decision = input("Quiere jugar? (si/*)...")
	if decision == "si":
		partida(lemario, registro)
		escribir_registro(camino_al_registro, registro)
	print("bye.")
