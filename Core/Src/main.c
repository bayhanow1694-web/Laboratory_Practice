#include "init.h"
#include "task.h"
#include "handlers.h"
#include "I2C.h"
#include <stdint.h>

volatile uint32_t tick_count = 0;
volatile uint8_t btn9_pressed = 0; // PB9  смена частоты   
volatile uint32_t btn9_press_time = 0;   // время нажатия PB9
volatile uint8_t counter1 = 0;

volatile uint8_t  btn8_pressed = 0; // PB8 переключение LED
volatile uint32_t btn8_press_time = 0;
volatile uint8_t pwm_mode = 0;

typedef struct {
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    float Ax, Ay, Az;
    float Gx, Gy, Gz;
} MPU6050_t;

MPU6050_t MPU6050;

uint8_t MPU6050_Init(void) {
    uint8_t check;
    uint8_t data;
    
    // Проверка WHO_AM_I (должно быть 0x68)
    MPU_Read(MPU6050_ADDR, WHO_AM_I_REG, &check, 1);
    
    if (check == 104) { // 0x68
        // Пробуждение датчика
        data = 0;
        MPU_Write(MPU6050_ADDR, PWR_MGMT_1_REG, data);
        
        // Установка частоты дискретизации 1 кГц
        data = 0x07;
        MPU_Write(MPU6050_ADDR, SMPLRT_DIV_REG, data);
        
        // Конфигурация акселерометра (±2g)
        data = 0x00;
        MPU_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, data);
        
        // Конфигурация гироскопа (±250°/s)
        data = 0x00;
        MPU_Write(MPU6050_ADDR, GYRO_CONFIG_REG, data);
        
        return 0;
    }
    return 1;
}

void MPU6050_Read_All(void) {
    uint8_t data[14];
    
    // Чтение всех данных (акселерометр + температура + гироскоп)
    MPU_Read(MPU6050_ADDR, ACCEL_XOUT_H_REG, data, 14);
    
    // Акселерометр
    MPU6050.Accel_X_RAW = (int16_t)(data[0] << 8 | data[1]);
    MPU6050.Accel_Y_RAW = (int16_t)(data[2] << 8 | data[3]);
    MPU6050.Accel_Z_RAW = (int16_t)(data[4] << 8 | data[5]);
    
    // Гироскоп
    MPU6050.Gyro_X_RAW = (int16_t)(data[8] << 8 | data[9]);
    MPU6050.Gyro_Y_RAW = (int16_t)(data[10] << 8 | data[11]);
    MPU6050.Gyro_Z_RAW = (int16_t)(data[12] << 8 | data[13]);
    
    // Конвертация в физические величины
    MPU6050.Ax = MPU6050.Accel_X_RAW / 16384.0; // для ±2g
    MPU6050.Ay = MPU6050.Accel_Y_RAW / 16384.0;
    MPU6050.Az = MPU6050.Accel_Z_RAW / 16384.0;
    
    MPU6050.Gx = MPU6050.Gyro_X_RAW / 131.0; // для ±250°/s
    MPU6050.Gy = MPU6050.Gyro_Y_RAW / 131.0;
    MPU6050.Gz = MPU6050.Gyro_Z_RAW / 131.0;
}

void MPU6050_Send_Data(void) {
    char buffer[100];
    
    // Простой вывод для проверки
    UART_SendChar('Z'); // Маркер начала
    
    // Отправка сырых данных в hex
    uint8_t *ptr = (uint8_t*)&MPU6050.Accel_X_RAW;
    for(int i = 0; i < 12; i++) { // 6 значений по 2 байта
        UART_SendChar(ptr[i]);
    }
    
    UART_SendChar('\n');
}

int main(void)
{
    RCC_Init();
    TIM10_Init();
    TIM1_PWM_Init();
    I2C_Config();
    UART2_Init(); 
    MPU6050_Init();

    while (1)
    {
        
      MPU6050_Read_All();
       MPU6050_Send_Data();  // Отправка данных
       
       // Задержка 500 мс
       for(volatile int i = 0; i < 2000000; i++);
     
    }
}