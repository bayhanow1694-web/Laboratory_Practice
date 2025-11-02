#include "init.h"

void GPIO_Ini(void){

    RCC_GPIO_en |= RCC_GPIOB_en+RCC_GPIOD_en+RCC_GPIOC_en;
    
    *(uint32_t*)(0x40020800UL+0x00UL) |= 0x1000000UL; // C moder1 12 cvetadiod output
    *(uint32_t*)(0x40020C00UL+0x00UL) |= 0x10UL; // D moder2 01 cvetadiod output
    *(uint32_t*)(0x40020400UL+0x00UL) |= 0x4000UL; // B moder7 01 cvetadiod output

    //*(uint32_t*)(0x40020800UL+0x00UL) |= 0x1500000UL; // C moder12,11,10 01 cvetadiod output
    //*(uint32_t*)(0x40020800UL+0x08UL) |= 0x1500000UL;
    //*(uint32_t*)(0x40020800UL+0x0CUL) |= 0x00UL;

    //OSPEED
    
    *(uint32_t*)(0x40020400UL+0x08UL) |= 0x4000UL;
    *(uint32_t*)(0x40020800UL+0x08UL) |= 0x1000000UL;
    *(uint32_t*)(0x40020C00UL+0x08UL) |= 0x10UL;

    //pull_up/down rezistors 
    
    *(uint32_t*)(0x40020400UL+0x0CUL) |= 0x00UL;
    *(uint32_t*)(0x40020800UL+0x0CUL) |= 0x00UL;
    *(uint32_t*)(0x40020C00UL+0x0CUL) |= 0x00UL;

    // SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN);

    // SET_BIT(GPIOA->MODER, GPIO_MODER_MODE5_0);
    // SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR5);
    
    // //настройка PC9 в режиме алтернативной функции
    // SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1);
    // SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk);
    // MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x0);
}

// void RCC_Init(void){
//     //предварительная очистка регистров RCC устанавливается внутренный высокочастотный генератор
//     MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
//     CLEAR_REG(RCC->CFGR);
//     while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
//     CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
//     while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
//     CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
//     while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
//     CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

//     //настройка RCC_CR
//     SET_BIT(RCC->CR, RCC_CR_HSEON);//запуск внешного генератора
//     while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);
//     CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);//разрешение использование внешного источника
//     SET_BIT(RCC->CR, RCC_CR_CSSON);// ПИн контролья стабилности внешного генератора

    
//     // настройка RCC_PLLCFGR(регистр конфигурации фазовой автоподстройки частоты)
//     CLEAR_REG(RCC->PLLCFGR);
//     SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);//Эта строка устанавливает источник PLL — то есть указывает, откуда PLL должна брать входную частоту
//     MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); //предделитель PLLM2 на 4
//     MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_6);// умножение частоты на 100
//     CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // Настройка предделител итоговая частота
//     SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);

//     //настройка регистора RCC_CFGR(конфигурирует систему тактирования)
//     MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);// выбор PLL
//     MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //предделитель ABH, без делителя
//     MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2); //предделитель APB1, на 2
//     MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1); //предделитель APB2, без делителя
//     MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_0); //предделитель на выходе MCO2(PC9)=3
//     CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2); //настройка на выход
//     MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_3WS);

//     SET_BIT(RCC->CR, RCC_CR_PLLON);//Запуск PLL
//     while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);
// }

// void IRO_INInt(void){
//     SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
//     MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC);

//     SET_BIT(EXIT->IMR, EXTI_IMR_IM13);
//     SET_BIT(EXIT->RTCR, EXTI_RTSR_TR13);
//     CLEAR_BIT(EXIT->FTCR, EXTI_FTSR_TR13);
  

//     NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); 
//     NVIC_EnableIRQ(EXTI15_10_IRQn);

// }