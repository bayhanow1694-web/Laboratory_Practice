#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"

extern volatile uint8_t counter1;

void UART_SendChar(char c);
void UART_SendString(char* str);

