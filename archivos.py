# Helper para manipulacion de archivos



# Archivo -> [String]
def leer_lemario(path):
	result = leer_lineas(path)
	return result

# Archivo -> RegistroCompleto
def leer_registro(path):
    #TODO
    return {}

# Archivo -> None
def escribir_registro(path, registro):
    #TODO
    return

# Archivo -> [String]
# Toma un archivo y retorna las lineas del mismo normalizadas (sin separador)
def leer_lineas(path):
    f = open(path, "r", encoding="latin1")
    lines = f.readlines()
    # readlines deja un '\n' al final de cada lines asique armo un nuevo array
	# sin el ultimo caracter de cada linea
    lines = [line[:-1] for line in lines]
    f.close()
    return lines

# String -> Boolean
# Toma una linea del archivo registro y determina si es el nombre de un jugador
def es_nombre_jugador(line):
    sep = ','
    es_partida = sep in line    # si hay un separador entonces la linea determina una partida
    return not es_partida       # si no es partida entonces es nombre de jugador 