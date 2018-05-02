#include "net.h"
#include <dynamic_libs/os_functions.h>
#include <dynamic_libs/socket_functions.h>

static volatile int socket_lock __attribute__((section(".data"))) = 0;

s32 recvwait(s32 sock, void *buffer, s32 len) {
    while(socket_lock) {
        os_usleep(1000);
    }
    s32 ret;
    while (len > 0) {
        ret = recv(sock, buffer, len, 0);
        if(ret < 0) {
            socket_lock = 0;
            return ret;
        }
        len -= ret;
        buffer =  (void *)(((char *) buffer) + ret);
    }
    socket_lock = 0;
    return 0;
}

u8 recvbyte(s32 sock) {
    unsigned char buffer[1];
    s32 ret;

    ret = recvwait(sock, buffer, 1);
    if (ret < 0) return ret;
    return buffer[0];
}

u32 recvword(s32 sock) {
    u32 result;
    s32 ret;

    ret = recvwait(sock, &result, 4);
    if (ret < 0) return ret;
    return result;
}

s32 checkbyte(s32 sock) {
    while(socket_lock) {
        os_usleep(1000);
    }
    unsigned char buffer[1];
    s32 ret;

    ret = recv(sock, buffer, 1, MSG_DONTWAIT);
    socket_lock = 0;
    if (ret < 0) return ret;
    if (ret == 0) return -1;
    return buffer[0];
}

s32 sendwait(s32 sock, const void *buffer, s32 len) {
    while(socket_lock) {
        os_usleep(1000);
    }
    s32 ret;
    while (len > 0) {
        // For some reason the send blocks/crashes if the buffer is too big..
        int cur_length = len <= 0x30 ? len : 0x30;
        ret = send(sock, buffer, cur_length, 0);
        if(ret < 0) {
            socket_lock = 0;
            return ret;
        }
        len -= ret;
        buffer =  (void *)(((char *) buffer) + ret);
    }
    socket_lock = 0;
    return 0;
}

s32 sendbyte(s32 sock, unsigned char byte) {
    unsigned char buffer[1];
    buffer[0] = byte;
    return sendwait(sock, buffer, 1);
}
