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
        OSScreenPutFontEx(1, 0, 0, "== UsendMii Client ==");
        OSScreenPutFontEx(1, 0, 1, "Please insert your computer's IP adress below");
        OSScreenPutFontEx(1, 0, 2, "(use the DPAD to edit the IP adress)");
        OSScreenPutFontEx(1, 4 * selected_digit, 6, "vvv");
        snprintf(IP_str, 32, "%3d.%3d.%3d.%3d", IP[0], IP[1], IP[2], IP[3]);
        OSScreenPutFontEx(1, 0, 7, IP_str);
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
    OSScreenPutFontEx(0, 0, 0, "== UsendMii Client ==");
    OSScreenPutFontEx(0, 0, 2, msg_connected);
    OSScreenPutFontEx(0, 0, 4, "Remember the program will not work without");
    OSScreenPutFontEx(0, 0, 5, "UsendMii running on your computer.");
    OSScreenPutFontEx(0, 0, 6, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
    OSScreenPutFontEx(0, 0, 16, "Hold the HOME button to exit.");

    // print to DRC
    OSScreenPutFontEx(1, 0, 0, "== UsendMii Client ==");
    OSScreenPutFontEx(1, 0, 2, msg_connected);
    OSScreenPutFontEx(1, 0, 4, "Remember the program will not work without");
    OSScreenPutFontEx(1, 0, 5, "UsendMii running on your computer.");
    OSScreenPutFontEx(1, 0, 6, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
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
