/****************************************************************************
 * Copyright (C) 2015
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#include "os_functions.h"
#include "nn_fp_functions.h"

u32 nn_fp_handle __attribute__((section(".data"))) = 0;

EXPORT_DECL(void, nn_fp_GetMyPresence, void *);
EXPORT_DECL(void, nn_fp_Initialize, void);

void InitAcquireFp(void){
    if(coreinit_handle == 0){ InitAcquireOS(); };
    OSDynLoad_Acquire("nn_fp.rpl", &nn_fp_handle);
}

void InitFpFunctionPointers(void){
    u32 *funcPointer = 0;
    InitAcquireFp();
    
    OS_FIND_EXPORT_EX(nn_fp_handle, GetMyPresence__Q2_2nn2fpFPQ3_2nn2fp10MyPresence, nn_fp_GetMyPresence)
    OS_FIND_EXPORT_EX(nn_fp_handle, Initialize__Q2_2nn2fpFv, nn_fp_Initialize)
}
