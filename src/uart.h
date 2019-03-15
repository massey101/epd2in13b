// This code is designed to add uart printf functionality.
//
// In order to use it simply use:
//
//   uart_init(38400);
//   stdout = &uart_stdout;
//   stdin = &uart_stdin;
//
//   printf("Oh jeez Rick");
//

#include <stdio.h>
#include <stdint.h>

const FILE uart_stdout;
const FILE uart_input;
const FILE uart_io;

void uart_init(uint32_t baud);
char uart_getc(void);
void uart_sendc(char data);
char uart_getchar(FILE * stream);
int uart_printchar(char var, FILE * stream);
