.data
# La seccion data contiene los datos estaticos de nuestro programa. Estos se
# cargan en memoria cuando el programa arranca, de manera analoga a la palabra
# static del lenguaje C.

msg1: .asciiz "Hola Mundo\n"     # static char[] = "Hola Mundo\n"
num1: .word 3                    # static int = 3

.text
# La seccion text contiene las instrucciones que nuestro programa va a ejecutar.
# A continuacion vemos algunas de ellas, como li, la, y syscall
main:

# li ("Load Immediate") carga un valor 'inmediato' en el registro indicado.
# Un inmediato es algo asi como un 'literal' en C. Es decir, un valor escrito
# en el codigo.
    li $v0, 4                    # v0 = 4

# la ("Load Address") carga la direccion de una etiqueta en un registro.
    la $a0, msg1                 # a0 = msg1

# syscall ("SYStem CALL") realiza una llamada al sistema. Estas realizan
# distintas acciones relacionadas con I/O y manejo de memoria
# Para decidir la accion que tomara la llamada al sistema, guardamos un codigo
# en el registro $v0. el codigo 4 indica que queremos 'imprimir' un string
# terminado en NULL, de forma similar a la funcion puts() de C.
# Esta llamada al sistema lee la direccion del string del registro $a0.
    syscall                      # syscall(4, msg1) -> puts(mgs1)

# La llamada al sistema 10 representa una salida exitosa del programa.
    li $v0, 10                   # v0 = 10
    syscall                      # syscall(10) -> exit(0)
