# Clase 2:
# En esta clase se explica una manera de realizar bucles en MIPS.

.data

# tlist es el tamaño de vlist
tlist: .word 10                               # static int tlist = 10
vlist: .word 1, 5, 2, 9, 14, 8, 7, 2, 1, 0    # static int vlist[] = {1, 5, ...etc}

.text

main:

	# cargamos el tamaño de nuestra lista de valores en t0 y la direccion en memoria de la lista en t1.

	lw $t0, tlist($0)       # t0 = *tlist
	la $t1, vlist           # t1 = vlist
	
	# En t2 vamos a guardar la suma de todos los valores.
	# Empezamos guardando el primer valor en vez de guardar un cero, asi nos ahorramos una suma.
	
	lw $t2, ($t1)           # t2 = t2 = *t1

L0:
	# Movemos el puntero cuatro espacios, este es el tamaño de palabra
	addi $t1, $t1, 4        # t1 = t1 + 4
	
	# cargamos el elemento actual y lo sumamos a t2
	lw $t3, ($t1)           # t3 = *t1
	add $t2, $t2, $t3       # t2 = t2 + t3

	# Reducimos el largo en 1 y comparamos con 1.
	# Si el tamaño es 1 entonces tenemos que terminar el bucle, en ese caso saltamos al final de bucle.
	addi $t0, $t0, -1       # t0 = t0 + -1
	beq $t0, 1, EL0         # if (t0 == 1) goto EL0
	
	# Si llegamos a esta instruccion t0 no es uno, por lo que tenemos que seguir ciclando
	j L0                    # goto L0
EL0:

	move $a0, $t2           # a0 = t2
	li $v0, 1               # v0 = 1
	syscall                 # syscall(1, t2) -> print_int(t2)

exit:
	li $v0, 10              # v0 = 10
	syscall                 # exit(0)
