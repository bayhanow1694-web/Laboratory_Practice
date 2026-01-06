// #include <stdint.h>
// #include "stm32f4xx.h"
// #include "stm32f411xe.h"
// typedef struct {
//     int16_t Accel_X_RAW;
//     int16_t Accel_Y_RAW;
//     int16_t Accel_Z_RAW;
//     int16_t Gyro_X_RAW;
//     int16_t Gyro_Y_RAW;
//     int16_t Gyro_Z_RAW;
//     float Ax, Ay, Az;
//     float Gx, Gy, Gz;
// } MPU6050_t;

// extern MPU6050_t MPU6050;
// uint8_t MPU6050_Init(void);
// void MPU6050_Read_All(void);
// void MPU6050_Send_Data(void);

// #ifndef MCU6050_H
// #define MCU6050_H

// #include <stdint.h>
// #include <math.h>
// #include "stm32f4xx.h"
// #include "stm32f411xe.h"

// // Структура данных MPU6050
// typedef struct {
//     int16_t Accel_X_RAW;
//     int16_t Accel_Y_RAW;
//     int16_t Accel_Z_RAW;
//     int16_t Gyro_X_RAW;
//     int16_t Gyro_Y_RAW;
//     int16_t Gyro_Z_RAW;

//     float Ax, Ay, Az;   // отфильтрованные данные акселерометра
//     float Gx, Gy, Gz;   // данные гироскопа
// } MPU6050_t;

// // Глобальные переменные
// extern MPU6050_t MPU6050;
// extern float roll;
// extern float pitch;
// extern float ax_filt;
// extern float ay_filt;
// extern float az_filt;

// // Настройки фильтров
// #define ALPHA_ACC 0.5f       // экспоненциальный фильтр акселерометра
// #define ALPHA_COMP 0.98f     // комплементарный фильтр
// #define DT 0.001f             // 10 мс шаг интегрирования (скорость опроса MPU)

// uint8_t MPU6050_Init(void);
// void MPU6050_Read_All(void);
// void MPU6050_Filter(void);
// void MPU6050_ComputeAngles(void);
// void MPU6050_Send_Data(void);

// #endif
#ifndef MCU6050_H
#define MCU6050_H

#include <stdint.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"

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
float MPU6050_Read_Yaw_Rate_No_Offset(void);


// Отладка
void MPU6050_Print_Debug(void);

#endif
