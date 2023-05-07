#include "udp.h"
#include <coreinit/memdefaultheap.h>
#include <coreinit/thread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* A ripoff of logger.c */

static int udp_socket = -1;
static volatile bool udp_lock = false;


void udp_init(const char * ipString, unsigned short ipport)
{
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(udp_socket < 0) {
        return;
    }

    struct sockaddr_in connect_addr;
    memset(&connect_addr, 0, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    connect_addr.sin_port = ipport;
    inet_aton(ipString, &connect_addr.sin_addr);

    if(connect(udp_socket, (struct sockaddr*)&connect_addr, sizeof(connect_addr)) < 0)
    {
        close(udp_socket);
        udp_socket = -1;
    }
}

void udp_deinit(void)
{
    if(udp_socket >= 0)
    {
        close(udp_socket);
        udp_socket = -1;
    }
}

void udp_print(const char *str)
{
    // socket is always 0 initially as it is in the BSS
    if(udp_socket < 0) {
        return;
    }

    while(udp_lock == true) {
        OSSleepTicks(OSMillisecondsToTicks(1));
    }
    udp_lock = true;

    int len = strlen(str);
    while (len > 0) {
        const int block = len < 1400 ? len : 1400; // take max 1400 bytes per UDP packet
        const int ret = send(udp_socket, str, block, 0);
        if(ret < 0) {
            break;
        }

        len -= ret;
        str += ret;
    }

    udp_lock = false;
}
