# Tarea 1: Menu
# Advertencia: esta implementacion de un menu sencillo en MIPS contiene codigo
# que no sigue las convenciones de llamadas a funciones. Por lo que se deben
# tener ciertas consideraciones al leerlo.
# particularmente, se utilizan saltos sobre el registro $a1 en lugar de jal y $ra
    .data

TextoInicio: .asciiz "PRINCIPAL\nOpciones:\n - 1.\n - 2.\n - 3.\n - 4. Salir\n"
TextoOp1: .asciiz "OP1\nOpciones:\n - 1. Volver\n - 2. Salir\n"
TextoOp2: .asciiz "OP2\nOpciones:\n - 1. Volver\n - 2. Salir\n"
TextoOp3: .asciiz "OP3\nOpciones:\n - 1. Volver\n - 2. Salir\n"
TextoInvalido: .asciiz "Opcion Invalida. Vuelva a intentar\n"
TextoTerminacion: .asciiz "Saliendo del programa.\n"

    .text

main:
    la $a0, TextoInicio
    li $v0, 4
    syscall

    # 
    li $v0, 5
    syscall

    beq $v0, 1, op1Selec
    beq $v0, 2, op2Selec
    beq $v0, 3, op3Selec
    beq $v0, 4, exit

    la $a0, main
    j opcionInvalida

op1Selec:
    la $a0, TextoOp1
    li $v0, 4
    syscall

    li $v0, 5
    syscall

    beq $v0, 1, main
    beq $v0, 2, exit

    la $a0, op1Selec
    j opcionInvalida
    
op2Selec:
    la $a0, TextoOp2
    li $v0, 4
    syscall

    li $v0, 5
    syscall

    beq $v0, 1, main
    beq $v0, 2, exit

    la $a0, op2Selec
    j opcionInvalida

op3Selec:
    la $a0, TextoOp3
    li $v0, 4
    syscall

    li $v0, 5
    syscall

    beq $v0, 1, main
    beq $v0, 2, exit

    la $a0, op3Selec
    j opcionInvalida

opcionInvalida:

    move $a1, $a0
    la $a0, TextoInvalido
    li $v0, 4
    syscall

    j $a1

exit:
    li $v0, 4
    la $a0, TextoTerminacion
    syscall

    li $v0, 10
    syscall 
