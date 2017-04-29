#include "program.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "vpad_to_json.h"
#include "udp.h"
#include "system/memory.h"
#include <stdio.h>
#include <malloc.h>

char IP[4] = {192, 168, 1, 100};
unsigned short Port = 4242;

static void PrintHeader(u32 bufferNum)
{
    OSScreenPutFontEx(bufferNum, 0, 0, " _   _                 _ __  __ _ _    ___ _ _         _");
    OSScreenPutFontEx(bufferNum, 0, 1, "| | | |___ ___ _ _  __| |  \\/  (_|_)  / __| (_)___ _ _| |");
    OSScreenPutFontEx(bufferNum, 0, 2, "| |_| (_-</ -_) ' \\/ _` | |\\/| | | | | (__| | / -_) ' \\  _|");
    OSScreenPutFontEx(bufferNum, 0, 3, " \\___//__/\\___|_||_\\__,_|_|  |_|_|_|  \\___|_|_\\___|_||_\\__| v0.0.2");
}

int _entryPoint()
{
    InitOSFunctionPointers();
    InitSocketFunctionPointers();
    InitVPadFunctionPointers();

    memoryInitialize();

    // Init screen and screen buffers
    OSScreenInit();
    u8 *ScreenBuffer0 = MEM1_alloc(OSScreenGetBufferSizeEx(0), 0x40);
    u8 *ScreenBuffer1 = MEM1_alloc(OSScreenGetBufferSizeEx(1), 0x40);
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

    char * IP_str = malloc(32);
    int selected_digit = 0;

    // Insert the IP adress (some code was taken from the IP Adress selector of geckiine made by brienj)
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
        OSScreenPutFontEx(1, 0, 5, "Please insert your computer's IP adress below");
        OSScreenPutFontEx(1, 0, 6, "(use the DPAD to edit the IP adress)");
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

    // Get IP Adress (without spaces)
    char * IP_ADRESS = malloc(32);
    snprintf(IP_ADRESS, 32, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);

    // Initialize the UDP connection
    udp_init(IP_ADRESS, Port);

    // Output the IP adress
    char * msg_connected = malloc(255);
    snprintf(msg_connected, 255, "Connected to %s:%d", IP_ADRESS, Port);

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


    // The buffer sent to the computer
    char msg_data[512];

    u16 holdTime = 0;

    for(;;) {
        // Read the VPAD
        VPADRead(0, &vpad_data, 1, &error);

        // Flush the cache (may be needed due to continuous refresh of the data ?)
        DCFlushRange(&vpad_data, sizeof(VPADData));

        // Transform to JSON
        vpad_to_json(&vpad_data, msg_data, sizeof(msg_data));

        // Send the message
        udp_printf(msg_data);

        // Make a small delay to prevent filling up the computer receive buffer
        usleep(10000); // I guess it should be enough? Make this value smaller for faster refreshing

        // Check for exit signal
        if (vpad_data.btns_h & VPAD_BUTTON_HOME && ++holdTime > 500) {
            break;
        }
        if (vpad_data.btns_r & VPAD_BUTTON_HOME) {
            holdTime = 0;
        }
    }

    free(IP_ADRESS);
    MEM1_free(ScreenBuffer0);
    MEM1_free(ScreenBuffer1);
    ScreenBuffer0 = NULL;
    ScreenBuffer1 = NULL;

    return 0;
}
