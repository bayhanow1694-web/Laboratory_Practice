#include "init.h"
#include "task.h"
#include "handlers.h"
#include "I2C.h"
#include <stdint.h>
#include <stdio.h>
#include "MCU6050.h"

volatile uint32_t tick_count = 0;
volatile uint8_t btn9_pressed = 0; // PB9  смена частоты   
volatile uint32_t btn9_press_time = 0;   // время нажатия PB9
volatile uint8_t counter1 = 0;

volatile uint8_t  btn8_pressed = 0; // PB8 переключение LED
volatile uint32_t btn8_press_time = 0;
volatile uint8_t pwm_mode = 0;
MPU6050_t MPU6050;




int main(void)
{
 
    
    RCC_Init();
    TIM10_Init();
    TIM1_PWM_Init();
    TIM3_Encoder_Init();
    I2C_Config();
    UART2_Init(); 
    MPU6050_Init();
    char buffer[16];

    while (1)
    {
        
    //   MPU6050_Read_All();
    // MPU6050_Send_Data();
     uint16_t pos = (uint16_t)TIM3->CNT; // Получаем значение
        
        // Преобразуем число в строку (требует библиотеки stdio.h)
        sprintf(buffer, "Pos: %u\r\n", pos);
        UART_SendString(buffer);
    
      //  uint32_t start_tick = tick_count; // Предполагая, что tick_count растет в прерывании TIM10
      //  while((tick_count - start_tick) < 500); 

     
    }
}