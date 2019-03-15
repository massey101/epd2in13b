#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>
#include "uart.h"

const FILE uart_stdout = FDEV_SETUP_STREAM(
        uart_printchar,
        NULL,
        _FDEV_SETUP_WRITE
);

const FILE uart_input = FDEV_SETUP_STREAM(
        NULL,
        uart_getchar,
        _FDEV_SETUP_READ
);

const FILE uart_io = FDEV_SETUP_STREAM(
        uart_printchar,
        uart_getchar,
        _FDEV_SETUP_RW
);

void uart_init(uint32_t baud)
{
	/* Set Baud rate */
	UBRR0 = F_CPU/16/baud-1;
	/* Set frame format: 8 bit data */
	UCSR0C=(3<<UCSZ00);
	/* Enable The receiver and transmitter */
	UCSR0B=(1<<RXEN0)|(1<<TXEN0);
}


char uart_getc()
{
	/* Wait untill a data is available */
	while(!(UCSR0A & (1<<RXC0)));
	/* Return data */
	return UDR0;
}


void uart_sendc(char data)
{
	/* Wait untill the transmitter is ready */
	while(!(UCSR0A & (1<<UDRE0)));
	/* Send Data */
	UDR0 = data;
}

char uart_getchar(FILE * stream) {
        char c = uart_getc();
        /* Give return line endings. */
        if (c == '\r') {
            return '\n';
        }
        return c;
}

int uart_printchar(char var, FILE * stream)
{
	/* Give return line endings. */
	if (var == '\n') {
		uart_sendc('\r');
	}
	/* Send character */
	uart_sendc(var);
	return 0;
}
