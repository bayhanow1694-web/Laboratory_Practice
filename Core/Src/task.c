#include "task.h"
#include "handlers.h"

void UART_SendChar(char c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}