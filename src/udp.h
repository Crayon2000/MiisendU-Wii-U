#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* A ripoff of logger.h */

void udp_init(const char * ip);
void udp_deinit(void);
void udp_print(const char *str);
void udp_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif
