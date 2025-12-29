#include "I2C.h"


void I2C_Config(void) {

    // Включение тактирования GPIOB и I2C1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    
    // Настройка PB8 и PB9 в режим альтернативной функции
    GPIOB->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;
    
    // Тип выхода - Open Drain
    GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
    
    // Высокая скорость
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
    
    // Pull-up резисторы
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0;
    
    // Альтернативная функция AF4 для I2C1
    GPIOB->AFR[1] |= (4<<0) | (4<<4);
    
    // Сброс I2C1
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    
    // Частота APB1 = 48 МГц
    I2C1->CR2 |= 48 << 0;
    
    // Настройка скорости I2C (100 кГц в стандартном режиме)
    // CCR = 48000000 / (2 * 100000) = 240
    I2C1->CCR = 240 << 0;
    
    // TRISE = 48 + 1 = 49
    I2C1->TRISE = 49;
    
    // Включение I2C1
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C_Start(void) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C_Stop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C_Write(uint8_t data) {
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));
}

void I2C_Address(uint8_t address) {
    I2C1->DR = address;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    uint32_t temp = I2C1->SR1 | I2C1->SR2; // Очистка флага ADDR
}

void MPU_Write(uint8_t device_addr, uint8_t reg_addr, uint8_t data) {
    I2C_Start();
    I2C_Address(device_addr);
    I2C_Write(reg_addr);
    I2C_Write(data);
    I2C_Stop();
}

void MPU_Read(uint8_t device_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size) {
    I2C_Start();
    I2C_Address(device_addr);
    I2C_Write(reg_addr);
    
    I2C_Start(); // Повторный старт
    I2C_Address(device_addr + 1); // Адрес чтения
    
    I2C1->CR1 |= I2C_CR1_ACK;
    
    for (uint8_t i = 0; i < size; i++) {
        if (i == size - 1) {
            I2C1->CR1 &= ~I2C_CR1_ACK; // NACK перед последним байтом
        }
        while (!(I2C1->SR1 & I2C_SR1_RXNE));
        buffer[i] = I2C1->DR;
    }
    
    I2C_Stop();
}

