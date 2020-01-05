#ifndef _VPADTOJSON_H_
#define _VPADTOJSON_H_

#include <wut_types.h>
#include <padscore/kpad.h>
#include <vpad/input.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Struct to hold all controllers data.
 */
typedef struct {
    VPADStatus* vpad;     /**< Wii U Gamepad. */
    KPADStatus* kpad[4];  /**< Wii Remotes. */
} PADData;

void pad_to_json(PADData pad_data, char* out, uint32_t out_size);

#ifdef __cplusplus
}
#endif

#endif /* _VPADTOJSON_H_ */
