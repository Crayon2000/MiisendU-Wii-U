#include <string.h>
#include "rapidjson/writer.h"
#include <map>
#include <unordered_map>
#include "vpad_to_json.h"

/**
 * Mask for the Gamecube Controller.
 */
static const std::map gcmask = {
    std::pair{HPAD_BUTTON_LEFT, 0x0001},
    {HPAD_BUTTON_RIGHT, 0x0002},
    {HPAD_BUTTON_DOWN, 0x0004},
    {HPAD_BUTTON_UP, 0x0008},
    {HPAD_TRIGGER_Z, 0x0010},
    {HPAD_TRIGGER_R, 0x0020},
    {HPAD_TRIGGER_L, 0x0040},
    {HPAD_BUTTON_A, 0x0100},
    {HPAD_BUTTON_B, 0x0200},
    {HPAD_BUTTON_X, 0x0400},
    {HPAD_BUTTON_Y, 0x0800},
    {HPAD_BUTTON_START, 0x1000},
};

/**
 * Change a value from one range to another.
 * @param[in] value Value to change.
 * @param[in] oldMin Old minimum value.
 * @param[in] oldMax Old maximum value.
 * @param[in] newMin New minimum value.
 * @param[in] newMax New maximum value.
 * @return Value in the new range.
 */
[[nodiscard]] static constexpr int change_range(int value, int oldMin, int oldMax, int newMin, int newMax)
{
    return static_cast<int>((static_cast<float>(value - oldMin) / (oldMax - oldMin)) * (newMax - newMin) + newMin);
}

/**
 * Convert GamePad data to JSON string used by UsendMii.
 * @param[in] pad_data Controllers data.
 * @return Returns the JSON string.
 */
