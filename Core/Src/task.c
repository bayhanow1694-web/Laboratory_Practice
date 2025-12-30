#include "task.h"
#include "handlers.h"

void UART_SendChar(char c) {
    while (!(USART2->SR & USART_SR_TXE)); // Ждем, пока буфер передачи освободится
    USART2->DR = (uint8_t)c;              // Явное приведение
}

void UART_SendString(char* str) {
    while (*str) UART_SendChar(*str++);
}