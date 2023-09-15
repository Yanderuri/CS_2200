!============================================================
! CS 2200 Homework 2 Part 1: mod
!
! Edit any part of this file necessary to implement the
! mod operation. Store your result in $v0.
!============================================================

mod:
    addi    $a0, $zero, 28      ! $a0 = 28, the number a to compute mod(a,b)
    addi    $a1, $zero, 13      ! $a1 = 13, the number b to compute mod(a,b)
    
    add $t0, $a0, $zero          ! int x = a; t0
    add $t1, $a1, $zero          ! b -> t1;
    nand $t1, $t1, $t1
    addi $t1, $t1, 0x01
    loop:
    blt $t0, $a1, end
    add $t0, $t0, $t1
    beq $zero, $zero, loop

    end:
    add $v0, $zero, $t0
    halt