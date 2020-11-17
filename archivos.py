# Helper para input/output y manipulacion de archivos


# String -> [String]
def leer_lemario(path):
	f = open(path, "r", encoding="latin1")
	lines = f.readlines()
	f.close()
	# readlines deja un '\n' al final de cada lines asique armo un nuevo array
	# sin el ultimo caracter de cada linea
	result = [line[:-1] for line in lines]
	return result

# String -> RegistroCompleto
def leer_registro(path):
    #TODO
    return {}

# String -> None
def escribir_registro(path, registro):
    #TODO
    return

# String -> Boolean
# Toma una linea del archivo registro y determina si es el nombre de un jugador
def es_nombre_jugador(line):
    sep = ','
    es_partida = sep in line    # si hay un separador entonces la linea determina una partida
    return not es_partida       # si no es partida entonces es nombre de jugador 