#ifndef _UTILS_NET_H_
#define _UTILS_NET_H_

#include <dynamic_libs/os_types.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 recvwait(s32 sock, void *buffer, s32 len);
u8 recvbyte(s32 sock);
u32 recvword(s32 sock);
s32 checkbyte(s32 sock);
s32 sendwait(s32 sock, const void *buffer, s32 len);
s32 sendbyte(s32 sock, unsigned char byte);

#ifdef __cplusplus
}
#endif

#endif
