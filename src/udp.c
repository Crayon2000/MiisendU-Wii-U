#define _GNU_SOURCE

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "common/common.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "udp.h"

/* A ripoff of logger.c */

static int udp_socket = -1;
static volatile int udp_lock = 0;


void udp_init(const char * ipString, unsigned short ipport)
{
    udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_socket < 0)
        return;

    struct sockaddr_in connect_addr;
    memset(&connect_addr, 0, sizeof(connect_addr));
    connect_addr.sin_family = AF_INET;
    connect_addr.sin_port = ipport;
    inet_aton(ipString, &connect_addr.sin_addr);

    if(connect(udp_socket, (struct sockaddr*)&connect_addr, sizeof(connect_addr)) < 0)
    {
        socketclose(udp_socket);
        udp_socket = -1;
    }
}

void udp_deinit(void)
{
    if(udp_socket >= 0)
    {
        socketclose(udp_socket);
        udp_socket = -1;
    }
}

void udp_print(const char *str)
{
    // socket is always 0 initially as it is in the BSS
    if(udp_socket < 0) {
        return;
    }

    while(udp_lock)
        usleep(1000);
    udp_lock = 1;

    int len = strlen(str);
    while (len > 0) {
        int block = len < 1400 ? len : 1400; // take max 1400 bytes per UDP packet
        int ret = send(udp_socket, str, block, 0);
        if(ret < 0)
            break;

        len -= ret;
        str += ret;
    }

    udp_lock = 0;
}

void udp_printf(const char *format, ...)
{
    if(udp_socket < 0) {
        return;
    }

    char * tmp = NULL;

    va_list va;
    va_start(va, format);
    if((vasprintf(&tmp, format, va) >= 0) && tmp)
    {
        udp_print(tmp);
    }
    va_end(va);

    if(tmp)
        free(tmp);
}

// Similar to the parseBytes function from Luis Chavier on stackoverflow
void udp_string_to_ipv4(const char* str, unsigned char* bytes, unsigned char maxBytes)
{
    for (int i = 0; i < maxBytes; i++)
    {
        bytes[i] = atoi(str);             // Convert byte
        str = strchr(str, '.');           // Find next separator
        if (str == NULL || *str == '\0')
        {
            break;                        // No more separators, exit
        }
        str++;                            // Point to next character after separator
    }
}
