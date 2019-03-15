/**
 *  @filename   :   epd2in13.cpp
 *  @brief      :   Implements for e-paper library
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

#include <stdlib.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "epd2in13.h"
#include <util/delay.h>


int epd_init(struct epd * epd, const unsigned char* lut) {
    epd->reset_pin = RST_PIN;
    epd->dc_pin = DC_PIN;
    epd->cs_pin = CS_PIN;
    epd->busy_pin = BUSY_PIN;
    epd->width = EPD_WIDTH;
    epd->height = EPD_HEIGHT;
    epd->lut = lut;
    printf("init\n");

    _delay_ms(2000);

    /* this calls the peripheral hardware interface, see epdif */
    if (epd_if_init() != 0) {
        return -1;
    }
    /* EPD hardware init start */
    epd_reset(epd);
    epd_send_command(epd, DRIVER_OUTPUT_CONTROL);
    epd_send_data(epd, (EPD_HEIGHT - 1) & 0xFF);
    epd_send_data(epd, ((EPD_HEIGHT - 1) >> 8) & 0xFF);
    epd_send_data(epd, 0x00);                     // GD = 0; SM = 0; TB = 0;
    epd_send_command(epd, BOOSTER_SOFT_START_CONTROL);
    epd_send_data(epd, 0xD7);
    epd_send_data(epd, 0xD6);
    epd_send_data(epd, 0x9D);
    epd_send_command(epd, WRITE_VCOM_REGISTER);
    epd_send_data(epd, 0xA8);                     // VCOM 7C
    epd_send_command(epd, SET_DUMMY_LINE_PERIOD);
    epd_send_data(epd, 0x1A);                     // 4 dummy lines per gate
    epd_send_command(epd, SET_GATE_TIME);
    epd_send_data(epd, 0x08);                     // 2us per line
    epd_send_command(epd, DATA_ENTRY_MODE_SETTING);
    epd_send_data(epd, 0x03);                     // X increment; Y increment
    epd_set_lut(epd, lut);
    /* EPD hardware init end */
    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void epd_send_command(struct epd * epd, unsigned char command) {
    epd_if_digital_write(epd->dc_pin, LOW);
    epd_if_spi_transfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void epd_send_data(struct epd * epd, unsigned char data) {
    epd_if_digital_write(epd->dc_pin, HIGH);
    epd_if_spi_transfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes LOW
 */
void epd_wait_until_idle(struct epd * epd) {
    //LOW: idle, HIGH: busy
    printf("W1\n");
    printf("W %d\n", epd_if_digital_read(epd->busy_pin));
    _delay_ms(100);
    printf("W %d\n", epd_if_digital_read(epd->busy_pin));
    while(epd_if_digital_read(epd->busy_pin) == HIGH) {
        //printf("W\n");
        _delay_ms(100);
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Sleep();
 */
void epd_reset(struct epd * epd) {
    //module reset
    epd_if_digital_write(epd->reset_pin, LOW);
    _delay_ms(200);
    epd_if_digital_write(epd->reset_pin, HIGH);
    _delay_ms(200);
}

/**
 *  @brief: set the look-up table register
 */
void epd_set_lut(struct epd * epd, const unsigned char* lut) {
    epd->lut = lut;
    epd_send_command(epd, WRITE_LUT_REGISTER);
    /* the length of look-up table is 30 bytes */
    for (int i = 0; i < 30; i++) {
        epd_send_data(epd, lut[i]);
    }
}

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 */
void epd_set_frame_memory_at(
    struct epd * epd,
    const unsigned char* image_buffer,
    unsigned int x,
    unsigned int y,
    unsigned int image_width,
    unsigned int image_height
) {
    unsigned int x_end;
    unsigned int y_end;

    if (
        image_buffer == NULL ||
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }

    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= epd->width) {
        x_end = epd->width - 1;
    } else {
        x_end = x + image_width - 1;
    }

    if (y + image_height >= epd->height) {
        y_end = epd->height - 1;
    } else {
        y_end = y + image_height - 1;
    }

    printf("epd_set_memory_area: %d, %d, %d, %d\n", x, y, x_end, y_end);
    epd_set_memory_area(epd, x, y, x_end, y_end);
    /* set the frame memory line by line */
    for (unsigned int j = y; j <= y_end; j++) {
        epd_set_memory_pointer(epd, x, j);
        epd_send_command(epd, WRITE_RAM);
        for (unsigned int i = x / 8; i <= x_end / 8; i++) {
            unsigned int buffer_i = (i - x / 8) + (j - y) * (image_width / 8);
            epd_send_data(epd, image_buffer[buffer_i]);
        }
    }
}

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 *
 *          Question: When do you use this function instead of
 *          void SetFrameMemory(
 *              const unsigned char* image_buffer,
 *              int x,
 *              int y,
 *              int image_width,
 *              int image_height
 *          );
 *          Answer: SetFrameMemory with parameters only reads image data
 *          from the RAM but not from the flash in AVR chips (for AVR chips,
 *          you have to use the function pgm_read_byte to read buffers
 *          from the flash).
 */
void epd_set_frame_memory(struct epd * epd, const unsigned char* image_buffer) {
    epd_set_memory_area(epd, 0, 0, epd->width - 1, epd->height - 1);
    /* set the frame memory line by line */
    for (int j = 0; j < epd->height; j++) {
        epd_set_memory_pointer(epd, 0, j);
        epd_send_command(epd, WRITE_RAM);
        for (int i = 0; i < epd->width / 8; i++) {
            unsigned int buffer_i = i + j * (epd->width / 8);
            epd_send_data(epd, pgm_read_byte(&image_buffer[buffer_i]));
        }
    }
}

/**
 *  @brief: clear the frame memory with the specified color.
 *          this won't update the display.
 */
void epd_clear_frame_memory(struct epd * epd, unsigned char color) {
    epd_set_memory_area(epd, 0, 0, epd->width - 1, epd->height - 1);
    /* set the frame memory line by line */
    for (int j = 0; j < epd->height; j++) {
        epd_set_memory_pointer(epd, 0, j);
        epd_send_command(epd, WRITE_RAM);
        for (int i = 0; i < epd->width / 8; i++) {
            epd_send_data(epd, color);
        }
    }
}

/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will
 *          set the other memory area.
 */
void epd_display_frame(struct epd * epd) {
    epd_send_command(epd, DISPLAY_UPDATE_CONTROL_2);
    epd_send_data(epd, 0xC4);
    epd_send_command(epd, MASTER_ACTIVATION);
    epd_send_command(epd, TERMINATE_FRAME_READ_WRITE);
    epd_wait_until_idle(epd);
}

/**
 *  @brief: private function to specify the memory area for data R/W
 */
void epd_set_memory_area(struct epd * epd, int x_start, int y_start, int x_end, int y_end) {
    epd_send_command(epd, SET_RAM_X_ADDRESS_START_END_POSITION);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    epd_send_data(epd, (x_start >> 3) & 0xFF);
    epd_send_data(epd, (x_end >> 3) & 0xFF);
    epd_send_command(epd, SET_RAM_Y_ADDRESS_START_END_POSITION);
    epd_send_data(epd, y_start & 0xFF);
    epd_send_data(epd, (y_start >> 8) & 0xFF);
    epd_send_data(epd, y_end & 0xFF);
    epd_send_data(epd, (y_end >> 8) & 0xFF);
}

/**
 *  @brief: private function to specify the start point for data R/W
 */
void epd_set_memory_pointer(struct epd * epd, int x, int y) {
    epd_send_command(epd, SET_RAM_X_ADDRESS_COUNTER);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    epd_send_data(epd, (x >> 3) & 0xFF);
    epd_send_command(epd, SET_RAM_Y_ADDRESS_COUNTER);
    epd_send_data(epd, y & 0xFF);
    epd_send_data(epd, (y >> 8) & 0xFF);
    epd_wait_until_idle(epd);
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          You can use Init() to awaken
 */
void epd_sleep(struct epd * epd) {
    epd_send_command(epd, DEEP_SLEEP_MODE);
    epd_wait_until_idle(epd);
}

const unsigned char lut_full_update[] =
{
    0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char lut_partial_update[] =
{
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/* END OF FILE */
