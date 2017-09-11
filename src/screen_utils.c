#include <math.h>
#include <stdlib.h>
#include "dynamic_libs/os_functions.h"

// Exchanges the values of x and y.
#define SWAP(x, y) do {  \
    typeof(x) _x = x;    \
    typeof(y) _y = y;    \
    x = _y;              \
    y = _x;              \
} while(0)

/**
 * Draw a pixel.
 * @param bufferNum The buffer number where to draw.
 * @param x The x coordinate where to draw the pixel.
 * @param y The y coordinate where to draw the pixel.
 * @param color The pixel color in RGBA.
 */
void DrawPixel(u32 bufferNum, u32 x, u32 y, u32 color)
{
    OSScreenPutPixelEx(bufferNum, x, y, color);
}

/**
 * Draw a line. This is using the Bresenham's line algorithm.
 * @param bufferNum The buffer number where to draw.
 * @param x0 The x coordinate where the line start.
 * @param y0 The y coordinate where the line start.
 * @param x1 The x coordinate where the line end.
 * @param y1 The y coordinate where the line end.
 * @param color The line color in RGBA.
 */
void DrawLine(u32 bufferNum, u32 x0, u32 y0, u32 x1, u32 y1, u32 color)
{
    const s32 dx = x1 - x0;
    const s32 dy = y1 - y0;
    const s32 dx0 = fabs(dx);
    const s32 dy0 = fabs(dy);
    s32 px = 2 * dy0 - dx0;
    s32 py = 2 * dx0 - dy0;

    if(dy0 <= dx0)
    {
        s32 x, y, xe;

        if(dx >= 0)
        {
            x = x0;
            y = y0;
            xe = x1;
        }
        else
        {
            x = x1;
            y = y1;
            xe = x0;
        }
        DrawPixel(bufferNum, x, y, color);
        for(s32 i = 0; x < xe; i++)
        {
            x++;
            if(px < 0)
            {
                px += 2 * dy0;
            }
            else
            {
                if((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    y++;
                }
                else
                {
                    y--;
                }
                px += 2 *(dy0 - dx0);
            }
            DrawPixel(bufferNum, x, y, color);
        }
    }
    else
    {
        s32 x, y, ye;

        if(dy >= 0)
        {
            x = x0;
            y = y0;
            ye = y1;
        }
        else
        {
            x = x1;
            y = y1;
            ye = y0;
        }
        DrawPixel(bufferNum, x, y, color);
        for(s32 i = 0; y < ye; i++)
        {
            y++;
            if(py <= 0)
            {
                py += 2 * dx0;
            }
            else
            {
                if((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    x++;
                }
                else
                {
                    x--;
                }
                py += 2 *(dx0 - dy0);
            }
            DrawPixel(bufferNum, x, y, color);
        }
    }
}

/**
 * Draw a circle.
 * @param bufferNum The buffer number where to draw.
 * @param x The x coordinate where to draw the circle.
 * @param y The y coordinate where to draw the circle.
 * @param radius The radius of the circle.
 * @param color The circle color in RGBA.
 */
void DrawCircle(u32 bufferNum, u32 x, u32 y, f32 radius, u32 color)
{
    for(f32 angle = 0.0f; angle < 360.0f; angle += 0.1f)
    {
        const f32 x1 = x + radius * cos(angle * M_PI / 180.0f);
        const f32 y1 = y + radius * sin(angle * M_PI / 180.0f);
        DrawPixel(bufferNum, round(x1), round(y1), color);
    }
}

/**
 * Draw a rectangle.
 * @param bufferNum The buffer number where to draw.
 * @param x0 The x coordinate where the rectangle start.
 * @param y0 The y coordinate where the rectangle start.
 * @param x1 The x coordinate where the rectangle end.
 * @param y1 The y coordinate where the rectangle end.
 * @param color The rectangle color in RGBA.
 */
void DrawRect(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, u32 color)
{
    DrawLine(bufferNum, x0, y0, x1, y0, color);
    DrawLine(bufferNum, x1, y0, x1, y1, color);
    DrawLine(bufferNum, x0, y1, x1, y1, color);
    DrawLine(bufferNum, x0, y0, x0, y1, color);
}

/**
 * Draw a filled rectangle.
 * @param bufferNum The buffer number where to draw.
 * @param x0 The x coordinate where the rectangle start.
 * @param y0 The y coordinate where the rectangle start.
 * @param x1 The x coordinate where the rectangle end.
 * @param y1 The y coordinate where the rectangle end.
 * @param color The filled rectangle color in RGBA.
 */
void DrawFillRect(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, u32 color)
{
    f32 X0, X1, Y0, Y1;

    if(x0 < x1)
    {
        X0 = x0;
        X1 = x1;
    }
    else
    {
        X0 = x1;
        X1 = x0;
    }

    if(y0 < y1)
    {
        Y0 = y0;
        Y1 = y1;
    }
    else
    {
        Y0 = y1;
        Y1 = y0;
    }

    for(u32 x = X0; x <= X1; x++)
    {
        for(u32 y = Y0; y <= Y1; y++)
        {
            DrawPixel(bufferNum, x, y, color);
        }
    }
}

/**
 * Draw a triangle.
 * @param bufferNum The buffer number where to draw.
 * @param x0 The x0 coordinate.
 * @param y0 The y0 coordinate.
 * @param x1 The x1 coordinate.
 * @param y1 The y1 coordinate.
 * @param x2 The x2 coordinate.
 * @param y2 The y2 coordinate.
 * @param color The triangle color in RGBA.
 */
void DrawTriangle(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, u32 color)
{
    DrawLine(bufferNum, x0, y0, x1, y1, color);
    DrawLine(bufferNum, x1, y1, x2, y2, color);
    DrawLine(bufferNum, x2, y2, x0, y0, color);
}

/**
 * Draw a filled triangle.
 * @param bufferNum The buffer number where to draw.
 * @param x0 The x0 coordinate.
 * @param y0 The y0 coordinate.
 * @param x1 The x1 coordinate.
 * @param y1 The y1 coordinate.
 * @param x2 The x2 coordinate.
 * @param y2 The y2 coordinate.
 * @param color The filled triangle color in RGBA.
 */
void DrawFillTriangle(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, u32 color)
{
    // Sort the points vertically
    if(y1 > y2)
    {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }
    if(y0 > y1)
    {
        SWAP(x0, x1);
        SWAP(y0, y1);
    }
    if(y1 > y2)
    {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    // Define more algorithm variables
    f32 dx_far = (x2 - x0) / (y2 - y0 + 0.001f);
    f32 dx_upper = (x1 - x0) / (y1 - y0 + 0.001f);
    f32 dx_low = (x2 - x1) / (y2 - y1 + 0.001f);
    f32 xf = x0;
    f32 xt = x0 + dx_upper; // if y0 == y1, special case

    // Loop through coordinates
    for(int y = y0; y <= y2; y++)
    {
        if(y >= 0)
        {
            DrawLine(bufferNum, round(xf), y, round(xt), y, color);
        }
        xf += dx_far;
        if(y < y1)
        {
            xt += dx_upper;
        }
        else
        {
            xt += dx_low;
        }
    }
}

/**
 * Draw an arrow.
 * @param bufferNum The buffer number where to draw.
 * @param x0 The x coordinate where the line start.
 * @param y0 The y coordinate where the line start.
 * @param x1 The x coordinate where the line end.
 * @param y1 The y coordinate where the line end.
 * @param width The width of the arrow head.
 * @param color The arrow color in RGBA.
 */
void DrawArrow(u32 bufferNum, f32 x0, f32 y0, f32 x1, f32 y1, u32 color, f32 width)
{
    f32 vx = x1 - x0;
    f32 vy = y1 - y0;
    f32 f = sqrtf(vx * vx + vy * vy);

    if(f == 0.0f)
    {
        vx = 0.0f;
        vy = 0.0f;
    }
    else
    {
        f = width * 2.0f / f;
        vx *= f;
        vy *= f;
    }

    DrawLine(bufferNum, x0, y0, x1, y1, color);
    DrawLine(bufferNum, x1, y1, round(x1-vx-vx-vy), round(y1-vy-vy+vx), color);
    DrawLine(bufferNum, x1, y1, round(x1-vx-vx+vy), round(y1-vy-vy-vx), color);
}
