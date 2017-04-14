#include <stdio.h>
#include "vpad_to_json.h"

void vpad_to_json(VPADData* vpad_data, char* out, u32 out_size)
{
    s32 x = (vpad_data->tpdata.x - 100) * 854 / (3970 - 100);
    if(x > 854) x = 854;
    if(x < 0) x = 0;
    s32 y = 480 - ((vpad_data->tpdata.y - 165) * 480 / (3890 - 165));
    if(y > 480) y = 480;
    if(y < 0) y = 0;

    snprintf(out, out_size,
        "{" \
        "\"hold\":%lu," \
        "\"tpTouch\":%u," \
        "\"tpValidity\":%u," \
        "\"tpX\":%ld," \
        "\"tpY\":%ld," \
        "\"volume\":%d," \
        "\"lStickX\":%0.6f," \
        "\"lStickY\":%0.6f," \
        "\"rStickX\":%0.6f," \
        "\"rStickY\":%0.6f" \
        "}",
        vpad_data->btns_h,
        vpad_data->tpdata.touched,
        vpad_data->tpdata.invalid,
        x,
        y,
        vpad_data->volume,
        vpad_data->lstick.x,
        vpad_data->lstick.y,
        vpad_data->rstick.x,
        vpad_data->rstick.y);
}
