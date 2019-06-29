#include "program.h"
#include <wut_types.h>
#include <coreinit/screen.h>
#include <padscore/kpad.h>
#include <vpad/input.h>
#include <nsysnet/socket.h>
#include <whb/libmanager.h>
#include <whb/sdcard.h>
#include <coreinit/memheap.h>
#include <coreinit/cache.h>
#include <coreinit/memfrmheap.h>
#include "vendor/iniparser/iniparser.h"
#include "vpad_to_json.h"
#include "udp.h"
#include <stdio.h>
#include <malloc.h>

#define FRAME_HEAP_TAG (0x000DECAF)

/**
 * Print the header.
 */
static void PrintHeader(uint32_t bufferNum)
{
    OSScreenPutFontEx(bufferNum, 0, 0, " _   _                 _ __  __ _ _    ___ _ _         _");
    OSScreenPutFontEx(bufferNum, 0, 1, "| | | |___ ___ _ _  __| |  \\/  (_|_)  / __| (_)___ _ _| |");
    OSScreenPutFontEx(bufferNum, 0, 2, "| |_| (_-</ -_) ' \\/ _` | |\\/| | | | | (__| | / -_) ' \\  _|");
    OSScreenPutFontEx(bufferNum, 0, 3, " \\___//__/\\___|_||_\\__,_|_|  |_|_|_|  \\___|_|_\\___|_||_\\__| v0.1.0");
}

