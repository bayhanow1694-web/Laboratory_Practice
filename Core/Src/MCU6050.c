#include "MCU6050.h"
#include "I2C.h"
#include "task.h"
#include "handlers.h"

#include <stdio.h> 


uint8_t MPU6050_Init(void)
{
    uint8_t check;
    
    MPU_Read(MPU6050_ADDR, WHO_AM_I_REG, &check, 1);
    if (check != 0x68 && check != 0x98) return 1;  // Поддержка поддельных датчиков
    
    MPU_Write(MPU6050_ADDR, PWR_MGMT_1_REG, 0x00);
     uint32_t start_tick = tick_count; // Предполагая, что tick_count растет в прерывании TIM10
       while((tick_count - start_tick) < 30);
    
    MPU_Write(MPU6050_ADDR, SMPLRT_DIV_REG, 0x07);
    MPU_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, 0x00);
    MPU_Write(MPU6050_ADDR, GYRO_CONFIG_REG, 0x00);
    
    return 0;
}

void MPU6050_Read_All(void)
{
    uint8_t d[14];
    MPU_Read(MPU6050_ADDR, ACCEL_XOUT_H_REG, d, 14);

    MPU6050.Accel_X_RAW = (int16_t)((d[0] << 8) | d[1]);
    MPU6050.Accel_Y_RAW = (int16_t)((d[2] << 8) | d[3]);
    MPU6050.Accel_Z_RAW = (int16_t)((d[4] << 8) | d[5]);

    MPU6050.Gyro_X_RAW  = (int16_t)((d[8] << 8) | d[9]);
    MPU6050.Gyro_Y_RAW  = (int16_t)((d[10] << 8) | d[11]);
    MPU6050.Gyro_Z_RAW  = (int16_t)((d[12] << 8) | d[13]);

    MPU6050.Ax = MPU6050.Accel_X_RAW / 16384.0f;
    MPU6050.Ay = MPU6050.Accel_Y_RAW / 16384.0f;
    MPU6050.Az = MPU6050.Accel_Z_RAW / 16384.0f;

    MPU6050.Gx = MPU6050.Gyro_X_RAW / 131.0f;
    MPU6050.Gy = MPU6050.Gyro_Y_RAW / 131.0f;
    MPU6050.Gz = MPU6050.Gyro_Z_RAW / 131.0f;
}


void MPU6050_Send_Data(void)
{
    char buf[80];

    snprintf(buf, sizeof(buf),
        "Ax:%.2f Ay:%.2f Az:%.2f | Gx:%.2f Gy:%.2f Gz:%.2f\r\n",
        MPU6050.Ax, MPU6050.Ay, MPU6050.Az,
        MPU6050.Gx, MPU6050.Gy, MPU6050.Gz);

    for (int i = 0; buf[i]; i++)
        UART_SendChar(buf[i]);
}