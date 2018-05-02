    .globl getApplicationEndAddr
getApplicationEndAddr:
    lis r3, __CODE_END@h
    ori r3, r3, __CODE_END@l
    blr
