#include "program.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "udp.h"
#include "system/memory.h"
#include <stdio.h>
#include <malloc.h>


u32  buttons_val[22] = {VPAD_BUTTON_A, VPAD_BUTTON_B, VPAD_BUTTON_X, VPAD_BUTTON_Y, VPAD_BUTTON_LEFT, VPAD_BUTTON_RIGHT, VPAD_BUTTON_UP, VPAD_BUTTON_DOWN, VPAD_STICK_R_EMULATION_LEFT, VPAD_STICK_R_EMULATION_RIGHT, VPAD_STICK_R_EMULATION_UP, VPAD_STICK_R_EMULATION_DOWN, VPAD_STICK_L_EMULATION_LEFT, VPAD_STICK_L_EMULATION_RIGHT, VPAD_STICK_L_EMULATION_UP, VPAD_STICK_L_EMULATION_DOWN
, VPAD_BUTTON_PLUS, VPAD_BUTTON_MINUS, VPAD_BUTTON_ZL, VPAD_BUTTON_ZR, VPAD_BUTTON_L, VPAD_BUTTON_R};
char buttons_n_h[22] = {'A', 'B', 'X', 'Y', 'L', 'R', 'U', 'D', 'L', 'R', 'U', 'D', 'L', 'R', 'U', 'D', 'P', 'M', 'Q', 'W', 'E', 'T'};
char buttons_n_r[22] = {'a', 'b', 'x', 'y', 'l', 'r', 'u', 'd', 'l', 'r', 'u', 'd', 'l', 'r', 'u', 'd', 'p', 'm', 'q', 'w', 'e', 't'};

char IP[4] = {192, 168, 1, 67};

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
    int error;
    VPADData vpad_data;

    char * IP_str = malloc(32);
    int selected_digit = 0;

    // Insert the IP adress (some code was taken from the IP Adress selector of geckiine made by brienj)
    for (;;) {
        VPADRead(0, &vpad_data, 1, &error);
        if (vpad_data.btns_d & VPAD_BUTTON_LEFT  && selected_digit > 0) selected_digit--;
        if (vpad_data.btns_d & VPAD_BUTTON_RIGHT && selected_digit < 3) selected_digit++;
        if (vpad_data.btns_d & VPAD_BUTTON_UP)   IP[selected_digit] = (IP[selected_digit] < 255) ? (IP[selected_digit] + 1) : 0;
        if (vpad_data.btns_d & VPAD_BUTTON_DOWN) IP[selected_digit] = (IP[selected_digit] >   0) ? (IP[selected_digit] - 1) : 255;

        // Clear the screen
        OSScreenClearBufferEx(0, 0);
        OSScreenClearBufferEx(1, 0);
        // print to DRC
        OSScreenPutFontEx(1, 0, 0, "== RemotePad ==");
        OSScreenPutFontEx(1, 0, 1, "Please insert your computer's IP adress below ");
        OSScreenPutFontEx(1, 0, 2, "(use DPAD to edit the IP adress)");
        OSScreenPutFontEx(1, 4 * selected_digit, 6, "vvv");
        snprintf(IP_str, 32, "%3d.%3d.%3d.%3d", IP[0], IP[1], IP[2], IP[3]);
        OSScreenPutFontEx(1, 0, 7, IP_str);
        OSScreenPutFontEx(1, 0, 15, "Press 'A' to confirm");
        OSScreenPutFontEx(1, 0, 16, "Press HOME-Button to exit");
        // Flip buffers
        OSScreenFlipBuffersEx(0);
        OSScreenFlipBuffersEx(1);

        if (vpad_data.btns_d & VPAD_BUTTON_A) break;
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
    udp_init(IP_ADRESS);

    // Output the IP adress
    char * msg_connected = malloc(255);
    snprintf(msg_connected, 255, "Connected to %s!", IP_ADRESS);

    // Clear the screen
    OSScreenClearBufferEx(0, 0);
    OSScreenClearBufferEx(1, 0);

    // print to TV
    OSScreenPutFontEx(0, 0, 0, "== RemotePad ==");
    OSScreenPutFontEx(0, 0, 2, msg_connected);
    OSScreenPutFontEx(0, 0, 4, "Remember the program will not work without the");
    OSScreenPutFontEx(0, 0, 5, "client running on your computer. ");
    OSScreenPutFontEx(0, 0, 6, "You can get the client from goo.gl/7cvSSf");
    OSScreenPutFontEx(0, 0, 16, "Press HOME-Button to exit.");

    // print to DRC
    OSScreenPutFontEx(1, 0, 0, "== RemotePad ==");
    OSScreenPutFontEx(1, 0, 2, msg_connected);
    OSScreenPutFontEx(1, 0, 4, "Remember the program will not work without the");
    OSScreenPutFontEx(1, 0, 5, "client running on your computer. ");
    OSScreenPutFontEx(1, 0, 6, "You can get the client from goo.gl/7cvSSf");
    OSScreenPutFontEx(1, 0, 16, "Press HOME-Button to exit.");

    // Flip buffers
    OSScreenFlipBuffersEx(0);
    OSScreenFlipBuffersEx(1);

    free(msg_connected);


    // The buffer sent to the computer
    char msg_data[23];

    // Add nullstring terminator
    msg_data[22] = 0;

    for(;;) {
        // Read the VPAD
        VPADRead(0, &vpad_data, 1, &error);

        // Flush the cache (may be needed due to continuous refresh of the data ?)
        DCFlushRange(&vpad_data, sizeof(VPADData));

        // Get the status of the gamepad and insert in the message either the HOLD character for the key or the NOT PRESSED character of the key
        for(int i = 0; i < 22; i++)
            msg_data[i] = (vpad_data.btns_h & buttons_val[i]) ? buttons_n_h[i] : buttons_n_r[i];

        // Send the message
        udp_printf("%s", msg_data);

        // Make a small delay to prevent filling up the computer receive buffer
        usleep(10000); // I guess it should be enough? Make this value smaller for faster refreshing

        // Check for exit signal
        if (vpad_data.btns_h & VPAD_BUTTON_HOME)
            break;
    }

    free(IP_ADRESS);
    MEM1_free(ScreenBuffer0);
    MEM1_free(ScreenBuffer1);
    ScreenBuffer0 = NULL;
    ScreenBuffer1 = NULL;

    return 0;
}
