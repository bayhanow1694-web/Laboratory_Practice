#include "init.h"
#include <stdint.h>

// uint32_t fortime2 = 0;
uint8_t counter = 1;
uint8_t button = 0;
uint8_t prev_state1 = 0;
uint8_t prev_state2 = 0;


int main(void)
{

    // uint8_t counter = 0;
    // uint8_t button = 0;
    // uint32_t timeArr[4] = {0, 0, 0, 0};

    // RCC_Init();
    GPIO_Ini();

    while (1)
    {
        //         SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS5);
        //     }
        // }



     //доролнительная задача   
        //     if(READ_GPIO_B9==0)
        //     {
        //         for(volatile int i=0; i<1300; i++);
        //         if(READ_GPIO_B9==0 && prev_state2==0)
        //         {
        //             counter=(counter+1)%4;
        //             button=0;
        //             prev_state2 = 1;
        //         }
        //     }
        //     else
        //     {
        //         prev_state2 = 0;
        //     }

        //     if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8)==0){
        //     for(volatile int i=0; i<1300; i++);
        //     if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8)==0){
        //         while(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8)==0)
        //             {
        //                fortime2++;
        //             }
        //     if(fortime2<300000){
        //     button=(button+1)%4;
        //       switch(button) {
        //         case 0: timeArr[counter]= 0; break;
        //         case 1: timeArr[counter]= 30000; break;
        //         case 2: timeArr[counter]= 70000; break;
        //         case 3: timeArr[counter]= 150000; break;
        //              }
        //             }
        //             if(fortime2>=300000){
        //                 counter=(counter+1)%4;
        //                 button=0;
        //             }
        //             fortime2=0;
        //             }

        // }
        //     if (counter == 1)
        //         {
        //             RESET_GPIO_C12;
        //             RESET_GPIO_D2;
        //             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_7);
        //             for (volatile int i = 0; i < timeArr[counter]; i++)
        //             {
        //                 if(READ_GPIO_B9==0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
        //                 break;

        //             }
        //             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_7);
        //             for (volatile int i = 0; i < timeArr[counter]; i++){
        //                   if(READ_GPIO_B9==0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
        //                 break;
        //             }
        //         }

        //         else if (counter == 2)
        //         {
        //             RESET_GPIO_B7;
        //             RESET_GPIO_D2;
        //             SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS_12); // Включить
        //             for (volatile int i = 0; i < timeArr[counter]; i++){
        //                   if(READ_GPIO_B9==0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
        //                 break;
        //             }
        //             SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR_12); // Выключить
        //             for (volatile int i = 0; i < timeArr[counter]; i++)
        //             {
        //                   if(READ_GPIO_B9==0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
        //                 break;
        //             }
        //         }
        //         else if (counter == 3)
        //         {
        //             RESET_GPIO_B7;
        //             RESET_GPIO_C12;
        //             SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS_2); // Включить
        //             for (volatile int i = 0; i < timeArr[counter]; i++){
        //                   if(READ_GPIO_B9==0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
        //                 break;
        //             }
        //             SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR_2); // Выключить
        //             for (volatile int i = 0; i < timeArr[counter]; i++){
        //                   if(READ_GPIO_B9==0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
        //                 break;
        //             }
        //         }
        // }
        // }

        if (READ_GPIO_B9 == 0)
        {
            for (volatile int i = 0; i < 2000; i++);
            if (READ_GPIO_B9 == 0 && prev_state2 == 0)
            {
                counter++;
                RESET_GPIO_C12;
                RESET_GPIO_D2;
                RESET_GPIO_B7;
                if (counter == 4)
                {
                    counter = 1;
                }
                prev_state2 = 1;
            }
        }
        else
        {
            prev_state2 = 0;
        }

        if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
        {
            for (volatile int i = 0; i < 2000; i++);
            if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0 && prev_state1 == 0)
            {
                RESET_GPIO_C12;
                RESET_GPIO_D2;
                RESET_GPIO_B7;
                button = (button + 1);
                if (button == 4)
                {
                    button = 1;
                }
                 if (button == 3 && counter == 3)
                {
                    button = 1;
                }
                if( counter == 1){
                switch (button)
                {
                case 1:
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS_12);
                    break;
                case 2:
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_7);
                    break;
                case 3:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS_2);
                    break;
                default:
                    break;
                } 
            }
            if( counter == 2){
               switch (button)
                {
                case 1:

                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
                    
                    break;
                case 2:

                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
                   
                    break;
                case 3:

                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                   
                    break;
                default:
                    break;
                } 
            }
             if( counter == 3){
               switch (button)
                {
                case 1:
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                    break;
                case 2:
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                    break;
                default:
                    break;
                }  
            }
            prev_state1 = 1;
        }
    }
        else
        {
            prev_state1 = 0;
        }
    }
}