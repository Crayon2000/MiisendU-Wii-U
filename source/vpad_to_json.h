#ifndef _VPADTOJSON_H_
#define _VPADTOJSON_H_

#include <wut_types.h>
#include <padscore/kpad.h>
#include <vpad/input.h>
#include <nn/hpad/hpad.h>
#include <string_view>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Struct to hold all controllers data.
 */
typedef struct {
    VPADStatus* vpad;     /**< Wii U Gamepad. */
    KPADStatus* kpad[4];  /**< Wii Remotes. */
    HPADStatus* hpad[4];  /**< USB Gamecube Controller Adapter. */
} PADData;

std::string_view pad_to_json(PADData pad_data);

#ifdef __cplusplus
}
#endif

#endif /* _VPADTOJSON_H_ */
