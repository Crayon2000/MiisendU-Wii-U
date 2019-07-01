#ifndef _VPADTOJSON_H_
#define _VPADTOJSON_H_

#include <wut_types.h>
#include <padscore/kpad.h>
#include <vpad/input.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum VPADTouchPadResolution
{
   //! 1920 x 1080 resolution
   VPAD_TP_1920X1080             = 0,
   //! 1280 x 720 resolution
   VPAD_TP_1280X720              = 1,
   //! 854 x 480 resolution
   VPAD_TP_854X480               = 2,
} VPADTouchPadResolution;

/**
 * Transform touch data according to the current calibration data.
 *
 * \note
 * Retail Wii U systems have a single Gamepad on \link VPADChan::VPAD_CHAN_0
 * VPAD_CHAN_0. \endlink
 *
 * \param chan
 * Denotes which channel to get the calibration data from.
 *
 * \param tpResolution
 * Touchpad resolution.
 *
 * \param calibratedData
 * Pointer to write calibrated touch data to.
 *
 * \param uncalibratedData
 * Pointer to the source data to apply the calibration to.
 *
 * \sa
 * - VPADTouchData
 */
void
VPADGetTPCalibratedPointEx(VPADChan chan,
                           VPADTouchPadResolution tpResolution,
                           VPADTouchData *calibratedData,
                           VPADTouchData *uncalibratedData);

/**
 * Struct to hold all controllers data.
 */
typedef struct {
    VPADStatus* vpad;     /**< Wii U Gamepad. */
    KPADStatus* kpad[4];  /**< Wii Remotes. */
} PADData;

void pad_to_json(PADData pad_data, char* out, uint32_t out_size);

#ifdef __cplusplus
}
#endif

#endif /* _VPADTOJSON_H_ */