int main(int argc, char **argv)
{
    uint8_t IP[4] = {192, 168, 1, 100};
    unsigned short Port = 4242;

    //InitOSFunctionPointers();
    //InitFSFunctionPointers();
    //InitSocketFunctionPointers();
    //InitVPadFunctionPointers();
    //InitPadScoreFunctionPointers();

    WHBInitializeSocketLibrary();
    VPADInit();
    KPADInit();
    WPADEnableURCC(1);

    WHBMountSdCard();
    char path[256];
    char *sdRootPath = WHBGetSdCardMountPath();
    snprintf(path, sizeof(path), "%s/wiiu/apps/UsendMii_Client/settings.ini", sdRootPath);

    MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
    MEMRecordStateForFrmHeap(heap, FRAME_HEAP_TAG);

    // Init screen and screen buffers
    const uint32_t sBufferSizeTV = OSScreenGetBufferSizeEx(SCREEN_TV);
    const uint32_t sBufferSizeDRC = OSScreenGetBufferSizeEx(SCREEN_DRC);
    OSScreenInit();
    void *ScreenBuffer0 = MEMAllocFromFrmHeapEx(heap, sBufferSizeTV, 4);
    void *ScreenBuffer1 = MEMAllocFromFrmHeapEx(heap, sBufferSizeDRC, 4);
    OSScreenSetBufferEx(SCREEN_TV, ScreenBuffer0);
    OSScreenSetBufferEx(SCREEN_DRC, ScreenBuffer1);
    OSScreenEnableEx(SCREEN_TV, 1);
    OSScreenEnableEx(SCREEN_DRC, 1);

    // Clear screens
    OSScreenClearBufferEx(SCREEN_TV, 0x993333FF);
    OSScreenClearBufferEx(SCREEN_DRC, 0x993333FF);

    // Gamepad key state data
    int32_t error;
    VPADStatus vpad_data;

    char * IP_str = (char*)malloc(32);
    int8_t selected_digit = 0;

    // Read default settings from file
    dictionary * ini = iniparser_load(path);
    if (ini != NULL) {
        const char * temp = iniparser_getstring(ini, "server:ipaddress", NULL);
        if (temp != NULL) {
            inet_pton(AF_INET, temp, &IP);
        }
        Port = iniparser_getint(ini, "server:port", Port);
        iniparser_freedict(ini);
    }

    // Insert the IP address (some code was taken from the IP Address selector of geckiine made by brienj)
    for (;;) {
        VPADRead(0, &vpad_data, 1, &error);
        if (vpad_data.trigger & VPAD_BUTTON_LEFT  && selected_digit > 0) {
            selected_digit--;
        }
        if (vpad_data.trigger & VPAD_BUTTON_RIGHT && selected_digit < 3) {
            selected_digit++;
        }
        if (vpad_data.trigger & VPAD_BUTTON_UP) {
            IP[selected_digit] = (IP[selected_digit] < 255) ? (IP[selected_digit] + 1) : 0;
        }
        if (vpad_data.trigger & VPAD_BUTTON_DOWN) {
            IP[selected_digit] = (IP[selected_digit] >   0) ? (IP[selected_digit] - 1) : 255;
        }

        // Clear the screen
        OSScreenClearBufferEx(SCREEN_TV, 0x993333FF);
        OSScreenClearBufferEx(SCREEN_DRC, 0x993333FF);
        // print to DRC
        PrintHeader(1);
        OSScreenPutFontEx(SCREEN_DRC, 0, 5, "Please insert your computer's IP address below");
        OSScreenPutFontEx(SCREEN_DRC, 0, 6, "(use the DPAD to edit the IP address)");
        OSScreenPutFontEx(SCREEN_DRC, 4 * selected_digit, 8, "vvv");
        snprintf(IP_str, 32, "%3d.%3d.%3d.%3d", IP[0], IP[1], IP[2], IP[3]);
        OSScreenPutFontEx(SCREEN_DRC, 0, 9, IP_str);
        OSScreenPutFontEx(SCREEN_DRC, 0, 15, "Press 'A' to confirm");
        OSScreenPutFontEx(SCREEN_DRC, 0, 16, "Press the HOME button to exit");
        // Flip buffers
        DCFlushRange(ScreenBuffer0, sBufferSizeTV);
        DCFlushRange(ScreenBuffer1, sBufferSizeDRC);
        OSScreenFlipBuffersEx(SCREEN_TV);
        OSScreenFlipBuffersEx(SCREEN_DRC);

        if (vpad_data.trigger & VPAD_BUTTON_A) {
            break;
        }
        if (vpad_data.trigger & VPAD_BUTTON_HOME) {
            free(IP_str);
            WHBUnmountSdCard();
            //MEM1_free(ScreenBuffer0);
            //MEM1_free(ScreenBuffer1);
            //ScreenBuffer0 = NULL;
            //ScreenBuffer1 = NULL;
            WHBDeinitializeSocketLibrary();
            OSScreenShutdown();
            MEMFreeByStateToFrmHeap(heap, FRAME_HEAP_TAG);
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
    OSScreenClearBufferEx(SCREEN_TV, 0x993333FF);
    OSScreenClearBufferEx(SCREEN_DRC, 0x993333FF);

    // print to TV
    PrintHeader(SCREEN_TV);
    OSScreenPutFontEx(SCREEN_TV, 0, 5, msg_connected);
    OSScreenPutFontEx(SCREEN_TV, 0, 7, "Remember the program will not work without");
    OSScreenPutFontEx(SCREEN_TV, 0, 8, "UsendMii 0.6.0 running on your computer.");
    OSScreenPutFontEx(SCREEN_TV, 0, 9, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
    OSScreenPutFontEx(SCREEN_TV, 0, 16, "Hold the HOME button to exit.");

    // print to DRC
    PrintHeader(SCREEN_DRC);
    OSScreenPutFontEx(SCREEN_DRC, 0, 5, msg_connected);
    OSScreenPutFontEx(SCREEN_DRC, 0, 7, "Remember the program will not work without");
    OSScreenPutFontEx(SCREEN_DRC, 0, 8, "UsendMii 0.6.0 running on your computer.");
    OSScreenPutFontEx(SCREEN_DRC, 0, 9, "You can get UsendMii from http://wiiubrew.org/wiki/UsendMii");
    OSScreenPutFontEx(SCREEN_DRC, 0, 16, "Hold the HOME button to exit.");

    // Flip buffers
    DCFlushRange(ScreenBuffer0, sBufferSizeTV);
    DCFlushRange(ScreenBuffer1, sBufferSizeDRC);
    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);

    free(msg_connected);

    // Save settings to file
    FILE * IP_file = fopen(path, "w");
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

    uint16_t holdTime = 0;

    for(;;) {
        int32_t kpad_error1 = -6;
        int32_t kpad_error2 = -6;
        int32_t kpad_error3 = -6;
        int32_t kpad_error4 = -6;
        KPADStatus kpad_data1;
        KPADStatus kpad_data2;
        KPADStatus kpad_data3;
        KPADStatus kpad_data4;

        // Read the VPAD
        VPADRead(0, &vpad_data, 1, &error);

        // Read the KPADs
        KPADReadEx(0, &kpad_data1, 1, &kpad_error1);
        KPADReadEx(1, &kpad_data2, 1, &kpad_error2);
        KPADReadEx(2, &kpad_data3, 1, &kpad_error3);
        KPADReadEx(3, &kpad_data4, 1, &kpad_error4);

        // Flush the cache (may be needed due to continuous refresh of the data ?)
        DCFlushRange(&vpad_data, sizeof(VPADStatus));

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
        usleep(10000); // I guess it should be enough? Make this value smaller for faster refreshing

        // Check for exit signal
        if (vpad_data.hold & VPAD_BUTTON_HOME && ++holdTime > 500) {
            break;
        }
        if (vpad_data.release & VPAD_BUTTON_HOME) {
            holdTime = 0;
        }
    }

    free(IP_ADDRESS);
    WHBUnmountSdCard();
    //MEM1_free(ScreenBuffer0);
    //MEM1_free(ScreenBuffer1);
    //ScreenBuffer0 = NULL;
    //ScreenBuffer1 = NULL;
    WHBDeinitializeSocketLibrary();
    OSScreenShutdown();
    MEMFreeByStateToFrmHeap(heap, FRAME_HEAP_TAG);

    return 0;
}
