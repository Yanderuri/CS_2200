addi $a0, $zero, 0x05
addi $a1, $zero, 0x03
blt $a0, $a1, continue
main: blt $a1, $a0, continue
add $a2, $zero, $zero
continue: bgt $a1, $a0, main
halt
