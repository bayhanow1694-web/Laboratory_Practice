#include "I2C.h"

void I2C_Config(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // PB8, PB9 → AF
    GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
    GPIOB->MODER |=  (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1);

    GPIOB->OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;

    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);
    GPIOB->PUPDR |=  GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0;

    GPIOB->AFR[1] &= ~((0xF << 0) | (0xF << 4));
    GPIOB->AFR[1] |=  (4 << 0) | (4 << 4);

    // Reset I2C
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // APB1 = 48 MHz
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 48;

    // 100 kHz
    I2C1->CCR = 240;
    I2C1->TRISE = 49;

    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C_Start(void)
{
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C_Stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C_Address(uint8_t addr)
{
    I2C1->DR = addr;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    volatile uint32_t tmp = I2C1->SR1 | I2C1->SR2;
    (void)tmp;
}


void I2C_Write(uint8_t data)
{
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF));
}

void MPU_Write(uint8_t dev, uint8_t reg, uint8_t data)
{
    I2C_Start();
    I2C_Address(dev << 1);
    I2C_Write(reg);
    I2C_Write(data);
    I2C_Stop();
}

void MPU_Read(uint8_t dev, uint8_t reg, uint8_t *buf, uint8_t len)
{
    // Write register address
    I2C_Start();
    I2C_Address(dev << 1);
    I2C_Write(reg);

    // Repeated start for read
    I2C_Start();

    if (len == 1)
    {
        // ОСОБЫЙ СЛУЧАЙ: 1 БАЙТ
        I2C1->CR1 &= ~I2C_CR1_ACK;      // ACK = 0
        I2C_Address((dev << 1) | 1);    // read address
                        

        while (!(I2C1->SR1 & I2C_SR1_RXNE));
        buf[0] = I2C1->DR;
          I2C_Stop(); 
    }
    else
    {
        // Multi-byte
        I2C1->CR1 |= I2C_CR1_ACK;
        I2C_Address((dev << 1) | 1);

        for (uint8_t i = 0; i < len; i++)
        {
            if (i == len - 1)
            {
                I2C1->CR1 &= ~I2C_CR1_ACK;
                I2C_Stop();
            }
            while (!(I2C1->SR1 & I2C_SR1_RXNE));
            buf[i] = I2C1->DR;
        }
    }
}



