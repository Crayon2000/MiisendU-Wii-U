#ifndef PROGRAM_H
#define PROGRAM_H

#include <nsysnet/socket.h>

// Should be in socket.h
char *
inet_ntoa_r(struct in_addr in, char * buf);

const char *
inet_ntop(int af, const void *src, char *dst, socklen_t size);

int
inet_pton(int af, const char *src, void *dst);

#endif /* PROGRAM_H */
