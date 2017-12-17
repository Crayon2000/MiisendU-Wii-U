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
#ifndef __NN_ACT_FUNCTIONS_H_
#define __NN_ACT_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "os_types.h"

extern u32 nn_act_handle;

extern void(* nn_act_Initialize)(void);
extern void(* nn_act_Finalize)(void);
extern u8(* nn_act_GetSlotNo)(void);
extern u8(* nn_act_GetDefaultAccount)(void);
extern u32(*nn_act_GetPersistentIdEx)(u8 slot);

void InitACTFunctionPointers(void);
void InitAcquireACT(void);

#ifdef __cplusplus
}
#endif

#endif // __NN_ACT_FUNCTIONS_H_
