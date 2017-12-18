#ifndef __FS_OS_UTILS_H_
#define __FS_OS_UTILS_H_

#include <dynamic_libs/os_types.h>

class FSOSUtils{
    public:

    static s32 MountFS(void *pClient, void *pCmd, char **mount_path);
    static s32 UmountFS(void *pClient, void *pCmd, const char *mountPath);
};

#endif // __FS_OS_UTILS_H_