std::string_view pad_to_json(PADData pad_data)
{
    VPADTouchData TPCalibrated;
    VPADGetTPCalibratedPointEx(VPAD_CHAN_0, VPAD_TP_854X480, &TPCalibrated, &pad_data.vpad->tpNormal);

    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.SetMaxDecimalPlaces(10);

    writer.StartObject(); // Start root object

    // Wii U GamePad
    writer.Key("wiiUGamePad");
    writer.StartObject();
    writer.Key("hold");
    writer.Uint(pad_data.vpad->hold);
    writer.Key("tpTouch");
    writer.Uint(TPCalibrated.touched);
    writer.Key("tpValidity");
    writer.Uint(TPCalibrated.validity);
    writer.Key("tpX");
    writer.Uint(TPCalibrated.x);
    writer.Key("tpY");
    writer.Uint(TPCalibrated.y);
    writer.Key("volume");
    writer.Uint(pad_data.vpad->slideVolume);
    writer.Key("lStickX");
    writer.Double(pad_data.vpad->leftStick.x);
    writer.Key("lStickY");
    writer.Double(pad_data.vpad->leftStick.y);
    writer.Key("rStickX");
    writer.Double(pad_data.vpad->rightStick.x);
    writer.Key("rStickY");
    writer.Double(pad_data.vpad->rightStick.y);
    writer.Key("gyroX");
    writer.Double(pad_data.vpad->gyro.x);
    writer.Key("gyroY");
    writer.Double(pad_data.vpad->gyro.y);
    writer.Key("gyroZ");
    writer.Double(pad_data.vpad->gyro.z);
    writer.Key("angleX");
    writer.Double(pad_data.vpad->angle.x);
    writer.Key("angleY");
    writer.Double(pad_data.vpad->angle.y);
    writer.Key("angleZ");
    writer.Double(pad_data.vpad->angle.z);
    writer.Key("accX");
    writer.Double(pad_data.vpad->accelorometer.acc.x);
    writer.Key("accY");
    writer.Double(pad_data.vpad->accelorometer.acc.y);
    writer.Key("accZ");
    writer.Double(pad_data.vpad->accelorometer.acc.z);
    writer.Key("dirXx");
    writer.Double(pad_data.vpad->direction.x.x);
    writer.Key("dirXy");
    writer.Double(pad_data.vpad->direction.x.y);
    writer.Key("dirXz");
    writer.Double(pad_data.vpad->direction.x.z);
    writer.Key("dirYx");
    writer.Double(pad_data.vpad->direction.y.x);
    writer.Key("dirYy");
    writer.Double(pad_data.vpad->direction.y.y);
    writer.Key("dirYz");
    writer.Double(pad_data.vpad->direction.y.z);
    writer.Key("dirZx");
    writer.Double(pad_data.vpad->direction.z.x);
    writer.Key("dirZy");
    writer.Double(pad_data.vpad->direction.z.y);
    writer.Key("dirZz");
    writer.Double(pad_data.vpad->direction.z.z);
    writer.EndObject(); // End wiiUGamePad object

    // Wii Remotes / Wii U Pro Controllers
    std::unordered_map<uint8_t, KPADStatus*> wii_remotes_status;
    std::unordered_map<uint8_t, KPADStatus*> wii_u_pro_status;
    for(uint8_t i = 0; i < 4; ++i)
    {
        if(pad_data.kpad[i] != nullptr)
        {
            if(pad_data.kpad[i]->extensionType != WPAD_EXT_PRO_CONTROLLER)
            {   // Wii Remote with or without an extension
                wii_remotes_status.emplace(i + 1, pad_data.kpad[i]);
            }
            else
            {   // Wii U Pro Controller
                wii_u_pro_status.emplace(i + 1, pad_data.kpad[i]);
            }
        }
    }
    if (wii_remotes_status.empty() == false) {
        writer.Key("wiiRemotes");
        writer.StartArray();
        for (const auto& [order, kpad] : wii_remotes_status) {
            writer.StartObject(); // Start wiiremote object
            writer.Key("order");
            writer.Uint(order);
            writer.Key("hold");
            writer.Uint(kpad->hold);
            writer.Key("posX");
            writer.Int(kpad->pos.x);
            writer.Key("posY");
            writer.Int(kpad->pos.y);
            writer.Key("angleX");
            writer.Double(kpad->angle.x);
            writer.Key("angleY");
            writer.Double(kpad->angle.y);
            switch(kpad->extensionType)
            {
                case WPAD_EXT_NUNCHUK:
                case WPAD_EXT_MPLUS_NUNCHUK:
                    { // Nunchuk
                        writer.Key("extension");
                        writer.StartObject();
                        writer.Key("type");
                        writer.String("nunchuk");
                        writer.Key("hold");
                        writer.Uint(kpad->nunchuk.hold);
                        writer.Key("stickX");
                        writer.Double(kpad->nunchuk.stick.x);
                        writer.Key("stickY");
                        writer.Double(kpad->nunchuk.stick.y);
                        writer.EndObject();
                    }
                    break;
                case WPAD_EXT_CLASSIC:
                case WPAD_EXT_MPLUS_CLASSIC:
                    { // Classic Controller
                        writer.Key("extension");
                        writer.StartObject();
                        writer.Key("type");
                        writer.String("classic");
                        writer.Key("hold");
                        writer.Uint(kpad->classic.hold);
                        writer.Key("lStickX");
                        writer.Double(kpad->classic.leftStick.x);
                        writer.Key("lStickY");
                        writer.Double(kpad->classic.leftStick.y);
                        writer.Key("rStickX");
                        writer.Double(kpad->classic.rightStick.x);
                        writer.Key("rStickY");
                        writer.Double(kpad->classic.rightStick.y);
                        writer.Key("lTrigger");
                        writer.Double(kpad->classic.leftTrigger);
                        writer.Key("rTrigger");
                        writer.Double(kpad->classic.rightTrigger);
                        writer.EndObject();
                    }
                    break;
                default:
                    break;
            }
            writer.EndObject(); // End wiiremote object
        }
        writer.EndArray();
    }
    if (wii_u_pro_status.empty() == false) {
        writer.Key("wiiUProControllers");
        writer.StartArray();
        for (const auto& [order, kpad] : wii_remotes_status) {
            writer.StartObject(); // Start wiiuprocontrollers object
            writer.Key("order");
            writer.Uint(order);
            writer.Key("hold");
            writer.Int(kpad->pro.hold);
            writer.Key("lStickX");
            writer.Double(kpad->pro.leftStick.x);
            writer.Key("lStickY");
            writer.Double(kpad->pro.leftStick.y);
            writer.Key("rStickX");
            writer.Double(kpad->pro.rightStick.x);
            writer.Key("rStickY");
            writer.Double(kpad->pro.rightStick.y);
            writer.EndObject(); // End wiiuprocontrollers object
        }
        writer.EndArray();
    }

    // USB Gamecube Controller Adapter
    if(pad_data.hpad[0] != nullptr ||
       pad_data.hpad[1] != nullptr ||
       pad_data.hpad[2] != nullptr ||
       pad_data.hpad[3] != nullptr)
    {
        writer.Key("gameCubeControllers");
        writer.StartArray();
        for(int i = 0; i < 4; ++i)
        {
            if(pad_data.hpad[i] == nullptr)
            {
                continue;
            }
            int32_t holdgc = 0;
            const int32_t badgc = pad_data.hpad[i]->hold;
            for (auto const& [oldid, newid] : gcmask)
            {
                if(badgc & oldid) {
                    holdgc |= newid;
                }
            }

            writer.StartObject(); // Start gccontroller object
            writer.Key("order");
            writer.Uint(i + 1);
            writer.Key("hold");
            writer.Int(holdgc);
            writer.Key("ctrlStickX");
            writer.Int(change_range(pad_data.hpad[i]->stickX, HPAD_STICK_AXIS_MIN, HPAD_STICK_AXIS_MAX, -128, 127));
            writer.Key("ctrlStickY");
            writer.Int(change_range(pad_data.hpad[i]->stickY, HPAD_STICK_AXIS_MIN, HPAD_STICK_AXIS_MAX, -128, 127));
            writer.Key("cStickX");
            writer.Int(change_range(pad_data.hpad[i]->substickX, HPAD_SUBSTICK_AXIS_MIN, HPAD_SUBSTICK_AXIS_MAX, -128, 127));
            writer.Key("cStickY");
            writer.Int(change_range(pad_data.hpad[i]->substickY, HPAD_SUBSTICK_AXIS_MIN, HPAD_SUBSTICK_AXIS_MAX, -128, 127));
            writer.Key("lTrigger");
            writer.Uint(change_range(pad_data.hpad[i]->triggerL, HPAD_TRIGGER_MIN, HPAD_TRIGGER_MAX, 0, 255));
            writer.Key("rTrigger");
            writer.Uint(change_range(pad_data.hpad[i]->triggerR, HPAD_TRIGGER_MIN, HPAD_TRIGGER_MAX, 0, 255));
            writer.EndObject(); // Start gccontroller object
        }
        writer.EndArray();
    }

    writer.EndObject(); // End root object

    // Convert to string_view
    return std::string_view(sb.GetString(), sb.GetSize());
}
