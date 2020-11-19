# Modelo de datos

# None -> RegistroCompleto
# Retorna un registro completo vacio
def new_registro(): 
    return dict()

# None -> RegistroJugador
# Retorna un registro de jugador vacio
def new_jugador(): 
    return dict()

# [String] -> ResultadoPartida
# Toma una lista del tipo [palabra, condicion, intentos] y retorna un ResultadoPartida
def new_partida(val):
    resultado = interpret_resultado(val[1], val[2])
    return resultado

# RegistroJugador String ResultadoPartida -> RegistroJugador
# Añade un ResultadoPartida al registro de jugador
def agregar_partida(registro_jugador, palabra, resultado_partida):
    registro_jugador[palabra] = resultado_partida
    return registro_jugador

# String String -> ResultadoPartida
# Toma dos valores 'condicion', 'intentos' y retorna un ResultadoPartida
def interpret_resultado(condicion, intentos):
    try:
        intentos = int(intentos)
        condicion = interpret_condicion(condicion)      # interpreta condicion
    except ValueError:
        print('Error en la entrada para el ResultadoPartida')
        raise
    return (condicion, intentos)

# 'SI' | 'NO' -> Boolean
# Determina la condicion de exito
def interpret_condicion(condicion):
    if condicion == 'SI': return True
    elif condicion == 'NO': return False
    else: 
        print('\'{}\' no es un valor valido'.format(condicion))
        raise ValueError