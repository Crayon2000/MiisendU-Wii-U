#ifndef __FS_UTILS_H_
#define __FS_UTILS_H_
#include <dynamic_libs/os_types.h>

class FSUtils {
public:
    static s32 LoadFileToMem(const char *filepath, u8 **inbuffer, u32 *size);

    //! todo: C++ class
    static s32 CreateSubfolder(const char * fullpath);
    static s32 CheckFile(const char * filepath);
    static bool saveBufferToFile(const char * path, void * buffer, u32 size);
};

#endif // __FS_UTILS_H_
