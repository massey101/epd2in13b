/**
 *  @filename   :   epd2in13-demo.ino
 *  @brief      :   2.13inch e-paper display demo
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     September 9 2017
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

#include <util/delay.h>
#include <stdio.h>

#include "epd2in13.h"
#include "epdpaint.h"
#include "uart.h"

#define COLORED     0
#define UNCOLORED   1

/**
  * Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
  * In this case, a smaller image buffer is allocated and you have to 
  * update a partial display several times.
  * 1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
  */
unsigned char image[1120];
// Paint paint(image, 0, 0);
// Epd epd;
unsigned long time_start_ms;
unsigned long time_now_s;

void draw_rec(struct epd * epd, struct paint * paint, unsigned int x, unsigned int y, unsigned int w) {
    paint_DrawFilledRectangle(paint, x, y, x+w, y+w, COLORED);
    epd_set_partial_window_black(
        epd,
        paint_GetImage(paint),
        0,
        0,
        paint_GetWidth(paint),
        paint_GetHeight(paint)
    );
}

void setup(struct epd * epd, struct paint * paint) {
    // put your setup code here, to run once:
    uart_init(38400);
    stdout = &uart_stdout;
    stdin = &uart_input;
    epd_init(epd);
    paint_init(paint, image, 0, 0);

    epd_clear_frame_memory(epd);   // bit set = white, bit reset = black

    paint_SetRotate(paint, ROTATE_90);
    paint_SetWidth(paint, 16);    // width should be the multiple of 8
    paint_SetHeight(paint, 200);

    paint_Clear(paint, UNCOLORED);
    paint_DrawStringAt(paint, 0, 0, "enter a string:", &Font16, COLORED);
    epd_set_partial_window_black(
        epd,
        paint_GetImage(paint),
        epd->width - paint->width,
        8,
        paint_GetWidth(paint),
        paint_GetHeight(paint)
    );

    /* For simplicity, the arguments are explicit numerical coordinates */
    epd_display_frame(epd);
}


int main() {
    struct epd epd;
    struct paint paint;
    setup(&epd, &paint);
    char number_str[32];
    int number;
    paint_SetWidth(&paint, 24);    // width should be the multiple of 8
    while (1) {
        paint_Clear(&paint, UNCOLORED);
        printf("Enter a number: ");
        scanf("%d", &number);
        sprintf(number_str, "Got: %d", number);
        paint_DrawStringAt(&paint, 0, 0, number_str, &Font24, COLORED);

        epd_set_partial_window_black(
            &epd,
            paint_GetImage(&paint),
            epd.width - paint.width-32,
            8,
            paint_GetWidth(&paint),
            paint_GetHeight(&paint)
        );
        epd_display_frame(&epd);
    }

    return 0;
}
