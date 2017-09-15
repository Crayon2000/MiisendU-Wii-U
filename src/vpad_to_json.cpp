#include <stdio.h>
#include <math.h>
#include "vpad_to_json.h"

/**
 * Convert GamePad data to JSON string used by UsendMii.
 * @param[in] pad_data Controllers data.
 * @param[out] out Buffer where to copy the formatted data.
 * @param[in] out_size Size of the out buffer.
 */
void pad_to_json(PADData pad_data, char* out, u32 out_size)
{
    VPADTPData TPCalibrated;
    VPADGetTPCalibratedPoint(0, &TPCalibrated, &pad_data.vpad->tpdata);

    // Adjust calibrated screen coordinates to 854x480 resolution
    const u16 x = round(TPCalibrated.x * 854.0 / 1280.0);
    const u16 y = round(TPCalibrated.y * 480.0 / 720.0);

    snprintf(out, out_size,
        "{" \
        "\"hold\":%lu," \
        "\"tpTouch\":%u," \
        "\"tpValidity\":%u," \
        "\"tpX\":%d," \
        "\"tpY\":%d," \
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
        pad_data.vpad->btns_h,
        TPCalibrated.touched,
        TPCalibrated.invalid,
        x,
        y,
        pad_data.vpad->volume,
        pad_data.vpad->lstick.x,
        pad_data.vpad->lstick.y,
        pad_data.vpad->rstick.x,
        pad_data.vpad->rstick.y,
        pad_data.vpad->gyro.x,
        pad_data.vpad->gyro.y,
        pad_data.vpad->gyro.z,
        pad_data.vpad->angle.x,
        pad_data.vpad->angle.y,
        pad_data.vpad->angle.z,
        pad_data.vpad->acc.x,
        pad_data.vpad->acc.y,
        pad_data.vpad->acc.z,
        pad_data.vpad->dir.X.x,
        pad_data.vpad->dir.X.y,
        pad_data.vpad->dir.X.z,
        pad_data.vpad->dir.Y.x,
        pad_data.vpad->dir.Y.y,
        pad_data.vpad->dir.Y.z,
        pad_data.vpad->dir.Z.x,
        pad_data.vpad->dir.Z.y,
        pad_data.vpad->dir.Z.z);
}
