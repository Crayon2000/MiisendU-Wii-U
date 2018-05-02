#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "FSUtils.h"
#include "CFile.hpp"
#include "utils/logger.h"

s32 FSUtils::LoadFileToMem(const char *filepath, u8 **inbuffer, u32 *size) {
    //! always initialze input
    *inbuffer = NULL;
    if(size)
        *size = 0;

    s32 iFd = open(filepath, O_RDONLY);
    if (iFd < 0)
        return -1;

    u32 filesize = lseek(iFd, 0, SEEK_END);
    lseek(iFd, 0, SEEK_SET);

    u8 *buffer = (u8 *) malloc(filesize);
    if (buffer == NULL) {
        close(iFd);
        return -2;
    }

    u32 blocksize = 0x4000;
    u32 done = 0;
    s32 readBytes = 0;

    while(done < filesize) {
        if(done + blocksize > filesize) {
            blocksize = filesize - done;
        }
        readBytes = read(iFd, buffer + done, blocksize);
        if(readBytes <= 0)
            break;
        done += readBytes;
    }

    close(iFd);

    if (done != filesize) {
        free(buffer);
        buffer = NULL;
        return -3;
    }

    *inbuffer = buffer;

    //! sign is optional input
    if(size) {
        *size = filesize;
    }

    return filesize;
}

s32 FSUtils::CheckFile(const char * filepath) {
    if(!filepath)
        return 0;

    struct stat filestat;

    char dirnoslash[strlen(filepath)+2];
    snprintf(dirnoslash, sizeof(dirnoslash), "%s", filepath);

    while(dirnoslash[strlen(dirnoslash)-1] == '/')
        dirnoslash[strlen(dirnoslash)-1] = '\0';

    char * notRoot = strrchr(dirnoslash, '/');
    if(!notRoot) {
        strcat(dirnoslash, "/");
    }

    if (stat(dirnoslash, &filestat) == 0)
        return 1;

    return 0;
}

s32 FSUtils::CreateSubfolder(const char * fullpath) {
    if(!fullpath)
        return 0;

    s32 result = 0;

    char dirnoslash[strlen(fullpath)+1];
    strcpy(dirnoslash, fullpath);

    s32 pos = strlen(dirnoslash)-1;
    while(dirnoslash[pos] == '/') {
        dirnoslash[pos] = '\0';
        pos--;
    }

    if(CheckFile(dirnoslash)) {
        return 1;
    } else {
        char parentpath[strlen(dirnoslash)+2];
        strcpy(parentpath, dirnoslash);
        char * ptr = strrchr(parentpath, '/');

        if(!ptr) {
            //!Device root directory (must be with '/')
            strcat(parentpath, "/");
            struct stat filestat;
            if (stat(parentpath, &filestat) == 0)
                return 1;

            return 0;
        }

        ptr++;
        ptr[0] = '\0';

        result = CreateSubfolder(parentpath);
    }

    if(!result)
        return 0;

    if (mkdir(dirnoslash, 0777) == -1) {
        return 0;
    }

    return 1;
}

bool FSUtils::saveBufferToFile(const char * path, void * buffer, u32 size) {
    s32 res = open(path, O_CREAT | O_TRUNC | O_WRONLY);
    close(res);
    CFile file(path, CFile::WriteOnly);
    if (!file.isOpen()) {
        DEBUG_FUNCTION_LINE("Failed to open %s\n",path);
        return false;
    }
    file.write((const u8*) buffer,size);
    file.close();
    return true;
}

