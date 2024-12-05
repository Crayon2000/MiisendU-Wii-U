#include "console.h"
#include "vpad_to_json.h"
#include "udp.h"
#include <whb/proc.h>
#include <coreinit/screen.h>
#include <padscore/kpad.h>
#include <vpad/input.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <nn/ac/ac_c.h>
#include <whb/sdcard.h>
#include <coreinit/cache.h>
#include <coreinit/thread.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>
#include <wut_types.h>
#include <ini.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Application configuration.
 */
typedef struct {
    const char* ipaddress;
    int port;
} configuration;

/**
 * Handler for ini parser.
 * @param user User data.
 * @param section Section name.
 * @param name Name.
 * @param value Value.
 * @return Returns nonzero on success, zero on error.
 */
static int handler(void* user, const char* section, const char* name, const char* value)
{
    configuration* pconfig = static_cast<configuration*>(user);
    if(strcmp(section, "server") == 0) {
        if(strcmp(name, "ipaddress") == 0) {
            pconfig->ipaddress = strdup(value);
        }
        else if(strcmp(name, "port") == 0) {
            pconfig->port = std::atoi(value);
        }
        else {
            return 0; // Unknown name
        }
    }
    else {
        return 0; // Unknown section
    }
    return 1;
}

/**
 * Print the header.
 * @param bufferNum The ID of the screen to place the pixel in.
 */
static void PrintHeader(OSScreenID bufferNum)
{
    OSScreenPutFontEx(bufferNum, -4, 0, " __  __ _ _                 _ _   _  __      ___ _   _   _ ");
    OSScreenPutFontEx(bufferNum, -4, 1, "|  \\/  (_|_)___ ___ _ _  __| | | | | \\ \\    / (_|_) | | | |");
    OSScreenPutFontEx(bufferNum, -4, 2, "| |\\/| | | (_-</ -_) ' \\/ _` | |_| |  \\ \\/\\/ /| | | | |_| |");
    OSScreenPutFontEx(bufferNum, -4, 3, "|_|  |_|_|_/__/\\___|_||_\\__,_|\\___/    \\_/\\_/ |_|_|  \\___/  v1.3.0");
}

/**
 * Reset orientation.
 */
static void ResetOrientation()
{
    // Posture with the gamepad up and straight in front of user
    static VPADDirection dir_base = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };

    VPADSetGyroAngle(VPAD_CHAN_0, 0.0f, 0.0f, 0.0f);
    VPADSetGyroDirection(VPAD_CHAN_0, &dir_base);
    VPADSetGyroDirReviseBase(VPAD_CHAN_0, &dir_base);
}

