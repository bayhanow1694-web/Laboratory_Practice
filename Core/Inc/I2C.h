// #include <stdint.h>
// #include "stm32f4xx.h"
// #include "stm32f411xe.h"

// #define MPU6050_ADDR 0x68
// #define WHO_AM_I_REG 0x75
// #define PWR_MGMT_1_REG 0x6B
// #define SMPLRT_DIV_REG 0x19
// #define ACCEL_CONFIG_REG 0x1C
// #define GYRO_CONFIG_REG 0x1B
// #define ACCEL_XOUT_H_REG 0x3B
// #define GYRO_XOUT_H_REG 0x43

// void I2C_Config(void);
// void I2C_Start(void);
// void I2C_Stop(void);
// void I2C_Write(uint8_t data);
// void I2C_Address(uint8_t address);
// void MPU_Write(uint8_t device_addr, uint8_t reg_addr, uint8_t data);
// void MPU_Read(uint8_t device_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t size);
#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "stm32f411xe.h"

// 7-битный адрес MPU6050
#define MPU6050_ADDR        0x68

#define WHO_AM_I_REG        0x75
#define PWR_MGMT_1_REG      0x6B
#define SMPLRT_DIV_REG      0x19
#define ACCEL_CONFIG_REG    0x1C
#define GYRO_CONFIG_REG     0x1B
#define ACCEL_XOUT_H_REG    0x3B

void I2C_Config(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);
void I2C_Address(uint8_t address);

void MPU_Write(uint8_t dev, uint8_t reg, uint8_t data);
void MPU_Read(uint8_t dev, uint8_t reg, uint8_t *buf, uint8_t len);

#endif
