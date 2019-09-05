##
# Enunciado:
#
# Implementar la funcion que imprime los movimientos necesarios para resolver
# el juego "Torres de Hanoi". Para hacer esto se puede seguir el pseudo codigo
# presente en el campus virtual.
##

.data
msg_pretty_print_str0: .asciiz "disco "
msg_pretty_print_str1: .asciiz " de "
msg_pretty_print_str2: .asciiz " a "
msg_pretty_print_str3: .asciiz "\n"

.text


##
# pretty_print(n : int, orig : char, dest : char) : void
#
# equivalente a:
# printf("disco %d de %c a %c\n", n, orig, dest)
##
pretty_print:
    move $t0, $a0
    move $t1, $a1
    move $t2, $a2

    # "disco "
    li		$v0, 4
    la		$a0, msg_pretty_print_str0
    syscall

    # n
    li		$v0, 1
    move	$a0, $t0
    syscall

    # " de "
    li		$v0, 4
    la		$a0, msg_pretty_print_str1
    syscall

    # orig
    li		$v0, 11
    move	$a0, $t1
    syscall

    # " a "
    li		$v0, 4
    la		$a0, msg_pretty_print_str2
    syscall

    # dest
    li		$v0, 11
    move	$a0, $t2
    syscall

    # "\n"
    li		$v0, 4
    la		$a0, msg_pretty_print_str3
    syscall

    jr		$ra

##
# hanoi(n : int, orig : char, dest : char, spare : char) : void
#
# imprime la secuencia de movimientos necesaria para pasar todos los discos
# desde la columna "orig" a la columna "dest", en el juego "Torres de Hanoi"
#
# pseudo codigo:
#
# def hanoi (n, orig, dest, spare):
#   if n == 1:
#     pretty_print(n, orig, dest);
#   else:
#     hanoi (n-1, orig, spare, dest);
#     pretty_print(n, orig, dest);
#     hanoi (n-1, spare, dest, orig);
##
hanoi:
    # push $s0, $s1, $s2, $s3, $ra
    sw $s0, -0($sp)
    sw $s1, -4($sp)
    sw $s2, -8($sp)
    sw $s3, -12($sp)
    sw $ra, -16($sp)
    addi $sp, $sp, -20
    
    move $s0, $a0 # $s0 = n
    move $s1, $a1 # $s1 = orig
    move $s2, $a2 # $s2 = dest
    move $s3, $a3 # $s3 = spare

    bne	$a0, 1, hanoi_else

    # pretty_print(n, orig, dest)
    # los argumentos ya estan donde corresponden
    jal pretty_print

    j hanoi_return
hanoi_else:

    # hanoi(n-1, orig, spare, dest)
    addi $a0, $s0, -1
    move $a1, $s1
    move $a2, $s3
    move $a3, $s2
    jal hanoi

    # pretty_print(n, orig, dest)
    move $a0, $s0
    move $a1, $s1
    move $a2, $s2
    jal pretty_print

    # hanoi(n-1, spare, dest, orig)
    addi $a0, $s0, -1
    move $a1, $s3
    move $a2, $s2
    move $a3, $s1
    jal hanoi

hanoi_return:

    addi	$sp, $sp, 20
    lw		$s0, -0($sp)
    lw		$s1, -4($sp)
    lw		$s2, -8($sp)
    lw		$s3, -12($sp)
    lw		$ra, -16($sp)

    jr		$ra

main:
    li		$a0, 40
    li		$a1, 65
    li		$a2, 66
    li		$a3, 67
    jal hanoi

    li		$v0, 10
    syscall
    
