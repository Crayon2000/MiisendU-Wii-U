#ifndef __NN_FP_FUNCTIONS_H_
#define __NN_FP_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "os_types.h"

/* Handle for coreinit */
extern u32 nn_fp_handle;

extern void(* nn_fp_GetMyPresence)(void *);
extern void(* nn_fp_Initialize)(void);

void InitAcquireFp(void);
void InitFpFunctionPointers(void);

#ifdef __cplusplus
}
#endif

#endif // __NN_FP_FUNCTIONS_H_
