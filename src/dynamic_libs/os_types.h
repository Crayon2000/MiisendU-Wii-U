#ifndef _OS_TYPES_H_
#define _OS_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gctypes.h>


#define OS_MESSAGE_NOBLOCK 0
#define OS_MESSAGE_BLOCK 1

#define OS_EXCEPTION_DSI 2
#define OS_EXCEPTION_ISI 3
#define OS_EXCEPTION_PROGRAM 6
#define OS_EXCEPTION_MODE_THREAD 1
#define OS_EXCEPTION_MODE_GLOBAL_ALL_CORES 4

typedef struct OSThread_ OSThread;

struct OSThreadLink {
    OSThread *next;
    OSThread *prev;
};

typedef struct OSThreadQueue_ {
	OSThread *head;
	OSThread *tail;
	void *parentStruct;
	u32 reserved;
} OSThreadQueue;

typedef struct OSMessage_ {
    u32 message;
    u32 data0;
    u32 data1;
    u32 data2;
}OSMessage;

typedef struct OSMessageQueue_ {
    u32 tag;
    char *name;
    u32 reserved;

    OSThreadQueue sendQueue;
    OSThreadQueue recvQueue;
    OSMessage *messages;
    int msgCount;
    int firstIndex;
    int usedCount;
} OSMessageQueue;

typedef struct OSContext_ {
	char tag[8];

	u32 gpr[32];

	u32 cr;
	u32 lr;
	u32 ctr;
	u32 xer;

	u32 srr0;
	u32 srr1;

	u32 ex0;
	u32 ex1;

	u32 exception_type;
	u32 reserved;

	double fpscr;
	double fpr[32];

	u16 spinLockCount;
	u16 state;

	u32 gqr[8];
	u32 pir;
	double psf[32];

	u64 coretime[3];
	u64 starttime;

	u32 error;
	u32 attributes;

	u32 pmc1;
	u32 pmc2;
	u32 pmc3;
	u32 pmc4;
	u32 mmcr0;
	u32 mmcr1;
} OSContext;

typedef enum OSExceptionType
{
   OS_EXCEPTION_TYPE_SYSTEM_RESET         = 0,
   OS_EXCEPTION_TYPE_MACHINE_CHECK        = 1,
   OS_EXCEPTION_TYPE_DSI                  = 2,
   OS_EXCEPTION_TYPE_ISI                  = 3,
   OS_EXCEPTION_TYPE_EXTERNAL_INTERRUPT   = 4,
   OS_EXCEPTION_TYPE_ALIGNMENT            = 5,
   OS_EXCEPTION_TYPE_PROGRAM              = 6,
   OS_EXCEPTION_TYPE_FLOATING_POINT       = 7,
   OS_EXCEPTION_TYPE_DECREMENTER          = 8,
   OS_EXCEPTION_TYPE_SYSTEM_CALL          = 9,
   OS_EXCEPTION_TYPE_TRACE                = 10,
   OS_EXCEPTION_TYPE_PERFORMANCE_MONITOR  = 11,
   OS_EXCEPTION_TYPE_BREAKPOINT           = 12,
   OS_EXCEPTION_TYPE_SYSTEM_INTERRUPT     = 13,
   OS_EXCEPTION_TYPE_ICI                  = 14,
} OSExceptionType;


typedef int (*ThreadFunc)(int argc, void *argv);

struct OSThread_ {
    OSContext context;

    u32 txtTag;
    u8 state;
    u8 attr;

    short threadId;
    int suspend;
    int priority;

    char _[0x394 - 0x330];

	void *stackBase;
	void *stackEnd;

	ThreadFunc entryPoint;

	char _3A0[0x6A0 - 0x3A0];
};

typedef struct _OSCalendarTime {
  int sec;
  int min;
  int hour;
  int mday;
  int mon;
  int year;
  int wday;
  int yday;
  int msec;
  int usec;
} OSCalendarTime;


#ifdef __cplusplus
}
#endif

#endif
