#include "handlers.h"

void TIM1_UP_TIM10_IRQHandler(void){
    if (TIM10->SR & TIM_SR_UIF){       // Проверка флага обновления
        TIM10->SR &= ~TIM_SR_UIF;      // Сброс флага
        tick_count++; // Увеличиваем счетчик каждую 1 мс
        if(tick_count==1000)
        tick_count=0;
       
    }
}