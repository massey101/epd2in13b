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


int epd_init(struct epd * epd) {
    epd->reset_pin = RST_PIN;
    epd->dc_pin = DC_PIN;
    epd->cs_pin = CS_PIN;
    epd->busy_pin = BUSY_PIN;
    epd->width = EPD_WIDTH;
    epd->height = EPD_HEIGHT;

    /* this calls the peripheral hardware interface, see epdif */
    if (epd_if_init() != 0) {
        return -1;
    }
    /* EPD hardware init start */
    epd_reset(epd);
    epd_send_command(epd, BOOSTER_SOFT_START);
    epd_send_data(epd, 0x17);
    epd_send_data(epd, 0x17);
    epd_send_data(epd, 0x17);
    epd_send_command(epd, POWER_ON);
    epd_wait_until_idle(epd);
    epd_send_command(epd, PANEL_SETTING);
    epd_send_data(epd, 0x8F);
    epd_send_command(epd, VCOM_AND_DATA_INTERVAL_SETTING);
    epd_send_data(epd, 0x37);
    epd_send_command(epd, RESOLUTION_SETTING);
    epd_send_data(epd, 0x68);     // width: 104
    epd_send_data(epd, 0x00);
    epd_send_data(epd, 0xD4);     // height: 212
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
    //LOW: busy, HIGH: idle
    while(epd_if_digital_read(epd->busy_pin) == LOW) {
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
 *  @brief: transmit partial data to the SRAM
 */
void epd_set_partial_window(
    struct epd * epd,
    const unsigned char* buffer_black,
    const unsigned char* buffer_red,
    int x,
    int y,
    int w,
    int l
) {
    epd_send_command(epd, PARTIAL_IN);
    epd_send_command(epd, PARTIAL_WINDOW);
    epd_send_data(epd, x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    epd_send_data(epd, ((x & 0xf8) + w  - 1) | 0x07);
    epd_send_data(epd, y >> 8);
    epd_send_data(epd, y & 0xff);
    epd_send_data(epd, (y + l - 1) >> 8);
    epd_send_data(epd, (y + l - 1) & 0xff);
    epd_send_data(epd, 0x01);         // Gates scan both inside and outside of the partial window. (default)
    _delay_ms(2);
    epd_send_command(epd, DATA_START_TRANSMISSION_1);
    if (buffer_black != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, buffer_black[i]);
        }
    } else {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, 0x00);
        }
    }
    _delay_ms(2);
    epd_send_command(epd, DATA_START_TRANSMISSION_2);
    if (buffer_red != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, buffer_red[i]);
        }
    } else {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, 0x00);
        }
    }
    _delay_ms(2);
    epd_send_command(epd, PARTIAL_OUT);
}


/**
 *  @brief: transmit partial data to the black part of SRAM
 */
void epd_set_partial_window_black(
    struct epd * epd,
    const unsigned char* buffer_black,
    unsigned int x,
    unsigned int y,
    unsigned int w,
    unsigned int l
) {
    epd_send_command(epd, PARTIAL_IN);
    epd_send_command(epd, PARTIAL_WINDOW);
    epd_send_data(epd, x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    epd_send_data(epd, ((x & 0xf8) + w  - 1) | 0x07);
    epd_send_data(epd, y >> 8);
    epd_send_data(epd, y & 0xff);
    epd_send_data(epd, (y + l - 1) >> 8);
    epd_send_data(epd, (y + l - 1) & 0xff);
    epd_send_data(epd, 0x01);         // Gates scan both inside and outside of the partial window. (default)
    _delay_ms(2);
    epd_send_command(epd, DATA_START_TRANSMISSION_1);
    if (buffer_black != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, buffer_black[i]);
        }
    } else {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, 0x00);
        }
    }
    _delay_ms(2);
    epd_send_command(epd, PARTIAL_OUT);
}


/**
 *  @brief: transmit partial data to the black part of SRAM
 */
void epd_set_partial_window_red(
    struct epd * epd,
    const unsigned char* buffer_red,
    unsigned int x,
    unsigned int y,
    unsigned int w,
    unsigned int l
) {
    epd_send_command(epd, PARTIAL_IN);
    epd_send_command(epd, PARTIAL_WINDOW);
    epd_send_data(epd, x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    epd_send_data(epd, ((x & 0xf8) + w  - 1) | 0x07);
    epd_send_data(epd, y >> 8);
    epd_send_data(epd, y & 0xff);
    epd_send_data(epd, (y + l - 1) >> 8);
    epd_send_data(epd, (y + l - 1) & 0xff);
    epd_send_data(epd, 0x01);         // Gates scan both inside and outside of the partial window. (default)
    _delay_ms(2);
    epd_send_command(epd, DATA_START_TRANSMISSION_2);
    if (buffer_red != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, buffer_red[i]);
        }
    } else {
        for(int i = 0; i < w  / 8 * l; i++) {
            epd_send_data(epd, 0x00);
        }
    }
    _delay_ms(2);
    epd_send_command(epd, PARTIAL_OUT);
}

/**
 * @brief: refresh and displays the frame
 */
void epd_display_frame_direct(
    struct epd * epd,
    const unsigned char* frame_buffer_black,
    const unsigned char* frame_buffer_red
) {
    if (frame_buffer_black != NULL) {
        epd_send_command(epd, DATA_START_TRANSMISSION_1);
        _delay_ms(2);
        for (int i = 0; i < epd->width * epd->height / 8; i++) {
            epd_send_data(epd, pgm_read_byte(&frame_buffer_black[i]));
        }
        _delay_ms(2);
    }
    if (frame_buffer_red != NULL) {
        epd_send_command(epd, DATA_START_TRANSMISSION_2);
        _delay_ms(2);
        for (int i = 0; i < epd->width * epd->height / 8; i++) {
            epd_send_data(epd, pgm_read_byte(&frame_buffer_red[i]));
        }
        _delay_ms(2);
    }
    epd_send_command(epd, DISPLAY_REFRESH);
    epd_wait_until_idle(epd);
}


/**
 * @brief: clear the frame data from the SRAM, this won't refresh the display
 */
void epd_clear_frame_memory(struct epd * epd) {
    epd_send_command(epd, DATA_START_TRANSMISSION_1);
    _delay_ms(2);
    for(int i = 0; i < epd->width * epd->height / 8; i++) {
        epd_send_data(epd, 0xFF);
    }
    _delay_ms(2);
    epd_send_command(epd, DATA_START_TRANSMISSION_2);
    _delay_ms(2);
    for(int i = 0; i < epd->width * epd->height / 8; i++) {
        epd_send_data(epd, 0xFF);
    }
    _delay_ms(2);
}


/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will
 *          set the other memory area.
 */
void epd_display_frame(struct epd * epd) {
    epd_send_command(epd, DISPLAY_REFRESH);
    epd_wait_until_idle(epd);
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          You can use Init() to awaken
 */
void epd_sleep(struct epd * epd) {
    epd_send_command(epd, POWER_OFF);
    epd_wait_until_idle(epd);
    epd_send_command(epd, DEEP_SLEEP);
    epd_send_data(epd, 0xA5);
}

/* END OF FILE */
