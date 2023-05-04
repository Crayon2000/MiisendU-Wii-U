#include "console.h"

#include <coreinit/memheap.h>
#include <coreinit/cache.h>
#include <coreinit/memfrmheap.h>
#include <coreinit/memory.h>
#include <coreinit/screen.h>
#include <proc_ui/procui.h>

#define CONSOLE_FRAME_HEAP_TAG (0x000DECAF)

static void *sBufferTV = NULL;
static void *sBufferDRC = NULL;
static uint32_t sBufferSizeTV = 0;
static uint32_t sBufferSizeDRC = 0;
static BOOL sConsoleHasForeground = TRUE;

/**
 * Callback called when the application acquires the foreground.
 * @param context Unused.
 * @return Returns 0.
 */
static uint32_t ConsoleProcCallbackAcquired(void *context)
{
   MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
   MEMRecordStateForFrmHeap(heap, CONSOLE_FRAME_HEAP_TAG);

   if(sBufferSizeTV > 0) {
      sBufferTV = MEMAllocFromFrmHeapEx(heap, sBufferSizeTV, 4);
   }

   if(sBufferSizeDRC > 0) {
      sBufferDRC = MEMAllocFromFrmHeapEx(heap, sBufferSizeDRC, 4);
   }

   sConsoleHasForeground = TRUE;
   OSScreenSetBufferEx(SCREEN_TV, sBufferTV);
   OSScreenSetBufferEx(SCREEN_DRC, sBufferDRC);
   return 0;
}

/**
 * Callback called when the application must release the foreground.
 * @param context Unused.
 * @return Returns 0.
 */
static uint32_t ConsoleProcCallbackReleased(void *context)
{
   MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
   MEMFreeByStateToFrmHeap(heap, CONSOLE_FRAME_HEAP_TAG);
   sConsoleHasForeground = FALSE;
   return 0;
}

/**
 * Initialize console.
 */
void ConsoleInit()
{
   OSScreenInit();
   sBufferSizeTV = OSScreenGetBufferSizeEx(SCREEN_TV);
   sBufferSizeDRC = OSScreenGetBufferSizeEx(SCREEN_DRC);

   ConsoleProcCallbackAcquired(NULL);
   OSScreenEnableEx(SCREEN_TV, TRUE);
   OSScreenEnableEx(SCREEN_DRC, TRUE);

   ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, ConsoleProcCallbackAcquired, NULL, 100);
   ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, ConsoleProcCallbackReleased, NULL, 100);
}

/**
 * Free console.
 */
void ConsoleFree()
{
   if(sConsoleHasForeground == TRUE) {
      OSScreenShutdown();
      ConsoleProcCallbackReleased(NULL);
   }
}

/**
 * Start console draw.
 * @return Returns TRUE if drawing started.
 */
BOOL ConsoleDrawStart()
{
   if(sConsoleHasForeground == FALSE) {
      return FALSE;
   }

    // Clear the screen
    OSScreenClearBufferEx(SCREEN_TV, 0x000000FF);
    OSScreenClearBufferEx(SCREEN_DRC, 0x000000FF);

    return TRUE;
}

/**
 * End console draw.
 */
void ConsoleDrawEnd()
{
    // Clear the screen
    DCFlushRange(sBufferTV, sBufferSizeTV);
    DCFlushRange(sBufferDRC, sBufferSizeDRC);
    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);
}
