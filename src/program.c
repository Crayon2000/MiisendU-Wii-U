#include "program.h"
#include "vendor/iniparser/iniparser.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/fs_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/padscore_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "fs/sd_fat_devoptab.h"
#include "system/memory.h"
#include "vpad_to_json.h"
#include "udp.h"
#include <stdio.h>
#include <malloc.h>

/**
 * Print the header.
 */
static void PrintHeader(u32 bufferNum)
{
    OSScreenPutFontEx(bufferNum, 0, 0, " _   _                 _ __  __ _ _    ___ _ _         _");
    OSScreenPutFontEx(bufferNum, 0, 1, "| | | |___ ___ _ _  __| |  \\/  (_|_)  / __| (_)___ _ _| |");
    OSScreenPutFontEx(bufferNum, 0, 2, "| |_| (_-</ -_) ' \\/ _` | |\\/| | | | | (__| | / -_) ' \\  _|");
    OSScreenPutFontEx(bufferNum, 0, 3, " \\___//__/\\___|_||_\\__,_|_|  |_|_|_|  \\___|_|_\\___|_||_\\__| v0.0.3");
}

int _entryPoint(int argc, char **argv)
{
    unsigned char IP[4] = {192, 168, 1, 100};
    unsigned short Port = 4242;

    InitOSFunctionPointers();
    InitFSFunctionPointers();
    InitSocketFunctionPointers();
    InitVPadFunctionPointers();
    InitPadScoreFunctionPointers();

    memoryInitialize();
    mount_sd_fat("sd");

    // Init screen and screen buffers
    OSScreenInit();
    void *ScreenBuffer0 = MEM1_alloc(OSScreenGetBufferSizeEx(0), 0x40);
    void *ScreenBuffer1 = MEM1_alloc(OSScreenGetBufferSizeEx(1), 0x40);
    OSScreenSetBufferEx(0, ScreenBuffer0);
    OSScreenSetBufferEx(1, ScreenBuffer1);
    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);

    // Clear screens
    OSScreenClearBufferEx(0, 0);
    OSScreenClearBufferEx(1, 0);

    // Gamepad key state data
    s32 error;
    VPADData vpad_data;

    char * IP_str = (char*)malloc(32);
    s8 selected_digit = 0;

    // Read default settings from file
    dictionary * ini = iniparser_load("sd:/wiiu/apps/UsendMii_Client/settings.ini");
    if (ini != NULL) {
        const char * temp = iniparser_getstring(ini, "server:ipaddress", NULL);
        if (temp != NULL) {
            udp_string_to_ipv4(temp, IP, 4);
        }
        Port = iniparser_getint(ini, "server:port", Port);
        iniparser_freedict(ini);
    }

    // Insert the IP address (some code was taken from the IP Address selector of geckiine made by brienj)
    for (;;) {
        VPADRead(0, &vpad_data, 1, &error);
        if (vpad_data.btns_d & VPAD_BUTTON_LEFT  && selected_digit > 0) {
            selected_digit--;
        }
        if (vpad_data.btns_d & VPAD_BUTTON_RIGHT && selected_digit < 3) {
            selected_digit++;
        }
        if (vpad_data.btns_d & VPAD_BUTTON_UP) {
            IP[selected_digit] = (IP[selected_digit] < 255) ? (IP[selected_digit] + 1) : 0;
        }
        if (vpad_data.btns_d & VPAD_BUTTON_DOWN) {
            IP[selected_digit] = (IP[selected_digit] >   0) ? (IP[selected_digit] - 1) : 255;
        }

        // Clear the screen
        OSScreenClearBufferEx(0, 0);
        OSScreenClearBufferEx(1, 0);
        // print to DRC
        PrintHeader(1);
        OSScreenPutFontEx(1, 0, 5, "Please insert your computer's IP address below");
        OSScreenPutFontEx(1, 0, 6, "(use the DPAD to edit the IP address)");
        OSScreenPutFontEx(1, 4 * selected_digit, 8, "vvv");
        snprintf(IP_str, 32, "%3d.%3d.%3d.%3d", IP[0], IP[1], IP[2], IP[3]);
        OSScreenPutFontEx(1, 0, 9, IP_str);
        OSScreenPutFontEx(1, 0, 15, "Press 'A' to confirm");
        OSScreenPutFontEx(1, 0, 16, "Press the HOME button to exit");
        // Flip buffers
        OSScreenFlipBuffersEx(0);
        OSScreenFlipBuffersEx(1);

        if (vpad_data.btns_d & VPAD_BUTTON_A) {
            break;
        }
        if (vpad_data.btns_d & VPAD_BUTTON_HOME) {
            free(IP_str);
            MEM1_free(ScreenBuffer0);
            MEM1_free(ScreenBuffer1);
            ScreenBuffer0 = NULL;
            ScreenBuffer1 = NULL;
            return 0;
        }
    }
    free(IP_str);

    // Get IP Address (without spaces)
    char * IP_ADDRESS = (char*)malloc(32);
    snprintf(IP_ADDRESS, 32, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);

    // Initialize the UDP connection
    udp_init(IP_ADDRESS, Port);

    // Output the IP address
    char * msg_connected = (char*)malloc(255);
    snprintf(msg_connected, 255, "Connected to %s:%d", IP_ADDRESS, Port);

    // Clear the screen
    OSScreenClearBufferEx(0, 0);
    OSScreenClearBufferEx(1, 0);

    // print to TV
    PrintHeader(0);
    OSScreenPutFontEx(0, 0, 5, msg_connected);
    OSScreenPutFontEx(0, 0, 7, "Remember the program will not work without");
    OSScreenPutFontEx(0, 0, 8, "UsendMii running on your computer.");
    OSScreenPutFontEx(0, 0, 9, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
    OSScreenPutFontEx(0, 0, 16, "Hold the HOME button to exit.");

    // print to DRC
    PrintHeader(1);
    OSScreenPutFontEx(1, 0, 5, msg_connected);
    OSScreenPutFontEx(1, 0, 7, "Remember the program will not work without");
    OSScreenPutFontEx(1, 0, 8, "UsendMii running on your computer.");
    OSScreenPutFontEx(1, 0, 9, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
    OSScreenPutFontEx(1, 0, 16, "Hold the HOME button to exit.");

    // Flip buffers
    OSScreenFlipBuffersEx(0);
    OSScreenFlipBuffersEx(1);

    free(msg_connected);

    // Save settings to file
    FILE * IP_file = fopen("sd:/wiiu/apps/UsendMii_Client/settings.ini", "w");
    if (IP_file != NULL) {
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

    u16 holdTime = 0;

    for(;;) {
        s32 kpad_error1 = -6;
        s32 kpad_error2 = -6;
        s32 kpad_error3 = -6;
        s32 kpad_error4 = -6;
        KPADData kpad_data1;
        KPADData kpad_data2;
        KPADData kpad_data3;
        KPADData kpad_data4;

        // Read the VPAD
        VPADRead(0, &vpad_data, 1, &error);

        // Read the KPADs
        KPADReadEx(0, &kpad_data1, 1, &kpad_error1);
        KPADReadEx(1, &kpad_data2, 1, &kpad_error2);
        KPADReadEx(2, &kpad_data3, 1, &kpad_error3);
        KPADReadEx(3, &kpad_data4, 1, &kpad_error4);

        // Flush the cache (may be needed due to continuous refresh of the data ?)
        DCFlushRange(&vpad_data, sizeof(VPADData));

        // Transform to JSON
        PADData pad_data;
        memset(&pad_data, 0, sizeof(PADData));
        pad_data.vpad = &vpad_data;
        if(kpad_error1 == 0)
        {
            pad_data.kpad[0] = &kpad_data1;
        }
        if(kpad_error2 == 0)
        {
            pad_data.kpad[1] = &kpad_data2;
        }
        if(kpad_error3 == 0)
        {
            pad_data.kpad[2] = &kpad_data3;
        }
        if(kpad_error4 == 0)
        {
            pad_data.kpad[3] = &kpad_data4;
        }
        pad_to_json(pad_data, msg_data, sizeof(msg_data));

        // Send the message
        udp_printf(msg_data);

        // Make a small delay to prevent filling up the computer receive buffer
        os_usleep(10000); // I guess it should be enough? Make this value smaller for faster refreshing

        // Check for exit signal
        if (vpad_data.btns_h & VPAD_BUTTON_HOME && ++holdTime > 500) {
            break;
        }
        if (vpad_data.btns_r & VPAD_BUTTON_HOME) {
            holdTime = 0;
        }
    }

    free(IP_ADDRESS);
    unmount_sd_fat("sd");
    MEM1_free(ScreenBuffer0);
    MEM1_free(ScreenBuffer1);
    ScreenBuffer0 = NULL;
    ScreenBuffer1 = NULL;

    return 0;
}
