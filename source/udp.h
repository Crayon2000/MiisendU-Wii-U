#pragma once

#include <cstdint>
#include <string_view>

#ifdef __cplusplus
extern "C" {
#endif

void udp_init(std::string_view ipString, uint16_t ipport);
void udp_deinit(void);
bool udp_print(std::string_view str);

#ifdef __cplusplus
}
#endif
