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
// #include "demo-imagedata.h"

#define COLORED     0
#define UNCOLORED   1

/**
  * Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
  * In this case, a smaller image buffer is allocated and you have to 
  * update a partial display several times.
  * 1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
  */
unsigned char image[1024];
// Paint paint(image, 0, 0);
// Epd epd;
unsigned long time_start_ms;
unsigned long time_now_s;

void setup(struct epd * epd, struct paint * paint) {
    // put your setup code here, to run once:
    uart_init(38400);
    stdout = &uart_stdout;
    stdin = &uart_input;
    printf("epd_init\n");
    epd_init(epd, lut_full_update);
    printf("paint_init\n");
    paint_init(paint, image, 0, 0);

    printf("epd_clear_frame_memory\n");
    getchar();
    epd_clear_frame_memory(epd, 0xFF);   // bit set = white, bit reset = black

    paint_SetRotate(paint, ROTATE_0);
    paint_SetWidth(paint, 128);    // width should be the multiple of 8 
    paint_SetHeight(paint, 24);

    /* For simplicity, the arguments are explicit numerical coordinates */
    paint_Clear(paint, COLORED);
    paint_DrawFilledRectangle(paint, 10, 10, 20, 20, UNCOLORED);
    printf("epd_set_frame_memory_at\n");
    epd_set_frame_memory_at(
        epd,
        paint_GetImage(paint),
        0,
        0,
        paint_GetWidth(paint),
        paint_GetHeight(paint)
    );
    printf("epd_display_frame\n");
    epd_display_frame(epd);
    epd_set_frame_memory_at(
        epd,
        paint_GetImage(paint),
        0,
        0,
        paint_GetWidth(paint),
        paint_GetHeight(paint)
    );
    printf("epd_display_frame\n");
    epd_display_frame(epd);

    _delay_ms(4000);

    // paint_DrawStringAt(paint, 30, 4, "Hello world!", &Font12, UNCOLORED);

    // paint_Clear(paint, UNCOLORED);
    // paint_DrawStringAt(paint, 30, 4, "e-Paper Demo", &Font12, COLORED);
    // epd_set_frame_memory(epd, paint_GetImage(paint), 0, 30, paint_GetWidth(paint), paint_GetHeight(paint));

    // paint_SetWidth(paint, 64);
    // paint_SetHeight(paint, 64);

    // paint_Clear(paint, UNCOLORED);
    // paint_DrawRectangle(paint, 0, 0, 40, 50, COLORED);
    // paint_DrawLine(paint, 0, 0, 40, 50, COLORED);
    // paint_DrawLine(paint, 40, 0, 0, 50, COLORED);
    // epd_set_frame_memory(epd, paint_GetImage(paint), 16, 60, paint_GetWidth(paint), paint_GetHeight(paint));

    // paint_Clear(paint, UNCOLORED);
    // paint_DrawCircle(paint, 32, 32, 30, COLORED);
    // epd_set_frame_memory(epd, paint_GetImage(paint), 72, 60, paint_GetWidth(paint), paint_GetHeight(paint));

    // paint_Clear(paint, UNCOLORED);
    // paint_DrawFilledRectangle(paint, 0, 0, 40, 50, COLORED);
    // epd_set_frame_memory(epd, paint_GetImage(paint), 16, 130, paint_GetWidth(paint), paint_GetHeight(paint));

    // paint_Clear(paint, UNCOLORED);
    // paint_DrawFilledCircle(paint, 32, 32, 30, COLORED);
    // epd_set_frame_memory(epd, paint_GetImage(paint), 72, 130, paint_GetWidth(paint), paint_GetHeight(paint));
    // if (epd->Init(lut_partial_update) != 0) {
    //     Serial.print("e-Paper init failed");
    //     return;
    // }

    /** 
     *  there are 2 memory areas embedded in the e-paper display
     *  and once the display is refreshed, the memory area will be auto-toggled,
     *  i.e. the next action of SetFrameMemory will set the other memory area
     *  therefore you have to set the frame memory and refresh the display twice.
     */
    // epd_set_frame_memory(epd, IMAGE_DATA);
    // epd->DisplayFrame();
    // epd_set_frame_memory(epd, IMAGE_DATA);
    // epd->DisplayFrame();

    // time_start_ms = millis();
}


int main() {
    struct epd epd;
    struct paint paint;
    setup(&epd, &paint);
    int i = 0;
    int j = 0;
    while (1) {
        paint_Clear(&paint, UNCOLORED);
        paint_DrawFilledRectangle(&paint, i, j, i+10, j+10, COLORED);
        epd_set_frame_memory_at(
            &epd,
            paint_GetImage(&paint),
            0,
            0,
            paint_GetWidth(&paint),
            paint_GetHeight(&paint)
        );
        epd_display_frame(&epd);
        i += 10;
        j += 10;
        if (i > epd.width) {
            i = 0;
        }
        if (j > epd.height) {
            j = 0;
        }

        _delay_ms(4000);
    }

    return 0;
}
