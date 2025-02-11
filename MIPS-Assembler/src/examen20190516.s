.data

# Buffer de largo 32
buffer: .word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

amount_message: .asciiz "Cantidad de numeros: "

input_loop_message_1: .asciiz "Entre numero ["
input_loop_message_2: .asciiz "]: "

dif_message: .asciiz "Las diferencia de los cuadrados: "
dif_separator_message: .asciiz ", "

end_message: .asciiz "Fin del programa\n"

line_break_message: .asciiz "\n"

.text
main:

	li $v0, 4
	la $a0, amount_message
	syscall

	li $v0, 5
	syscall

	move $s0, $v0

	beq $s0, 0, main_end

	move $s1, $s0                  # $s1 -> contador
	la $s2, buffer                 # $s2 -> iterador del buffer

input_loop_start:

	li $v0, 4
	la $a0, input_loop_message_1   # Imprimir prefijo del mensaje
	syscall

	# Avanzamos el contador aca para que los indices en la salida empiezen en 1
	addi $s1, $s1, -1

	li $v0, 1
	sub $a0, $s0, $s1              # Imprimir indice
	syscall

	li $v0, 4
	la $a0, input_loop_message_2   # Imprimir sufijo del mensaje
	syscall

	li $v0, 5                      # Tomamos un entero
	syscall

	sw $v0, ($s2)                  # guardamos el entero en el buffer
	
	# Avanzamos nuestro iterador
	addi $s2, 4                    
	bne $s1, 0, input_loop_start
input_loop_end:
	
	# Al final del bucle $s2 ya apunta a la palabra siguiente al final del vector
	move $a1, $s2
	la $a0, buffer
	jal dif

	j main

main_end:

	li $v0, 4
	la $a0, end_message
	syscall

	li $v0, 10
	syscall

# dif : Imprime las diferencias de los cuadrados de valores consecutivos
# desde el final del vector hacia el principio
#
# $a0 : direccion de la palabra al inicio del vector
# $a1 : direccion de la palabra siguiente al final del vector
dif:

	move $t0, $a0                  # $t0 -> sentinela
	addi $t1, $a1, -4              # $t1 -> iterador

	# Si nuestro iterador ya apunta al inicio, no existe par para calcular la diferencia
	beq $t0, $t1, dif_loop_end

	li $v0, 4
	la $a0, dif_message
	syscall

	# En $t2 y $t3 guardamos los cuadrados de un par de numeros consecutivos en el vector
	# En $t3 el "actual", y en en $t2 el visitado previamente.
	lw $t2, ($t1)
	mult $t2, $t2
	mflo $t2

dif_loop_start:
	# Movemos nuestro iterador una posicion hacia atras.
	addi $t1, $t1, -4

	# Cargamos el numero actual en $t3 y lo elevamos al cuadrado
	lw $t3, ($t1)
	mult $t3, $t3
	mflo $t3


	li $v0, 1
	sub $a0, $t3, $t2
	syscall

	li $v0, 4
	la $a0, dif_separator_message
	syscall

	# Movemos el cuadrado del actual para tratarlo como "anterior" en la siguiente iteracion 
	move $t2, $t3

	# Mientras el iterador no haya llegado al principio, seguimos iterando.
	bne $t0, $t1, dif_loop_start
dif_loop_end:

	li $v0, 4
	la $a0, line_break_message
	syscall

	jr $ra
