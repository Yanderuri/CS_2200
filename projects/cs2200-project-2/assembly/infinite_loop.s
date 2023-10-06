vector0:    add $zero, $zero, $zero
            add $zero, $zero, $zero
            add $zero, $zero, $zero
            add $zero, $zero, $zero
            add $zero, $zero, $zero
            add $zero, $zero, $zero
            add $zero, $zero, $zero
            add $zero, $zero, $zero
main:	lea $sp, initsp                         ! initialize the stack pointer
        lw $sp, 0($sp)                          ! finish initialization
        lea $t0, timer_handler
        lea $t1, vector0
        sw $t0, 0($t1)
        ei                                      ! enable interrupts
        addi $t0, $zero, 1
        start: addi $t0, $zero, 4
        add $zero, $zero, $zero
        end: beq $zero, $zero, start
        halt                                    ! just in case, halt
timer_handler:                                  ! TODO FIX ME
        addi $sp, $sp, -1
        sw $k0, 0($sp)
        ei                                      
        addi $sp, $sp, -2                       
        sw $t0, 0($sp)
        sw $t1, 1($sp)
                                                ! Actual work here
        lea $t1, ticks
        lw $t1, 0($t1)                          ! $t1 = xFFFF
        lw $t0, 0($t1)
        addi $t0, $t0, 1
        sw $t0, 0($t1)
                                                ! timer_handler_teardown:
        lw $t0, 0($sp)
        lw $t1, 1($sp)
        addi $sp, $sp, 2
        di
        lw $k0, 0($sp)
        addi $sp, $sp, 1
        reti



halt                                            ! just in case, halt

initsp: .fill 0xA000
ticks:  .fill 0xFFFF
range:  .fill 0xFFFE
maxval: .fill 0xFFFD
minval: .fill 0xFFFC