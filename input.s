main: addi x10, x0, 2
jal x1, fact
beq x0, x0, exit
fact: addi sp, sp, -16
sd x1, 8(sp)
sd x10, 0(sp)
addi x5, x10, -1
blt x0, x5, L1
addi x10, x0, 1
addi sp, sp, 16
jalr x0, 0(x1)
L1: addi x10, x10, -1
jal x1, fact
addi x6, x10, 0
ld x10, 0(sp)
ld x1, 8(sp)
addi sp, sp, 16
jalr x0, 0(x1)
exit: add x0, x0, x0