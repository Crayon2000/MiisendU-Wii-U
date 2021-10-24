#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* A ripoff of logger.h */

void udp_init(const char * ip, unsigned short ipport);
void udp_deinit(void);
void udp_print(const char *str);

#ifdef __cplusplus
}
#endif
