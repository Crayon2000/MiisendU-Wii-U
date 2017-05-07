#include <stdio.h>
#include "vpad_to_json.h"

/**
 * Convert GamePad data to JSON string used by UsendMii.
 * @param[in] vpad_data GamePad data.
 * @param[out] out Buffer where to copy the formatted data.
 * @param[in] out_size Size of the out buffer.
 */
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
        "\"lStickX\":%0.8f," \
        "\"lStickY\":%0.8f," \
        "\"rStickX\":%0.8f," \
        "\"rStickY\":%0.8f" \
        "\"gyroX\":%0.8f" \
        "\"gyroY\":%0.8f" \
        "\"gyroZ\":%0.8f" \
        "\"angleX\":%0.8f" \
        "\"angleY\":%0.8f" \
        "\"angleZ\":%0.8f" \
        "\"accX\":%0.8f" \
        "\"accY\":%0.8f" \
        "\"accZ\":%0.8f" \
        "\"dirXx\":%0.8f" \
        "\"dirXy\":%0.8f" \
        "\"dirXz\":%0.8f" \
        "\"dirYx\":%0.8f" \
        "\"dirYy\":%0.8f" \
        "\"dirYz\":%0.8f" \
        "\"dirZx\":%0.8f" \
        "\"dirZy\":%0.8f" \
        "\"dirZz\":%0.8f" \
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
        vpad_data->rstick.y,
        vpad_data->gyro.x,
        vpad_data->gyro.y,
        vpad_data->gyro.z,
        vpad_data->angle.x,
        vpad_data->angle.y,
        vpad_data->angle.z,
        vpad_data->acc.x,
        vpad_data->acc.y,
        vpad_data->acc.z,
        vpad_data->dir.X.x,
        vpad_data->dir.X.y,
        vpad_data->dir.X.z,
        vpad_data->dir.Y.x,
        vpad_data->dir.Y.y,
        vpad_data->dir.Y.z,
        vpad_data->dir.Z.x,
        vpad_data->dir.Z.y,
        vpad_data->dir.Z.z);
}
