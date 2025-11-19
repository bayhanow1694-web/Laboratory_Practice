#include "init.h"

void GPIO_Ini(void)
{
    // инициализация В порта через макросы
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN); // включаем тактирование на  B  порте
     GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
     SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);
}

void RCC_Init(void){
    //предварительная очистка регистров RCC устанавливается внутренный высокочастотный генератор
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    //настройка RCC_CR
    SET_BIT(RCC->CR, RCC_CR_HSEON);//запуск внешного генератора
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);//разрешение использование внешного источника
    SET_BIT(RCC->CR, RCC_CR_CSSON);// ПИн контролья стабилности внешного генератора

    // настройка RCC_PLLCFGR(регистр конфигурации фазовой автоподстройки частоты)
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);//Эта строка устанавливает источник PLL — то есть указывает, откуда PLL должна брать входную частоту
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); //предделитель PLLM2 на 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_6);// умножение частоты на 100
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // Настройка предделител итоговая частота
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);

    //настройка регистора RCC_CFGR(конфигурирует систему тактирования)
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);// выбор PLL
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //предделитель ABH, без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2); //предделитель APB1, на 2
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1); //предделитель APB2, без делителя
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_3WS);

    SET_BIT(RCC->CR, RCC_CR_PLLON);//Запуск PLL
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);
}

void IRO_INInt(void){
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI9_Msk, SYSCFG_EXTICR3_EXTI9_PB);

    SET_BIT(EXTI->IMR, EXTI_IMR_IM9); 
    SET_BIT(EXTI->RTSR,  EXTI_RTSR_TR9); // прерывание по фронту
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR9); // прерывание по спаду

    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); //Установка 0 приоритета прерывания для вектора EXTI9_5
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}


void TIM10_Init(void){
   SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM10EN); 
   
   //при 96 МГц тактовой  96 000 000 / (95+1) / (999+1) = 1000 Гц
    MODIFY_REG(TIM10->PSC, TIM_PSC_PSC_Msk, 95UL);      // PSC = 2
    MODIFY_REG(TIM10->ARR, TIM_ARR_ARR_Msk, 999UL);    // ARR = 799 → 20 кГц

    //Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM10->EGR, TIM_EGR_UG);

     //Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM10->DIER, TIM_DIER_UIE);

      // Включаем прерывание в NVIC
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    //Запускаем таймер
    SET_BIT(TIM10->CR1, TIM_CR1_CEN);

}


void TIM2_PWM_Init(void)
{
    //Тактирование порта A и TIM2 
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

    // PA0 и PA1  Alternate Function mode (10)
    MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE0 | GPIO_MODER_MODE1,GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1);

    //Назначаем AF1 (TIM2) для PA0 и PA1
    MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL0 | GPIO_AFRL_AFSEL1, 1UL << GPIO_AFRL_AFSEL0_Pos |   1UL << GPIO_AFRL_AFSEL1_Pos);   

    //Останавливаем таймер перед настройкой 
    CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);

    //при 48 МГц тактовой  48 000 000 / (2+1) / (799+1) = 20 000 Гц
    MODIFY_REG(TIM2->PSC, TIM_PSC_PSC_Msk, 2UL);      // PSC = 2
    MODIFY_REG(TIM2->ARR, TIM_ARR_ARR_Msk, 799UL);    // ARR = 799 → 20 кГц

    //Включаем preload для ARR
    SET_BIT(TIM2->CR1, TIM_CR1_ARPE);

    // Канал 1  PWM mode 1 (110) + preload
    MODIFY_REG(TIM2->CCMR1,TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk, (6UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE);  

    //Канал 2  PWM mode 1 (110) + preload 
    MODIFY_REG(TIM2->CCMR1,TIM_CCMR1_OC2M_Msk | TIM_CCMR1_OC2PE_Msk,(6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE);   // 6 = 110b

    // Включаем выходы каналов
    SET_BIT(TIM2->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);

    //Начальные значения скважности = 0%
    MODIFY_REG(TIM2->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
    MODIFY_REG(TIM2->CCR2, TIM_CCR2_CCR2_Msk, 0UL);

    //Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM2->EGR, TIM_EGR_UG);

    //Запускаем таймер
    SET_BIT(TIM2->CR1, TIM_CR1_CEN);
}
    