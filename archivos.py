# Helper para manipulacion de archivos

import modelo


# Archivo -> [String]
# Toma un archivo de lemario y retorna un lemario
def leer_lemario(path):
	result = leer_lineas(path)
	return result

# Archivo -> RegistroCompleto
# Toma un archivo de registro y retorna un registro completo
def leer_registro(path):
    sep = separador()                           # separador de valores en la entrada

    registro_completo = modelo.new_registro()   # crea registro de juego

    # lee del archivo de registro
    lines = leer_lineas(path)
    # itera sobre el contenido del archivo
    jugador=None
    for line in lines:
        if es_nombre_jugador(line):                             # si la linea corresponde al nombre de un jugador
            jugador = line                                          # guarda el nombre en 'jugador'
            registro_completo[jugador] = modelo.new_jugador()       # crea un RegistroJugador en el registro completo

        # si la linea no corresponde al nombre de un jugador entonces corresponde a una partida
        else:                                                   
            partida = line.split(sep)                       # guarda [palabra, condicion, intentos] en partida 
            palabra = partida[0]
            resultado_partida = modelo.new_partida(partida) # crea nuevo ResultadoPartida a partir del valor leido

            # guarda el RegistroJugador asociado a 'jugador' en 'registro_jugador'
            registro_jugador = registro_completo[jugador]               
            # agrega partida a RegistroJugador nuevo
            registro_jugador_actualizado = modelo.agregar_partida(registro_jugador, palabra, resultado_partida)
            # actualiza RegistroJugador en el registro completo
            registro_completo[jugador] = registro_jugador_actualizado

    return registro_completo

# Archivo -> None
# Toma un archivo y un registro completo y guarda el segundo en el primero
def escribir_registro(path, registro):
    f = open(path, 'w')
    for jugador,partidas in registro.items():
        lines = format_registro_jugador(jugador, partidas)  # formatea registro
        f.writelines(lines)                                 # guarda registro en el archivo
    f.close()
    return

# Archivo -> [String]
# Toma un archivo y retorna las lineas del mismo normalizadas (sin separador)
def leer_lineas(path):
    try:
        f = open(path, "r", encoding="latin1")
        lines = f.readlines()
        # readlines deja un '\n' al final de cada lines asique armo un nuevo array
        # sin el ultimo caracter de cada linea
        lines = [line[:-1] for line in lines]
        f.close()
        return lines
    except FileNotFoundError:
        return []

# String -> Boolean
# Toma una linea del archivo registro y determina si es el nombre de un jugador
def es_nombre_jugador(line):
    sep = separador()
    es_partida = sep in line    # si hay un separador entonces la linea determina una partida
    return not es_partida       # si no es partida entonces es nombre de jugador

# String  RegistroJugador -> [String]
# Toma un nombre de jugador y el RegistroJugador asociado y retorna una lista con jugador
# como cabecera y cada partida de la forma 'palabra, condicion, intentos'
def format_registro_jugador(jugador, partidas):
    result = list()
    result.append(jugador + "\n")

    for palabra,resultado in partidas.items():
        val = format_partida(palabra, resultado)
        result.append(val + "\n")
    return result

# String ResultadoPartida -> String
# Toma una palabra y un ResultadoPartida y retorna una cadena de la forma
# 'palabra, condicion, intentos'
def format_partida(palabra, resultado):
    sep = separador()
    
    # obtiene los valores de la tupla ResultadoPartida
    (condicion, intentos) = resultado 
    # construye y retorna cadena 'palabra, condicion, intentos'
    return palabra + sep + format_condicion(condicion) + sep + str(intentos)

# Boolean -> String
# Toma una condicion de exito y retorna 'SI' o 'NO' para cada caso
def format_condicion(condicion):
    if condicion: return 'SI'
    else: return 'NO'

# None -> String
# Devuelve separador para valores inline en lectura/escritura de archvos
def separador(): return ','