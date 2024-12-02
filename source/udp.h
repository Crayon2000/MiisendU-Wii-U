#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void udp_init(const char * ipString, uint16_t ipport);
void udp_deinit(void);
bool udp_print(const char *str);

#ifdef __cplusplus
}
#endif
