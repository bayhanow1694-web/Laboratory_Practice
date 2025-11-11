#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"
#define RCC_GPIO_en          (*(uint32_t*)(0x40023800UL+0x30UL)) //включение тактирование на GPIO
#define RCC_GPIOB_en          0x02UL //включение тактирования на GPIOB  
#define RCC_GPIOD_en          0x08UL //включение тактирования на GPIOD

#define RCC_GPIOC_en          0x04UL //включение тактирования на GPIOC

#define READ_GPIO_B9        (*(uint32_t*)(0x40020400UL+0x10UL) & 0x200UL) // Считивание с порта В9

#define RESET_GPIO_C12      (*(uint32_t*)(0x40020800UL+0x18UL)|= 0x10000000UL) //сброс  BSSR12 для порта С12
#define RESET_GPIO_D2       (*(uint32_t*)(0x40020C00UL+0x18UL)|= 0x40000UL) //сброс  BSSR2 для порта D2
#define RESET_GPIO_B7       (*(uint32_t*)(0x40020400UL+0x18UL)|= 0x800000UL) //сброс  BSSR7 для порта B7


void GPIO_Ini(void);
// void dop_test(uint8_t *counter1, uint8_t *button1, uint8_t *prev_state3, uint32_t *fortime2);
void main_task(uint8_t *counter, uint8_t *button, uint8_t *prev_state1, uint8_t *prev_state2);