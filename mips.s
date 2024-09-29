.data
.text
.globl main
main:
	li $t0, 1
	sw $t0, t0
	li $t0, 12
	sw $t0, t1
	li $t0, 23
	sw $t0, t2
	li $t0, 10
	sw $t0, t3
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t4
	li $t0, 1
	sw $t0, t5
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t6
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t7
	li $t0, 5
	sw $t0, t8
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t9
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t10
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t11
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t12
	lw $t0, (null)
	lw $t1, (null)
	add $t2, $t0, $t1
	sw $t2, t13
	li $v0, 10
	syscall
