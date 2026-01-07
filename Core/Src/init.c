#include "init.h"


void GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

    // PC13 — вход
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13);

    // подтяжка вверх (кнопка обычно замыкает на GND)
    MODIFY_REG(GPIOC->PUPDR,
               GPIO_PUPDR_PUPD13_Msk,
               GPIO_PUPDR_PUPD13_0);
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
    
void TIM1_PWM_Init(void)
{
    //Тактирование порта A и TIM2 
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);

    // PA0 и PA1  Alternate Function mode (10)
    MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE8 | GPIO_MODER_MODE9
        | GPIO_MODER_MODE10| GPIO_MODER_MODE11,GPIO_MODER_MODE8_1 
        | GPIO_MODER_MODE9_1| GPIO_MODER_MODE10_1| GPIO_MODER_MODE11_1);

    //Назначаем AF1 (TIM2) для PA0 и PA1
    MODIFY_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9
        | GPIO_AFRH_AFSEL10 | GPIO_AFRH_AFSEL11, 1UL << GPIO_AFRH_AFSEL8_Pos
        | 1UL << GPIO_AFRH_AFSEL9_Pos | 1UL << GPIO_AFRH_AFSEL10_Pos | 1UL << GPIO_AFRH_AFSEL11_Pos);   

    //Останавливаем таймер перед настройкой 
    CLEAR_BIT(TIM1->CR1, TIM_CR1_CEN);

    //при 96 МГц тактовой  96 000 000 / (5+1) / (799+1) = 20 000 Гц
    MODIFY_REG(TIM1->PSC, TIM_PSC_PSC_Msk, 4UL);      // PSC = 6
    MODIFY_REG(TIM1->ARR, TIM_ARR_ARR_Msk, 999UL);    // ARR = 799 → 20 кГц

    //Включаем preload для ARR
    SET_BIT(TIM1->CR1, TIM_CR1_ARPE);

    // Канал 1  PWM mode 1 (110) + preload
    MODIFY_REG(TIM1->CCMR1,TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk, (6UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE);  

    //Канал 2  PWM mode 1 (110) + preload 
    MODIFY_REG(TIM1->CCMR1,TIM_CCMR1_OC2M_Msk | TIM_CCMR1_OC2PE_Msk,(6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE);   // 6 = 110b

    //Канал 3  PWM mode 1 (110) + preload 
    MODIFY_REG(TIM1->CCMR2,TIM_CCMR2_OC3M_Msk | TIM_CCMR2_OC3PE_Msk,(6UL << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE);   // 6 = 110b

    //Канал 2  PWM mode 1 (110) + preload 
    MODIFY_REG(TIM1->CCMR2,TIM_CCMR2_OC4M_Msk | TIM_CCMR2_OC4PE_Msk,(6UL << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE);   // 6 = 110b


    SET_BIT(TIM1->BDTR, TIM_BDTR_MOE);
    // Включаем выходы каналов
    SET_BIT(TIM1->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);

    //Начальные значения скважности = 0%
    MODIFY_REG(TIM1->CCR1, TIM_CCR1_CCR1_Msk, 0UL); // ПЕрвый двигатель 
    MODIFY_REG(TIM1->CCR2, TIM_CCR2_CCR2_Msk, 0UL); // назад

    MODIFY_REG(TIM1->CCR3, TIM_CCR3_CCR3_Msk, 0UL); // второй  двигатель -
    MODIFY_REG(TIM1->CCR4, TIM_CCR4_CCR4_Msk, 0UL); //назад
    //Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM1->EGR, TIM_EGR_UG);

    //Запускаем таймер
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);
}

void UART2_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    
    GPIOA->MODER &= ~(3 << 4);
    GPIOA->MODER |= (2 << 4);
    GPIOA->AFR[0] &= ~(0xF << 8);
    GPIOA->AFR[0] |= (7 << 8);
    
   
      USART2->BRR = 417;  // 48000000 / 115200 = 416.67
    
     USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void IRO_INInt_PB5_PB6(void)
{
    // --- Тактирование порта B ---
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // --- Настройка PB5 и PB6 как вход ---
    GPIOB->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6); // 00 = input
    // Подтяжка к 3.3V для LM393 (открытый коллектор)
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6);
    GPIOB->PUPDR |=  (GPIO_PUPDR_PUPD5_0 | GPIO_PUPDR_PUPD6_0); // 01 = pull-up

    // --- Привязка EXTI к пинам PB5/PB6 ---
    SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI5 | SYSCFG_EXTICR2_EXTI6);
    SYSCFG->EXTICR[1] |=  (SYSCFG_EXTICR2_EXTI5_PB | SYSCFG_EXTICR2_EXTI6_PB);

    // --- Разрешение прерываний EXTI ---
    EXTI->IMR  |= (EXTI_IMR_IM5 | EXTI_IMR_IM6);        // Разрешаем маску прерывания

    EXTI->RTSR |= (EXTI_RTSR_TR5 | EXTI_RTSR_TR6);
    EXTI->FTSR &= ~(EXTI_FTSR_TR5 | EXTI_FTSR_TR6); // спад отключён


    // --- NVIC ---
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}