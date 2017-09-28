#include <string.h>
#include <jansson.h>
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
    VPADGetTPCalibratedPointEx(0, VPAD_TP_854x480, &TPCalibrated, &pad_data.vpad->tpdata);

    // Wii U Gamepad
    json_t *wiiugamepad = json_object();
    json_object_set_new_nocheck(wiiugamepad, "hold", json_integer(pad_data.vpad->btns_h));
    json_object_set_new_nocheck(wiiugamepad, "tpTouch", json_integer(TPCalibrated.touched));
    json_object_set_new_nocheck(wiiugamepad, "tpValidity", json_integer(TPCalibrated.invalid));
    json_object_set_new_nocheck(wiiugamepad, "tpX", json_integer(TPCalibrated.x));
    json_object_set_new_nocheck(wiiugamepad, "tpY", json_integer(TPCalibrated.y));
    json_object_set_new_nocheck(wiiugamepad, "volume", json_integer(pad_data.vpad->volume));
    json_object_set_new_nocheck(wiiugamepad, "lStickX", json_real(pad_data.vpad->lstick.x));
    json_object_set_new_nocheck(wiiugamepad, "lStickY", json_real(pad_data.vpad->lstick.y));
    json_object_set_new_nocheck(wiiugamepad, "rStickX", json_real(pad_data.vpad->rstick.x));
    json_object_set_new_nocheck(wiiugamepad, "rStickY", json_real(pad_data.vpad->rstick.y));
    json_object_set_new_nocheck(wiiugamepad, "gyroX", json_real(pad_data.vpad->gyro.x));
    json_object_set_new_nocheck(wiiugamepad, "gyroY", json_real(pad_data.vpad->gyro.y));
    json_object_set_new_nocheck(wiiugamepad, "gyroZ", json_real(pad_data.vpad->gyro.z));
    json_object_set_new_nocheck(wiiugamepad, "angleX", json_real(pad_data.vpad->angle.x));
    json_object_set_new_nocheck(wiiugamepad, "angleY", json_real(pad_data.vpad->angle.y));
    json_object_set_new_nocheck(wiiugamepad, "angleZ", json_real(pad_data.vpad->angle.z));
    json_object_set_new_nocheck(wiiugamepad, "accX", json_real(pad_data.vpad->acc.x));
    json_object_set_new_nocheck(wiiugamepad, "accY", json_real(pad_data.vpad->acc.y));
    json_object_set_new_nocheck(wiiugamepad, "accZ", json_real(pad_data.vpad->acc.z));
    json_object_set_new_nocheck(wiiugamepad, "dirXx", json_real(pad_data.vpad->dir.X.x));
    json_object_set_new_nocheck(wiiugamepad, "dirXy", json_real(pad_data.vpad->dir.X.y));
    json_object_set_new_nocheck(wiiugamepad, "dirXz", json_real(pad_data.vpad->dir.X.z));
    json_object_set_new_nocheck(wiiugamepad, "dirYx", json_real(pad_data.vpad->dir.Y.x));
    json_object_set_new_nocheck(wiiugamepad, "dirYy", json_real(pad_data.vpad->dir.Y.y));
    json_object_set_new_nocheck(wiiugamepad, "dirYz", json_real(pad_data.vpad->dir.Y.z));
    json_object_set_new_nocheck(wiiugamepad, "dirZx", json_real(pad_data.vpad->dir.Z.x));
    json_object_set_new_nocheck(wiiugamepad, "dirZy", json_real(pad_data.vpad->dir.Z.y));
    json_object_set_new_nocheck(wiiugamepad, "dirZz", json_real(pad_data.vpad->dir.Z.z));

    // Convert to string
    char* s = json_dumps(wiiugamepad, JSON_COMPACT | JSON_REAL_PRECISION(10));
    strncpy(out, s, out_size);
    free(s);

    json_decref(wiiugamepad);
}
