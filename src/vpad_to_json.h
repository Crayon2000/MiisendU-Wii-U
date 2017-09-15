#ifndef _VPADTOJSON_H_
#define	_VPADTOJSON_H_

#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/padscore_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Struct to hold all controllers data.
 */
typedef struct {
    VPADData* vpad;     /**< Wii U Gamepad. */
    KPADData* kpad[4];  /**< Wii Remotes. */
} PADData;

void pad_to_json(PADData pad_data, char* out, u32 out_size);

#ifdef __cplusplus
}
#endif

#endif /* _VPADTOJSON_H_ */