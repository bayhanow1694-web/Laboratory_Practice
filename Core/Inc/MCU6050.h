#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"
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

extern MPU6050_t MPU6050;
uint8_t MPU6050_Init(void);
void MPU6050_Read_All(void);
void MPU6050_Send_Data(void);


