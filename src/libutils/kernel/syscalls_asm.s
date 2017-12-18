# Created by dimok
# Syscalls for kernel that we use

     .globl SC0x25_KernelCopyData
SC0x25_KernelCopyData:
    li r0, 0x2500
    sc
    blr
