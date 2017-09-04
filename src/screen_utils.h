#ifndef _SCREEN_UTILS_H_
#define _SCREEN_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

void DrawPixel(u32 bufferNum, u32 x, u32 y, u32 color);
void DrawLine(u32 bufferNum, u32 x0, u32 y0, u32 x1, u32 y1, u32 color);
void DrawCircle(u32 bufferNum, u32 x, u32 y, f32 radius, u32 color);
void DrawRect(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, u32 color);
void DrawFillRect(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, u32 color);
void DrawArrow(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, u32 color, f32 width);

#ifdef __cplusplus
}
#endif

#endif // _SCREEN_UTILS_H_
