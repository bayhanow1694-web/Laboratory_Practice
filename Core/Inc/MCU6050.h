#ifndef MCU6050_H
#define MCU6050_H

#include <stdint.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include <stdio.h> 


// Структура - только необходимое
typedef struct {
    float yaw_angle;        // Угол поворота вокруг Z (градусы)
    float yaw_rate;         // Угловая скорость (град/с)
    float yaw_rate_raw;     // Сырая скорость (без фильтра)
} MPU6050_t;

// Глобальные переменные
extern MPU6050_t MPU6050;
extern volatile float gz_offset;  // Смещение нуля

// Функции
uint8_t MPU6050_Init(void);
void MPU6050_Calibrate(void);      // Калибровка (робот стоит!)
float MPU6050_Read_Yaw_Rate(void); // Чтение угловой скорости
void MPU6050_Update_Yaw(void);     // Обновление угла
float MPU6050_Get_Yaw(void);       // Получить угол
void MPU6050_Reset_Yaw(void);      // Сбросить угол
float MPU6050_Read_Yaw_Rate_No_Offset(void); //Чтение сырой уголовой скорости без вычитания offset

// Отладка
void MPU6050_Print_Debug(void);

#endif
