#include <wut_types.h>
#include <whb/proc.h>
#include <coreinit/screen.h>
#include <padscore/kpad.h>
#include <vpad/input.h>
#include <nsysnet/socket.h>
#include <nn/ac/ac_c.h>
#include <whb/libmanager.h>
#include <whb/sdcard.h>
#include <coreinit/memheap.h>
#include <coreinit/cache.h>
#include <coreinit/memfrmheap.h>
#include <ini.h>
#include "vpad_to_json.h"
#include "udp.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define FRAME_HEAP_TAG (0x000DECAF)

/**
 * Application configuration.
 */
typedef struct {
    const char* ipaddress;
    int port;
} configuration;

/**
 * Handler for ini parser.
 * @return Returns nonzero on success, zero on error.
 */
static int handler(void* user, const char* section, const char* name, const char* value)
{
    configuration* pconfig = (configuration*)user;
    if(strcmp(section, "server") == 0) {
        if(strcmp(name, "ipaddress") == 0) {
            pconfig->ipaddress = strdup(value);
        }
        else if(strcmp(name, "port") == 0) {
            pconfig->port = atoi(value);
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
 */
static void PrintHeader(OSScreenID bufferNum)
{
    OSScreenPutFontEx(bufferNum, -4, 0, " _   _                 _ __  __ _ _    ___ _ _         _");
    OSScreenPutFontEx(bufferNum, -4, 1, "| | | |___ ___ _ _  __| |  \\/  (_|_)  / __| (_)___ _ _| |");
    OSScreenPutFontEx(bufferNum, -4, 2, "| |_| (_-</ -_) ' \\/ _` | |\\/| | | | | (__| | / -_) ' \\  _|");
    OSScreenPutFontEx(bufferNum, -4, 3, " \\___//__/\\___|_||_\\__,_|_|  |_|_|_|  \\___|_|_\\___|_||_\\__| v0.3.0");
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
 */
int main(int argc, char **argv)
{
    uint8_t IP[4] = {192, 168, 1, 100};

    WHBProcInit();
    WHBInitializeSocketLibrary();
    VPADInit();
    KPADInit();
    WPADEnableURCC(TRUE);
    VPADSetTVMenuInvalid(VPAD_CHAN_0, TRUE);

    WHBMountSdCard();
    char path[256];
    char *sdRootPath = WHBGetSdCardMountPath();
    snprintf(path, sizeof(path), "%s/wiiu/apps/UsendMii_Client/settings.ini", sdRootPath);

    // Init screen and screen buffers
    MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
    MEMRecordStateForFrmHeap(heap, FRAME_HEAP_TAG);
    OSScreenInit();
    const uint32_t sBufferSizeTV = OSScreenGetBufferSizeEx(SCREEN_TV);
    const uint32_t sBufferSizeDRC = OSScreenGetBufferSizeEx(SCREEN_DRC);
    void *ScreenBuffer0 = MEMAllocFromFrmHeapEx(heap, sBufferSizeTV, 4);
    void *ScreenBuffer1 = MEMAllocFromFrmHeapEx(heap, sBufferSizeDRC, 4);
    OSScreenSetBufferEx(SCREEN_TV, ScreenBuffer0);
    OSScreenSetBufferEx(SCREEN_DRC, ScreenBuffer1);
    OSScreenEnableEx(SCREEN_TV, 1);
    OSScreenEnableEx(SCREEN_DRC, 1);

    // Clear screens
    OSScreenClearBufferEx(SCREEN_TV, 0x000000FF);
    OSScreenClearBufferEx(SCREEN_DRC, 0x000000FF);

    // Gamepad key state data
    VPADReadError error;
    VPADStatus vpad_data;

    char * IP_str = (char*)malloc(32);
    int8_t selected_digit = 0;

    // Read default settings from file
    BOOL ip_loaded = FALSE;
    configuration config = {NULL, 4242};
    ini_parse(path, handler, &config);
    unsigned short Port = config.port;
    if(config.ipaddress != NULL) {
        if(inet_pton(AF_INET, config.ipaddress, &IP) > 0) {
            ip_loaded = TRUE;
        }
        free((void*)config.ipaddress);
    }
    if (ip_loaded == FALSE && NNResult_IsSuccess(ACInitialize())) {
        uint32_t ac_ip = 0;
        if (NNResult_IsSuccess(ACGetAssignedAddress(&ac_ip))) {
            IP[0] = (ac_ip >> 24) & 0xFF;
            IP[1] = (ac_ip >> 16) & 0xFF;
            IP[2] = (ac_ip >>  8) & 0xFF;
            IP[3] = (ac_ip >>  0) & 0xFF;
        }
        ACFinalize();
    }

    // Insert the IP address (some code was taken from the IP Address selector of geckiine made by brienj)
    for (;;) {
        VPADRead(VPAD_CHAN_0, &vpad_data, 1, &error);
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
        OSScreenClearBufferEx(SCREEN_TV, 0x000000FF);
        OSScreenClearBufferEx(SCREEN_DRC, 0x000000FF);
        // Print to DRC
        PrintHeader(SCREEN_DRC);
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
            WHBDeinitializeSocketLibrary();
            OSScreenShutdown();
            MEMFreeByStateToFrmHeap(heap, FRAME_HEAP_TAG);
            WHBProcShutdown();
            return 0;
        }
    }
    free(IP_str);

    // Reset orientation
    ResetOrientation();

    // Get IP Address (without spaces)
    char * IP_ADDRESS = (char*)malloc(32);
    snprintf(IP_ADDRESS, 32, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);

    // Initialize the UDP connection
    udp_init(IP_ADDRESS, Port);

    // Output the IP address
    char * msg_connected = (char*)malloc(255);
    snprintf(msg_connected, 255, "Connected to %s:%d", IP_ADDRESS, Port);

    // Clear the screen
    OSScreenClearBufferEx(SCREEN_TV, 0x000000FF);
    OSScreenClearBufferEx(SCREEN_DRC, 0x000000FF);

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

    float rot_deg;
    float xy_deg;
    float radius;
    VPADGetCrossStickEmulationParamsL(VPAD_CHAN_0, &rot_deg, &xy_deg, &radius);

    for(;;) {
        int32_t kpad_error1 = -6;
        int32_t kpad_error2 = -6;
        int32_t kpad_error3 = -6;
        int32_t kpad_error4 = -6;
        KPADStatus kpad_data1;
        KPADStatus kpad_data2;
        KPADStatus kpad_data3;
        KPADStatus kpad_data4;

        // Prevent dead zones with emulated buttons
        VPADSetCrossStickEmulationParamsL(VPAD_CHAN_0, -1.0f, xy_deg, radius);
        VPADSetCrossStickEmulationParamsR(VPAD_CHAN_0, 1.0f, xy_deg, radius);

        // Read the VPAD
        VPADRead(VPAD_CHAN_0, &vpad_data, 1, &error);

        // Read the KPADs
        KPADReadEx(WPAD_CHAN_0, &kpad_data1, 1, &kpad_error1);
        KPADReadEx(WPAD_CHAN_1, &kpad_data2, 1, &kpad_error2);
        KPADReadEx(WPAD_CHAN_2, &kpad_data3, 1, &kpad_error3);
        KPADReadEx(WPAD_CHAN_3, &kpad_data4, 1, &kpad_error4);

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
    WHBDeinitializeSocketLibrary();
    OSScreenShutdown();
    MEMFreeByStateToFrmHeap(heap, FRAME_HEAP_TAG);
    WHBProcShutdown();

    return 0;
}
