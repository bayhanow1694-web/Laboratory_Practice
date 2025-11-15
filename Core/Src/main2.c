#include "init2.h"
#include <stdint.h>

uint8_t counter = 1;
uint8_t button = 0;
uint8_t prev_state1 = 0;
uint8_t prev_state2 = 0;
int main(void)
{
    RCC_Init();
    GPIO_Ini();
    while (1)
    {
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS5);
    }
}