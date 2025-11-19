#include "handlers.h"


void TIM1_UP_TIM10_IRQHandler(void){
    if (TIM10->SR & TIM_SR_UIF){       // Проверка флага обновления
        TIM10->SR &= ~TIM_SR_UIF;      // Сброс флага
        tick_count++; // Увеличиваем счетчик каждую 1 мс
        if(tick_count>=1000)
        tick_count=0;
    }
}
void EXTI9_5_IRQHandler(void){
    uint32_t now = tick_count;

    // Сбрасываем флаг
    EXTI->PR = EXTI_PR_PR9;

    // Если кнопка нажата
    if (!(GPIOB->IDR & GPIO_IDR_ID9)) {
        if (btn9_press_time == 0) {
            btn9_press_time = now; // сохраняем момент первого нажатия
        }
    } else {
        // Кнопка отпущена
        if (btn9_press_time != 0) {
            if ((now - btn9_press_time) >= 5) {
                btn9_pressed = 1; // короткое нажатие зафиксировано
            }
            btn9_press_time = 0;
        }
    }
}
