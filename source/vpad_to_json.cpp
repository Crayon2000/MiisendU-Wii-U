#include <string.h>
#include <jansson.h>
#include "vpad_to_json.h"

/**
 * Convert GamePad data to JSON string used by UsendMii.
 * @param[in] pad_data Controllers data.
 * @param[out] out Buffer where to copy the formatted data.
 * @param[in] out_size Size of the out buffer.
 */
void pad_to_json(PADData pad_data, char* out, uint32_t out_size)
{
    VPADTouchData TPCalibrated;
    VPADGetTPCalibratedPointEx(VPAD_CHAN_0, VPAD_TP_854X480, &TPCalibrated, &pad_data.vpad->tpNormal);

    json_t *root = json_object();

    // Wii U GamePad
    json_t *wiiugamepad = json_object();
    json_object_set_new_nocheck(root, "wiiUGamePad", wiiugamepad);
    json_object_set_new_nocheck(wiiugamepad, "hold", json_integer(pad_data.vpad->hold));
    json_object_set_new_nocheck(wiiugamepad, "tpTouch", json_integer(TPCalibrated.touched));
    json_object_set_new_nocheck(wiiugamepad, "tpValidity", json_integer(TPCalibrated.validity));
    json_object_set_new_nocheck(wiiugamepad, "tpX", json_integer(TPCalibrated.x));
    json_object_set_new_nocheck(wiiugamepad, "tpY", json_integer(TPCalibrated.y));
    json_object_set_new_nocheck(wiiugamepad, "volume", json_integer(pad_data.vpad->slideVolume));
    json_object_set_new_nocheck(wiiugamepad, "lStickX", json_real(pad_data.vpad->leftStick.x));
    json_object_set_new_nocheck(wiiugamepad, "lStickY", json_real(pad_data.vpad->leftStick.y));
    json_object_set_new_nocheck(wiiugamepad, "rStickX", json_real(pad_data.vpad->rightStick.x));
    json_object_set_new_nocheck(wiiugamepad, "rStickY", json_real(pad_data.vpad->rightStick.y));
    json_object_set_new_nocheck(wiiugamepad, "gyroX", json_real(pad_data.vpad->gyro.x));
    json_object_set_new_nocheck(wiiugamepad, "gyroY", json_real(pad_data.vpad->gyro.y));
    json_object_set_new_nocheck(wiiugamepad, "gyroZ", json_real(pad_data.vpad->gyro.z));
    json_object_set_new_nocheck(wiiugamepad, "angleX", json_real(pad_data.vpad->angle.x));
    json_object_set_new_nocheck(wiiugamepad, "angleY", json_real(pad_data.vpad->angle.y));
    json_object_set_new_nocheck(wiiugamepad, "angleZ", json_real(pad_data.vpad->angle.z));
    json_object_set_new_nocheck(wiiugamepad, "accX", json_real(pad_data.vpad->accelorometer.acc.x));
    json_object_set_new_nocheck(wiiugamepad, "accY", json_real(pad_data.vpad->accelorometer.acc.y));
    json_object_set_new_nocheck(wiiugamepad, "accZ", json_real(pad_data.vpad->accelorometer.acc.z));
    json_object_set_new_nocheck(wiiugamepad, "dirXx", json_real(pad_data.vpad->direction.x.x));
    json_object_set_new_nocheck(wiiugamepad, "dirXy", json_real(pad_data.vpad->direction.x.y));
    json_object_set_new_nocheck(wiiugamepad, "dirXz", json_real(pad_data.vpad->direction.x.z));
    json_object_set_new_nocheck(wiiugamepad, "dirYx", json_real(pad_data.vpad->direction.y.x));
    json_object_set_new_nocheck(wiiugamepad, "dirYy", json_real(pad_data.vpad->direction.y.y));
    json_object_set_new_nocheck(wiiugamepad, "dirYz", json_real(pad_data.vpad->direction.y.z));
    json_object_set_new_nocheck(wiiugamepad, "dirZx", json_real(pad_data.vpad->direction.z.x));
    json_object_set_new_nocheck(wiiugamepad, "dirZy", json_real(pad_data.vpad->direction.z.y));
    json_object_set_new_nocheck(wiiugamepad, "dirZz", json_real(pad_data.vpad->direction.z.z));

    // Wii Remotes / Wii U Pro Controllers
    if(pad_data.kpad[0] != NULL ||
       pad_data.kpad[1] != NULL ||
       pad_data.kpad[2] != NULL ||
       pad_data.kpad[3] != NULL)
    {
        json_t *wiiremotes = json_array();
        json_object_set_new_nocheck(root, "wiiRemotes", wiiremotes);
        json_t *wiiuprocontrollers = json_array();
        json_object_set_new_nocheck(root, "wiiUProControllers", wiiuprocontrollers);
        for(int i = 0; i < 4; ++i)
        {
            if(pad_data.kpad[i] != NULL)
            {
                if(pad_data.kpad[i]->extensionType != WPAD_EXT_PRO_CONTROLLER)
                {   // Wii Remote with or without an extension
                    json_t *wiiremote = json_object();
                    json_object_set_new_nocheck(wiiremote, "order", json_integer(i + 1));
                    json_object_set_new_nocheck(wiiremote, "hold", json_integer(pad_data.kpad[i]->hold));
                    json_object_set_new_nocheck(wiiremote, "posX", json_integer(pad_data.kpad[i]->pos.x));
                    json_object_set_new_nocheck(wiiremote, "posY", json_integer(pad_data.kpad[i]->pos.y));
                    json_object_set_new_nocheck(wiiremote, "angleX", json_real(pad_data.kpad[i]->angle.x));
                    json_object_set_new_nocheck(wiiremote, "angleY", json_real(pad_data.kpad[i]->angle.y));
                    switch(pad_data.kpad[i]->extensionType)
                    {
                        case WPAD_EXT_NUNCHUK:
                        case WPAD_EXT_MPLUS_NUNCHUK:
                            { // Nunchuk
                                json_t *extension = json_object();
                                json_object_set_new_nocheck(wiiremote, "extension", extension);
                                json_object_set_new_nocheck(extension, "type", json_string("nunchuk"));
                                json_object_set_new_nocheck(extension, "hold", json_integer(pad_data.kpad[i]->nunchuck.hold));
                                json_object_set_new_nocheck(extension, "stickX", json_real(pad_data.kpad[i]->nunchuck.stick.x));
                                json_object_set_new_nocheck(extension, "stickY", json_real(pad_data.kpad[i]->nunchuck.stick.y));
                            }
                            break;
                        case WPAD_EXT_CLASSIC:
                        case WPAD_EXT_MPLUS_CLASSIC:
                            { // Classic Controller
                                json_t *extension = json_object();
                                json_object_set_new_nocheck(wiiremote, "extension", extension);
                                json_object_set_new_nocheck(extension, "type", json_string("classic"));
                                json_object_set_new_nocheck(extension, "hold", json_integer(pad_data.kpad[i]->classic.hold));
                                json_object_set_new_nocheck(extension, "lStickX", json_real(pad_data.kpad[i]->classic.leftStick.x));
                                json_object_set_new_nocheck(extension, "lStickY", json_real(pad_data.kpad[i]->classic.leftStick.y));
                                json_object_set_new_nocheck(extension, "rStickX", json_real(pad_data.kpad[i]->classic.rightStick.x));
                                json_object_set_new_nocheck(extension, "rStickY", json_real(pad_data.kpad[i]->classic.rightStick.y));
                                json_object_set_new_nocheck(extension, "lTrigger", json_real(pad_data.kpad[i]->classic.leftTrigger));
                                json_object_set_new_nocheck(extension, "rTrigger", json_real(pad_data.kpad[i]->classic.rightTrigger));
                            }
                            break;
                        default:
                            break;
                    }
                    json_array_append(wiiremotes, wiiremote);
                }
                else
                {   // Wii U Pro Controller
                    json_t *wiiuprocontroller = json_object();
                    json_object_set_new_nocheck(wiiuprocontroller, "order", json_integer(i + 1));
                    json_object_set_new_nocheck(wiiuprocontroller, "hold", json_integer(pad_data.kpad[i]->pro.hold));
                    json_object_set_new_nocheck(wiiuprocontroller, "lStickX", json_real(pad_data.kpad[i]->pro.leftStick.x));
                    json_object_set_new_nocheck(wiiuprocontroller, "lStickY", json_real(pad_data.kpad[i]->pro.leftStick.y));
                    json_object_set_new_nocheck(wiiuprocontroller, "rStickX", json_real(pad_data.kpad[i]->pro.rightStick.x));
                    json_object_set_new_nocheck(wiiuprocontroller, "rStickY", json_real(pad_data.kpad[i]->pro.rightStick.y));
                    json_array_append(wiiuprocontrollers, wiiuprocontroller);
                }
            }
        }
    }

    // Convert to string
    char* s = json_dumps(root, JSON_COMPACT | JSON_REAL_PRECISION(10));
    strncpy(out, s, out_size);
    free(s);

    json_decref(root);
}
