#ifndef _SCREEN_UTILS_H_
#define _SCREEN_UTILS_H_

#include <wut_types.h>

#ifdef __cplusplus
extern "C" {
#endif

void DrawPixel(uint32_t bufferNum, uint32_t x, uint32_t y, uint32_t color);
void DrawLine(uint32_t bufferNum, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void DrawCircle(uint32_t bufferNum, uint32_t x, uint32_t y, float radius, uint32_t color);
void DrawRect(uint32_t bufferNum, float x0, float y0, float x1, float y1, uint32_t color);
void DrawFillRect(uint32_t bufferNum, float x0, float y0, float x1, float y1, uint32_t color);
void DrawTriangle(uint32_t bufferNum, float x0, float y0, float x1, float y1, float x2, float y2, uint32_t color);
void DrawFillTriangle(uint32_t bufferNum, float x0, float y0, float x1, float y1, float x2, float y2, uint32_t color);
void DrawArrow(uint32_t bufferNum, float x0, float y0, float x1, float y1, uint32_t color, float width);

#ifdef __cplusplus
}
#endif

#endif // _SCREEN_UTILS_H_
