/**
 *  @filename   :   epdpaint.h
 *  @brief      :   Header file for epdpaint.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 28 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EPDPAINT_H
#define EPDPAINT_H

// Display orientation
#define ROTATE_0            0
#define ROTATE_90           1
#define ROTATE_180          2
#define ROTATE_270          3

// Color inverse. 1 or 0 = set or reset a bit if set a colored pixel
#define IF_INVERT_COLOR     1

#include "fonts.h"

struct paint {
    unsigned char* image;
    int width;
    int height;
    int rotate;
};

void paint_init(struct paint * paint, unsigned char* image, int width, int height);
void paint_Clear(struct paint * paint, int colored);
int  paint_GetWidth(struct paint * paint);
void paint_SetWidth(struct paint * paint, int width);
int  paint_GetHeight(struct paint * paint);
void paint_SetHeight(struct paint * paint, int height);
int  paint_GetRotate(struct paint * paint);
void paint_SetRotate(struct paint * paint, int rotate);
unsigned char* paint_GetImage(struct paint * paint);
void paint_DrawAbsolutePixel(struct paint * paint, int x, int y, int colored);
void paint_DrawPixel(struct paint * paint, int x, int y, int colored);
void paint_DrawCharAt(struct paint * paint, int x, int y, char ascii_char, sFONT* font, int colored);
void paint_DrawStringAt(struct paint * paint, int x, int y, const char* text, sFONT* font, int colored);
void paint_DrawLine(struct paint * paint, int x0, int y0, int x1, int y1, int colored);
void paint_DrawHorizontalLine(struct paint * paint, int x, int y, int width, int colored);
void paint_DrawVerticalLine(struct paint * paint, int x, int y, int height, int colored);
void paint_DrawRectangle(struct paint * paint, int x0, int y0, int x1, int y1, int colored);
void paint_DrawFilledRectangle(struct paint * paint, int x0, int y0, int x1, int y1, int colored);
void paint_DrawCircle(struct paint * paint, int x, int y, int radius, int colored);
void paint_DrawFilledCircle(struct paint * paint, int x, int y, int radius, int colored);

#endif

/* END OF FILE */
