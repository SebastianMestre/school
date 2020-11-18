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
    return (val[1], val[2])

# RegistroJugador String ResultadoPartida -> RegistroJugador
# Añade un ResultadoPartida al registro de jugador
def agregar_partida(registro_jugador, palabra, resultado_partida):
    registro_jugador[palabra] = resultado_partida
    return registro_jugador