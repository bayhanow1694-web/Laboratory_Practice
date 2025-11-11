#include "init.h"

void GPIO_Ini(void)
{

    RCC_GPIO_en |= RCC_GPIOB_en + RCC_GPIOD_en + RCC_GPIOC_en; // включаем тактирование на C B D портах


    *(uint32_t *)(0x40020800UL + 0x00UL) |= 0x1000000UL; // C moder1 12 cvetadiod output
    *(uint32_t *)(0x40020C00UL + 0x00UL) |= 0x10UL;      // D moder2 01 cvetadiod output
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x4000UL;    // B moder7 01 cvetadiod output

    // OSPEED

    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000UL;  //B порт на средную скороть
    *(uint32_t *)(0x40020800UL + 0x08UL) |= 0x1000000UL; //С порт на средную скороть
    *(uint32_t *)(0x40020C00UL + 0x08UL) |= 0x10UL; //D порт на средную скороть

    // pull_up/down rezistors, устанавливаем нули 

    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00UL;
    *(uint32_t *)(0x40020800UL + 0x0CUL) |= 0x00UL;
    *(uint32_t *)(0x40020C00UL + 0x0CUL) |= 0x00UL;

}



// void dop_test(uint8_t *counter1, uint8_t *button1, uint8_t *prev_state3, uint32_t *fortime2)
// {
//     if (READ_GPIO_B9 == 0)
//     {
//         for (volatile int i = 0; i < 1300; i++)
//             ;
//         if (READ_GPIO_B9 == 0 && *prev_state3 == 0)
//         {
//             *counter1 = (*counter1 + 1) % 4;
//             *button1 = 0;
//             *prev_state3 = 1;
//         }
//     }
//     else
//     {
//         *prev_state3 = 0;
//     }

//     if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
//     {
//         for (volatile int i = 0; i < 1300; i++);
//         if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
//         {
//             while (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
//             {
//                 (*fortime2)++;
//             }
//             if (*fortime2 < 300000)
//             {
//                 *button1 = (*button1 + 1) % 4;
//              }
//             if (*fortime2 >= 300000)
//             {
//                 *counter1 = (*counter1 + 1) % 4;
//                 *button1 = 0;
//             }
//             *fortime2 = 0;
//         }
//     }
  
// }

void main_task(uint8_t *counter, uint8_t *button, uint8_t *prev_state1, uint8_t *prev_state2)
{
if (READ_GPIO_B9 == 0)
        {
            for (volatile int i = 0; i < 2000; i++);
            if (READ_GPIO_B9 == 0 && *prev_state2 == 0)
            {
                (*counter)++;
                RESET_GPIO_C12;
                RESET_GPIO_D2;
                RESET_GPIO_B7;
                if (*counter == 4)
                {
                    *counter = 1;
                }
                *prev_state2 = 1;
            }
        }
        else
        {
            *prev_state2 = 0;
        }

        if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
        {
            for (volatile int i = 0; i < 2000; i++);
            if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0 && *prev_state1 == 0)
            {
                RESET_GPIO_C12;
                RESET_GPIO_D2;
                RESET_GPIO_B7;
                *button = (*button + 1);
                if (*button == 4)
                {
                    *button = 1;
                }
                 if (*button == 3 && *counter == 3)
                {
                    *button = 1;
                }
                
            *prev_state1 = 1;
        }
    }
        else
        {
            *prev_state1 = 0;
        }
}
