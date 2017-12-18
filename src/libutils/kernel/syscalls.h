#ifndef __SYSCALLS_H_
#define __SYSCALLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defs.h"

void SC0x25_KernelCopyData(u32 addr, u32 src, u32 len);

#ifdef __cplusplus
}
#endif

#endif // __KERNEL_FUNCTIONS_H_
