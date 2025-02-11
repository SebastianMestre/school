.text

# bubble_sort : Implementa el algoritmo de ordenamiento de burbuja
# argumentos :
#  $a0 : direccion de la primera palabra del vector
#  $a1 : direccion de la palabra siguiente al final del vector
bubble_sort:
    addi $t0, $a0, 4
    move $t1, $a1
L0:
    beq $t0, $t1, EL0
    move $t2, $t0
L1:
    beq $t2, $t1, EL1
    lw $t3, -4($t2)
    lw $t4, ($t2)
    ble $t3, $t4, noswap
    sw $t3, ($t2)
    sw $t4, -4($t2)
noswap:
    addi $t2, $t2, 4
    j L1
EL1:
    addi $t1, $t1, -4
    j L0
EL0:
    jr $ra

# main : Lee el largo 'N' de una sequencia de numeros, seguido de N numeros: los
# valores de la sequencia, imprime la secuencia ordenada
main:
    li $v0, 5
    syscall

    move $t0, $v0
    sll $t0, $t0, 2
    move $a1, $t0

    li $v0, 9
    move $a0, $t0
    syscall

    move $s0, $v0
    add $s1, $s0, $t0

    move $t0, $s0
    move $t1, $s1

L3:
    beq $t0, $t1, EL3

    li $v0, 5
    syscall
    
    sw $v0, ($t0)
    
    add $t0, $t0, 4
    j L3
EL3:
    
    move $a0, $s0
    move $a1, $s1
    jal bubble_sort

    move $t0, $s0
    move $t1, $s1
L2:
    beq $t0, $t1, EL2

    li $v0, 1
    lw $a0, ($t0)
    syscall

    add $t0, $t0, 4
    j L2
EL2:
    li $v0, 10
    syscall
