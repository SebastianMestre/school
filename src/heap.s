.text

##
# heap_push
#
# Pushes an element into a binary heap.
#
# Note: this procedure assumes the buffer is 
# large enough to hold the extra element.
#
# Arguments:
#  $a0 : pointer to buffer
#  $a1 : size of heap
#  $a2 : pointer to pushed element
#
# Runtime complexity : O(log_2($a1))
##

##
# For ease of reasoning, names were asigned to
# some of the registers that were used
# $s0 = arr
# $s2 = elem
# $s3 = i
# $s5 = j
##

heap_push:
	sw $s0, 0($sp)
	sw $s2, 4($sp)
	sw $s3, 8($sp)
	sw $s4, 12($sp)
	sw $s5, 16($sp)
	sw $s6, 20($sp)
	sw $s7, 24($sp)
	addi $sp, $sp, 28

	move $s0, $a0
	move $s2, $a2
	move $s3, $a1

	add $s4, $s0, $s3
	sw $s2, ($s4)

	heap_push_loop_check:
		; if i == 0
		;     goto end
		beqz $s3, heap_push_loop_end

		; j = (i-1)/2 # parent of i 
		addi $s5, $s3, -1
		sra $s5, $s5, 1

		; if arr[i] < arr[j]
		;     goto end
		add $s6, $s0, $s5
		lw $s7, ($s6)

		# TODO: Do proper comparison
		sub $v0, $s2, $s7
		
		bltz $v0, heap_push_loop_end
	heap_push_loop_body:
		; swap(arr[i], arr[j])
		sw $s7, ($s4)
		sw $s2, ($s6)

		; i = j
		move $s3, $s5
		move $s4, $s6

		; goto check
		j heap_push_loop_check
	heap_push_loop_end:

	addi $sp, $sp, -28
	lw $s0, 0($sp)
	lw $s2, 4($sp)
	lw $s3, 8($sp)
	lw $s4, 12($sp)
	lw $s5, 16($sp)
	lw $s6, 20($sp)
	lw $s7, 24($sp)

	jr $ra

