# Helper para input/output y manipulacion de archivos


# String -> [String]
def leer_lemario(path):
	f = open(path, "r", encoding="latin1")
	lines = f.readlines()
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
