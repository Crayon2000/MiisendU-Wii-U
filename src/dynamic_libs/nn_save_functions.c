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
#include "nn_save_functions.h"

u32 nn_save_handle __attribute__((section(".data"))) = 0;


EXPORT_DECL(void, SAVEInit, void);
EXPORT_DECL(int, SAVEOpenFile, void *pClient, void *pCmd, unsigned char user, const char *path, const char *mode, int *fd, int errHandling);

void InitAcquireSave(void) {
    if(coreinit_handle == 0) {
        InitAcquireOS();
    };
    OSDynLoad_Acquire("nn_save.rpl", &nn_save_handle);
}

void InitSaveFunctionPointers(void) {
    u32 *funcPointer = 0;
    InitAcquireSave();
    OS_FIND_EXPORT(nn_save_handle, SAVEInit);
    OS_FIND_EXPORT(nn_save_handle, SAVEOpenFile);
}
