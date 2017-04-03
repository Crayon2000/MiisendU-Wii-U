#ifndef _VPADTOJSON_H_
#define	_VPADTOJSON_H_

#include "dynamic_libs/vpad_functions.h"

#ifdef __cplusplus
extern "C" {
#endif

void vpad_to_json(VPADData* vpad_data, char* out, u32 out_size);


#ifdef __cplusplus
}
#endif

#endif /* _VPADTOJSON_H_ */