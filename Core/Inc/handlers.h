#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"

// Глобальные переменные для подсчёта импульсов
extern volatile uint32_t encoder1_count; // PB5
extern volatile uint32_t encoder2_count; // PB6
extern volatile uint32_t tick_count;
extern volatile uint8_t encoder1_delay;    // антидребезг PB5
extern volatile uint8_t encoder2_delay;    // антидребезг PB6

void TIM1_UP_TIM10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);