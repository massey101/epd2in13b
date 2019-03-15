/**
 *  @filename   :   epdif.cpp
 *  @brief      :   Implements EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
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

#include "epdif.h"

// CS = PB2
// RESET = PD7
// DC = PD6
// BUSY = PD5

void epd_if_digital_write(int pin, int value) {
    if (pin == RST_PIN) {
        if (value == LOW) {
            PORTD &= ~(1<<PD7);
        } else {
            PORTD |= (1<<PD7);
        }
    }
    if (pin == DC_PIN) {
        if (value == LOW) {
            PORTD &= ~(1<<PD6);
        } else {
            PORTD |= (1<<PD6);
        }
    }
    if (pin == CS_PIN) {
        if (value == LOW) {
            PORTB &= ~(1<<PB2);
        } else {
            PORTB |= (1<<PB2);
        }
    }
    if (pin == BUSY_PIN) {
        if (value == LOW) {
            PORTD &= ~(1<<PD5);
        } else {
            PORTD |= (1<<PD5);
        }
    }
}

int epd_if_digital_read(int pin) {
    if (pin == RST_PIN) {
        return (PIND & (1<<PD7)) >> PD7;
    }
    if (pin == DC_PIN) {
        return (PIND & (1<<PD6)) >> PD6;
    }
    if (pin == CS_PIN) {
        return (PINB & (1<<PB2)) >> PB2;
    }
    if (pin == BUSY_PIN) {
        return (PIND & (1<<PD5)) >> PD5;
    }
    return -1;
}

void epd_if_spi_transfer(unsigned char data) {
    epd_if_digital_write(CS_PIN, LOW);
    SPDR = data;
    while (!(SPSR & (1<<SPIF)));
    epd_if_digital_write(CS_PIN, HIGH);
}

int epd_if_init(void) {
    DDRB = (1<<PB3) | (1<<PB5) | (1<<PB2);
    DDRD = (1<<PD7) | (1<<PD6) | (0<<PD5);
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
    PORTB |= (1<<PB2);
    return 0;
}