/**
 * Entry point.
 * @param argc An integer that contains the count of arguments.
 * @param argv An array of null-terminated strings representing command-line arguments.
 * @return Returns zero on success, nonzero on error.
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    uint8_t IP[4] = {192, 168, 1, 100};

    WHBProcInit();
    VPADInit();
    KPADInit();
    HPADInit();
    WPADEnableURCC(true);

    WHBMountSdCard();
    char path[256];
    char *sdRootPath = WHBGetSdCardMountPath();
    snprintf(path, sizeof(path), "%s/wiiu/apps/MiisendU-Wii-U/settings.ini", sdRootPath);

    // Init screen and screen buffers
    ConsoleInit();

    // Clear screens
    ConsoleDrawStart();

    // Gamepad key state data
    VPADReadError error;
    VPADStatus vpad_data;

    char IP_str[32];
    int8_t selected_digit = 0;

    // Read default settings from file
    bool ip_loaded = false;
    configuration config = {nullptr, 4242};
    ini_parse(path, handler, &config);
    unsigned short Port = config.port;
    if(config.ipaddress != nullptr) {
        if(inet_pton(AF_INET, config.ipaddress, &IP) > 0) {
            ip_loaded = true;
        }
        free((void*)config.ipaddress);
    }
    if (ip_loaded == false && NNResult_IsSuccess(ACInitialize())) {
        uint32_t ac_ip = 0;
        if (NNResult_IsSuccess(ACGetAssignedAddress(&ac_ip))) {
            IP[0] = (ac_ip >> 24) & 0xFF;
            IP[1] = (ac_ip >> 16) & 0xFF;
            IP[2] = (ac_ip >>  8) & 0xFF;
            IP[3] = (ac_ip >>  0) & 0xFF;
        }
        ACFinalize();
    }

    bool running = true;

    uint32_t wait_time_horizontal = 0;
    uint32_t wait_time_vertical = 0;
    const uint8_t wait_time = 14;

    // Insert the IP address (some code was taken from the IP Address selector of geckiine made by brienj)
    while(running == true) {
        VPADRead(VPAD_CHAN_0, &vpad_data, 1, &error);
        if (vpad_data.hold & VPAD_BUTTON_LEFT && selected_digit > 0) {
            if (vpad_data.trigger & VPAD_BUTTON_LEFT || wait_time_horizontal++ > wait_time) {
                selected_digit--;
                wait_time_horizontal = 0;
            }
        }
        if (vpad_data.hold & VPAD_BUTTON_RIGHT && selected_digit < 3) {
            if (vpad_data.trigger & VPAD_BUTTON_RIGHT || wait_time_horizontal++ > wait_time) {
                selected_digit++;
                wait_time_horizontal = 0;
            }
        }
        if (vpad_data.hold & VPAD_BUTTON_UP) {
            if (vpad_data.trigger & VPAD_BUTTON_UP || wait_time_vertical++ > wait_time) {
                IP[selected_digit] = (IP[selected_digit] < 255) ? (IP[selected_digit] + 1) : 0;
                wait_time_vertical = 0;
            }
        }
        if (vpad_data.hold & VPAD_BUTTON_DOWN) {
            if (vpad_data.trigger & VPAD_BUTTON_DOWN || wait_time_vertical++ > wait_time) {
                IP[selected_digit] = (IP[selected_digit] >   0) ? (IP[selected_digit] - 1) : 255;
                wait_time_vertical = 0;
            }
        }

        if(ConsoleDrawStart() == true) {
            // Print to DRC
            PrintHeader(SCREEN_DRC);
            OSScreenPutFontEx(SCREEN_DRC, 0, 5, "Please insert your computer's IP address below");
            OSScreenPutFontEx(SCREEN_DRC, 0, 6, "(use the DPAD to edit the IP address)");
            OSScreenPutFontEx(SCREEN_DRC, 4 * selected_digit, 8, "vvv");
            snprintf(IP_str, 32, "%3d.%3d.%3d.%3d", IP[0], IP[1], IP[2], IP[3]);
            OSScreenPutFontEx(SCREEN_DRC, 0, 9, IP_str);
            OSScreenPutFontEx(SCREEN_DRC, 0, 15, "Press 'A' to confirm");
            OSScreenPutFontEx(SCREEN_DRC, 0, 16, "Press the HOME button to exit");

            ConsoleDrawEnd();
        }

        if (vpad_data.trigger & VPAD_BUTTON_A) {
            break;
        }

        running = WHBProcIsRunning();
    }
    if(running == false) {
        WHBUnmountSdCard();
        ConsoleFree();
        WHBProcShutdown();
        return 0;
    }

    // Disallow TV Menu
    VPADSetTVMenuInvalid(VPAD_CHAN_0, true);

    bool fromHBL;
    if(OSIsHomeButtonMenuEnabled() == false) {
        fromHBL = true;
    }
    else {
        fromHBL = false;

        // Disable HOME button menu
        OSEnableHomeButtonMenu(false);
    }

    // Reset orientation
    ResetOrientation();

    // Get IP Address (without spaces)
    char IP_ADDRESS[32];
    snprintf(IP_ADDRESS, 32, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);

    // Initialize the UDP connection
    udp_init(IP_ADDRESS, Port);

    if(ConsoleDrawStart() == true) {
        // Output the IP address
        char msg_connected[255];
        snprintf(msg_connected, 255, "Connected to %s:%d", IP_ADDRESS, Port);

        // Print to TV
        PrintHeader(SCREEN_TV);
        OSScreenPutFontEx(SCREEN_TV, 0, 5, msg_connected);
        OSScreenPutFontEx(SCREEN_TV, 0, 7, "Remember the program will not work without");
        OSScreenPutFontEx(SCREEN_TV, 0, 8, "UsendMii running on your computer.");
        OSScreenPutFontEx(SCREEN_TV, 0, 9, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
        OSScreenPutFontEx(SCREEN_TV, 0, 16, "Hold the HOME button to exit.");

        // Print to DRC
        PrintHeader(SCREEN_DRC);
        OSScreenPutFontEx(SCREEN_DRC, 0, 5, msg_connected);
        OSScreenPutFontEx(SCREEN_DRC, 0, 7, "Remember the program will not work without");
        OSScreenPutFontEx(SCREEN_DRC, 0, 8, "UsendMii running on your computer.");
        OSScreenPutFontEx(SCREEN_DRC, 0, 9, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
        OSScreenPutFontEx(SCREEN_DRC, 0, 16, "Hold the HOME button to exit.");

        ConsoleDrawEnd();
    }

    // Save settings to file
    FILE * IP_file = fopen(path, "w");
    if (IP_file != nullptr) {
        fprintf(IP_file,
            "[server]\n"
            "ipaddress=%s\n"
            "port=%d\n"
            "\n",
            IP_ADDRESS, Port);
        fclose(IP_file);
    }

    // The buffer sent to the computer
    char msg_data[1024];

    uint16_t holdTime = 0;

    while(running == true) {
        KPADError kpad_error1 = KPADError::KPAD_ERROR_UNINITIALIZED;
        KPADError kpad_error2 = KPADError::KPAD_ERROR_UNINITIALIZED;
        KPADError kpad_error3 = KPADError::KPAD_ERROR_UNINITIALIZED;
        KPADError kpad_error4 = KPADError::KPAD_ERROR_UNINITIALIZED;
        KPADStatus kpad_data1;
        KPADStatus kpad_data2;
        KPADStatus kpad_data3;
        KPADStatus kpad_data4;
        HPADStatus hpad_data1[16];
        HPADStatus hpad_data2[16];
        HPADStatus hpad_data3[16];
        HPADStatus hpad_data4[16];

        // Read the VPAD
        VPADRead(VPAD_CHAN_0, &vpad_data, 1, &error);

        // Read the KPADs
        KPADReadEx(WPAD_CHAN_0, &kpad_data1, 1, &kpad_error1);
        KPADReadEx(WPAD_CHAN_1, &kpad_data2, 1, &kpad_error2);
        KPADReadEx(WPAD_CHAN_2, &kpad_data3, 1, &kpad_error3);
        KPADReadEx(WPAD_CHAN_3, &kpad_data4, 1, &kpad_error4);

        // Read the HPADs
        int32_t hpad_error1 = HPADRead(HPAD_CHAN_0, &hpad_data1[0], 16);
        int32_t hpad_error2 = HPADRead(HPAD_CHAN_1, &hpad_data2[0], 16);
        int32_t hpad_error3 = HPADRead(HPAD_CHAN_2, &hpad_data3[0], 16);
        int32_t hpad_error4 = HPADRead(HPAD_CHAN_3, &hpad_data4[0], 16);

        // Flush the cache (may be needed due to continuous refresh of the data ?)
        DCFlushRange(&vpad_data, sizeof(VPADStatus));

        // Transform to JSON
        PADData pad_data;
        memset(&pad_data, 0, sizeof(PADData));
        pad_data.vpad = &vpad_data;
        if(kpad_error1 == KPADError::KPAD_ERROR_OK)
        {
            pad_data.kpad[0] = &kpad_data1;
        }
        if(kpad_error2 == KPADError::KPAD_ERROR_OK)
        {
            pad_data.kpad[1] = &kpad_data2;
        }
        if(kpad_error3 == KPADError::KPAD_ERROR_OK)
        {
            pad_data.kpad[2] = &kpad_data3;
        }
        if(kpad_error4 == KPADError::KPAD_ERROR_OK)
        {
            pad_data.kpad[3] = &kpad_data4;
        }
        if(hpad_error1 >= 0)
        {
            pad_data.hpad[0] = &hpad_data1[0];
        }
        if(hpad_error2 >= 0)
        {
            pad_data.hpad[1] = &hpad_data2[0];
        }
        if(hpad_error3 >= 0)
        {
            pad_data.hpad[2] = &hpad_data3[0];
        }
        if(hpad_error4 >= 0)
        {
            pad_data.hpad[3] = &hpad_data4[0];
        }
        pad_to_json(pad_data, msg_data, sizeof(msg_data));

        // Send the message
        udp_print(msg_data);

        // Make a small delay to prevent filling up the computer receive buffer
        OSSleepTicks(OSMillisecondsToTicks(10)); // Make this value smaller for faster refreshing

        // Check for exit signal
        if (vpad_data.hold & VPAD_BUTTON_HOME && ++holdTime > 500) {
            if(fromHBL == true) {
                running = false;
            }
            else {
                SYSLaunchMenu();
                running = WHBProcIsRunning();
            }
        }
        if (vpad_data.release & VPAD_BUTTON_HOME) {
            holdTime = 0;
        }
    }

    VPADShutdown();
    KPADShutdown();
    HPADShutdown();
    WHBUnmountSdCard();
    ConsoleFree();
    WHBProcShutdown();

    return 0;
}
