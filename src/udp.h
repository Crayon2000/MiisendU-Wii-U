#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* A ripoff of logger.h */

void udp_init(const char * ip, unsigned short ipport);
void udp_deinit(void);
void udp_print(const char *str);
void udp_printf(const char *format, ...);

void udp_string_to_ipv4(const char* str, unsigned char* bytes, unsigned char maxBytes);

#ifdef __cplusplus
}
#endif
