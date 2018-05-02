#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "FSOSUtils.h"
#include <dynamic_libs/fs_defs.h>
#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/fs_functions.h>

s32 FSOSUtils::MountFS(void *pClient, void *pCmd, char **mount_path) {
    InitOSFunctionPointers();
    InitFSFunctionPointers();
    s32 result = -1;

    void *mountSrc = malloc(FS_MOUNT_SOURCE_SIZE);
    if(!mountSrc)
        return -3;

    char* mountPath = (char*) malloc(FS_MAX_MOUNTPATH_SIZE);
    if(!mountPath) {
        free(mountSrc);
        mountSrc = NULL;
        return -4;
    }

    memset(mountSrc, 0, FS_MOUNT_SOURCE_SIZE);
    memset(mountPath, 0, FS_MAX_MOUNTPATH_SIZE);

    // Mount sdcard
    if (FSGetMountSource(pClient, pCmd, FS_SOURCETYPE_EXTERNAL, mountSrc, -1) == 0) {
        result = FSMount(pClient, pCmd, mountSrc, mountPath, FS_MAX_MOUNTPATH_SIZE, -1);
        if((result == 0) && mount_path) {
            *mount_path = (char*)malloc(strlen(mountPath) + 1);
            if(*mount_path)
                strcpy(*mount_path, mountPath);
        }
    }

    free(mountPath);
    free(mountSrc);

    mountPath = NULL;
    mountSrc = NULL;

    return result;
}

s32 FSOSUtils::UmountFS(void *pClient, void *pCmd, const char *mountPath) {
    s32 result = -1;
    result = FSUnmount(pClient, pCmd, mountPath, -1);

    return result;
}
