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
#include "nn_act_functions.h"

u32 nn_act_handle __attribute__((section(".data"))) = 0;

EXPORT_DECL(void, nn_act_Initialize, void);
EXPORT_DECL(void, nn_act_Finalize, void);
EXPORT_DECL(u8, nn_act_GetSlotNo, void);
EXPORT_DECL(u8, nn_act_GetDefaultAccount, void);
EXPORT_DECL(u32, nn_act_GetPersistentIdEx, u8 slot);

void InitAcquireACT(void) {
    if(coreinit_handle == 0) {
        InitAcquireOS();
    };
    OSDynLoad_Acquire("nn_act.rpl", &nn_act_handle);
}

void InitACTFunctionPointers(void) {
    u32 *funcPointer = 0;
    InitAcquireACT();

    OS_FIND_EXPORT_EX(nn_act_handle, Initialize__Q2_2nn3actFv, nn_act_Initialize)
    OS_FIND_EXPORT_EX(nn_act_handle, Finalize__Q2_2nn3actFv, nn_act_Finalize)
    OS_FIND_EXPORT_EX(nn_act_handle, GetSlotNo__Q2_2nn3actFv, nn_act_GetSlotNo)
    OS_FIND_EXPORT_EX(nn_act_handle, GetDefaultAccount__Q2_2nn3actFv, nn_act_GetDefaultAccount)
    OS_FIND_EXPORT_EX(nn_act_handle, GetPersistentIdEx__Q2_2nn3actFUc, nn_act_GetPersistentIdEx)
}
