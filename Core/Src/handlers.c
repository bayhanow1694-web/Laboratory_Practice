#include "handlers.h"

void TIM1_UP_TIM10_IRQHandler(void){
    if (TIM10->SR & TIM_SR_UIF){       // Проверка флага обновления
        TIM10->SR &= ~TIM_SR_UIF;      // Сброс флага
        tick_count++; // Увеличиваем счетчик каждую 1 мс 
          // Уменьшение delay counters прямо в таймере
        if (encoder1_delay > 0) encoder1_delay--;
        if (encoder2_delay > 0) encoder2_delay--;
        
    }
}

void EXTI9_5_IRQHandler(void)
{
    // PB5
    if (EXTI->PR & EXTI_PR_PR5)
    {
        if (encoder1_delay == 0)
        {
            encoder1_count++;
            encoder1_delay = 10; // антидребезг ~10 мс
        }
        EXTI->PR = EXTI_PR_PR5; // сброс флага
    }

    // PB6
    if (EXTI->PR & EXTI_PR_PR6)
    {
        if (encoder2_delay == 0)
        {
            encoder2_count++;
            encoder2_delay = 10; // антидребезг ~10 мс
        }
        EXTI->PR = EXTI_PR_PR6; // сброс флага
    }
}