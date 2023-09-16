!============================================================
! CS 2200 Homework 2 Part 2: Tower of Hanoi
!
! Apart from initializing the stack,
! please do not edit mains functionality. You do not need to
! save the return address before jumping to hanoi in
! main.
!============================================================

main:
    add     $zero, $zero, $zero     ! TODO: Here, you need to get the address of the stack
    lea     $sp, stack              ! using the provided label to initialize the stack pointer.
    lw      $fp, 0($sp)             ! load the label address into $sp and in the next instruction,
    lw      $sp, 0($sp)             ! use $sp as base register to load the value (0xFFFF) into $sp.


    lea     $at, hanoi              ! loads address of hanoi label into $at

    lea     $a0, testNumDisks3      ! loads address of number into $a0
    lw      $a0, 0($a0)             ! loads value of number into $a0
    ; addi $a0, $zero, 10             ! testing purposes

    jalr    $at, $ra                ! jump to hanoi, set $ra to return addr
    halt                            ! when we return, just halt

hanoi:
    addi $sp, $sp, -2               ! TODO: perform post-call portion of
    sw $fp, 0($sp)                  ! the calling convention. Make sure to
    add $fp, $sp, $zero             ! save any registers you will be using!

                                    ! TODO: Implement the following pseudocode in assembly:
    addi $t0, $zero, 0x01           ! IF ($a0 == 1)
    beq $t0, $a0, base              !    GOTO base
    beq $zero, $zero, else          ! ELSE
                                    !    GOTO else

else:
    addi $a0, $a0, -1               ! TODO: perform recursion after decrementing
                                    ! the parameter by 1. Remember, $a0 holds the
                                    ! parameter value.
    sw $ra, 1($sp)
    lea $at, hanoi
    jalr $at, $ra
    
    lw $ra, 1($sp)
    add $v0, $v0, $v0               ! TODO: Implement the following pseudocode in assembly:
    addi $v0, $v0, 0x01             ! $v0 = 2 * $v0 + 1
    beq $zero, $zero, teardown      ! RETURN $v0

base:
    addi $v0, $zero, 0x01           ! TODO: Return 1
    beq $zero, $zero, teardown

teardown:
    lw $fp, 0($fp)                  ! TODO: perform pre-return portion
    addi $sp, $fp, 0                ! of the calling convention
    jalr    $ra, $zero              ! return to caller



stack: .word 0xFFFF                 ! the stack begins here


! Words for testing \/

! 1
testNumDisks1:
    .word 0x0001

! 10
testNumDisks2:
    .word 0x000a

! 20
testNumDisks3:
    .word 0x0014
