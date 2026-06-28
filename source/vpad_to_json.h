#pragma once

#include <array>
#include <padscore/kpad.h>
#include <vpad/input.h>
#include <nn/hpad/hpad.h>
#include <string_view>

/**
 * Structure to hold all controllers data.
 */
struct PADData {
    /**
     * Wii U Gamepad.
     */
    VPADStatus* vpad{};
    /**
     * Wii Remotes.
     */
    std::array<KPADStatus*, 4> kpad{};
    /**
     * USB Gamecube Controller Adapter.
     */
    std::array<HPADStatus*, 4> hpad{};
};

std::string_view pad_to_json(PADData pad_data);
