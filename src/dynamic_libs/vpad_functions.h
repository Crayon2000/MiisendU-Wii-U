/****************************************************************************
 * Copyright (C) 2015
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#ifndef __VPAD_FUNCTIONS_H_
#define __VPAD_FUNCTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gctypes.h>

extern u32 vpad_handle;
extern u32 vpadbase_handle;

#define VPAD_BUTTON_A        0x8000
#define VPAD_BUTTON_B        0x4000
#define VPAD_BUTTON_X        0x2000
#define VPAD_BUTTON_Y        0x1000
#define VPAD_BUTTON_LEFT     0x0800
#define VPAD_BUTTON_RIGHT    0x0400
#define VPAD_BUTTON_UP       0x0200
#define VPAD_BUTTON_DOWN     0x0100
#define VPAD_BUTTON_ZL       0x0080
#define VPAD_BUTTON_ZR       0x0040
#define VPAD_BUTTON_L        0x0020
#define VPAD_BUTTON_R        0x0010
#define VPAD_BUTTON_PLUS     0x0008
#define VPAD_BUTTON_MINUS    0x0004
#define VPAD_BUTTON_HOME     0x0002
#define VPAD_BUTTON_SYNC     0x0001
#define VPAD_BUTTON_STICK_R  0x00020000
#define VPAD_BUTTON_STICK_L  0x00040000
#define VPAD_BUTTON_TV       0x00010000

#define VPAD_STICK_R_EMULATION_LEFT     0x04000000
#define VPAD_STICK_R_EMULATION_RIGHT    0x02000000
#define VPAD_STICK_R_EMULATION_UP       0x01000000
#define VPAD_STICK_R_EMULATION_DOWN     0x00800000

#define VPAD_STICK_L_EMULATION_LEFT     0x40000000
#define VPAD_STICK_L_EMULATION_RIGHT    0x20000000
#define VPAD_STICK_L_EMULATION_UP       0x10000000
#define VPAD_STICK_L_EMULATION_DOWN     0x08000000

//! Own definitions
#define VPAD_BUTTON_TOUCH               0x00080000
#define VPAD_MASK_EMULATED_STICKS       0x7F800000
#define VPAD_MASK_BUTTONS               ~VPAD_MASK_EMULATED_STICKS

typedef enum VPADTPResolution
{
    VPAD_TP_1920x1080,
    VPAD_TP_1280x720,
    VPAD_TP_854x480
} VPADTPResolution;

typedef struct
{
    f32 x,y;
} Vec2D;

typedef struct
{
    f32 x,y,z;
} Vec3D;

typedef struct
{
    Vec3D X,Y,Z;
} VPADOrientation;

typedef struct
{
    u16 x, y;               /* Touch coordinates */
    u16 touched;            /* 1 = Touched, 0 = Not touched */
    u16 invalid;            /* 0 = All valid, 1 = X invalid, 2 = Y invalid, 3 = Both invalid? */
} VPADTPData;

typedef struct
{
    s16 offsetX;
    s16 offsetY;
    f32 scaleX;
    f32 scaleY;
} VPADTPCalibrationParam;

typedef struct
{
    u32 btns_h;                  /* Held buttons */
    u32 btns_d;                  /* Buttons that are pressed at that instant */
    u32 btns_r;                  /* Released buttons */
    Vec2D lstick, rstick;        /* Each contains 4-byte X and Y components */
    Vec3D acc;                   /* Status of DRC accelerometer */
    f32 acc_magnitude;           /* Accelerometer magnitude */
    f32 acc_variation;           /* Accelerometer variation */
    Vec2D acc_vertical;          /* Vertical */
    Vec3D gyro;                  /* Gyro data */
    Vec3D angle;                 /* Angle data */
    s8 error;                    /* Error */
    VPADTPData tpdata;           /* Normal touchscreen data */
    VPADTPData tpdata1;          /* Modified touchscreen data 1 */
    VPADTPData tpdata2;          /* Modified touchscreen data 2 */
    VPADOrientation dir;         /* Orientation in three-dimensional space */
    BOOL headphone;              /* Set to TRUE if headphones are plugged in, FALSE otherwise */
    Vec3D mag;                   /* Magnetometer data */
    u8 volume;                   /* 0 to 255 */
    u8 battery;                  /* 0 to 6 */
    u8 mic;                      /* Microphone status */
    u8 unk_volume;               /* One less than volume */
    u8 paddings[7];
} VPADData;

void InitVPadFunctionPointers(void);
void InitAcquireVPad(void);

extern s32 (* VPADRead)(s32 chan, VPADData *buffer, u32 buffer_size, s32 *error);
extern void (* VPADSetTPCalibrationParam) (s32 chan, const VPADTPCalibrationParam param);
extern void (* VPADGetTPCalibrationParam) (s32 chan, VPADTPCalibrationParam* param);
extern void (* VPADGetTPCalibratedPoint) (s32 chan, VPADTPData *disp, const VPADTPData *raw);
extern void (* VPADGetTPCalibratedPointEx) (s32 chan, VPADTPResolution tpReso, VPADTPData *disp, const VPADTPData *raw);
extern s32 (* VPADGetLcdMode)(s32 padnum, s32 *lcdmode);
extern s32 (* VPADSetLcdMode)(s32 padnum, s32 lcdmode);
extern void (* VPADInit)(void);
extern s32 (* VPADBASEGetMotorOnRemainingCount)(s32 lcdmode);
extern s32 (* VPADBASESetMotorOnRemainingCount)(s32 lcdmode,s32 counter);

#ifdef __cplusplus
}
#endif

#endif // __VPAD_FUNCTIONS_H_
