#include "task.h"
#include "handlers.h"


void task1(void){
    if(btn9_pressed){
        counter1 = (counter1 + 1) % 5;
        btn9_pressed=0;
          Set_All_LEDs_Brightness();
    }
}
void Set_All_LEDs_Brightness(void)
{
    uint32_t duty_cycle;
    
    switch(counter1) {
        case 0: // Выключено (0%)
            duty_cycle = 0;
            break;
        case 1: // 25%
            duty_cycle = 200;   // 799 * 0.25 ≈ 200
            break;
        case 2: // 50%
            duty_cycle = 400;   // 799 * 0.5 ≈ 400
            break;
        case 3: // 75%
            duty_cycle = 600;   // 799 * 0.75 ≈ 600
            break;
        case 4: // 100%
            duty_cycle = 799;   // Полная яркость
            break;
        default:
            duty_cycle = 0;
            break;
    }
    
    MODIFY_REG(TIM2->CCR1, TIM_CCR1_CCR1_Msk, duty_cycle);  
    MODIFY_REG(TIM2->CCR2, TIM_CCR2_CCR2_Msk, duty_cycle);    
}