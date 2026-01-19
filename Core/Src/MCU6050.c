#include "handlers.h"
#include "MCU6050.h"
#include "I2C.h"
#include "task.h"


MPU6050_t MPU6050;

// Внутренние переменные
static float gz_filtered = 0.0f;
static float yaw_angle = 0.0f;
static uint32_t last_update = 0; 


// ИНИЦИАЛИЗАЦИЯ (только гироскоп Z)
uint8_t MPU6050_Init(void)
{
    uint8_t check;
    
    // Проверка связи
    MPU_Read(MPU6050_ADDR, WHO_AM_I_REG, &check, 1);
    if (check != 0x68 && check != 0x98) {
        return 1;  // Ошибка
    }

    // Сброс и пробуждение
    MPU_Write(MPU6050_ADDR, PWR_MGMT_1_REG, 0x00);
    
    // Ждём 100 мс
    uint32_t start = tick_count;
    while ((uint32_t)(tick_count - start) < 100);
    
    // Оптимальные настройки для измерения курса:
    
    // 1. Максимальная частота дискретизации
    MPU_Write(MPU6050_ADDR, SMPLRT_DIV_REG, 0x00);  // 1 kHz
    
    // 2. DLPF фильтр 44Hz - убираем высокочастотный шум
    MPU_Write(MPU6050_ADDR, CONFIG_REG, DLPF_44HZ);
    
    // 3. Диапазон ±250°/с (самый точный)
    MPU_Write(MPU6050_ADDR, GYRO_CONFIG_REG, GYRO_RANGE_250DPS);
    
    
    // Инициализация
    gz_filtered = 0.0f;
    yaw_angle = 0.0f;
    last_update = tick_count;
    
    return 0;  // Успех
}

// КАЛИБРОВКА (робот должен стоять НЕПОДВИЖНО!)
void MPU6050_Calibrate(void)
{
    uint32_t start = tick_count;
    float sum = 0.0f;
    uint16_t samples = 0;
    
    // Собираем 1000 семплов (~1 секунда)
    while ((uint32_t)(tick_count - start) < 3000) {
        float gz = MPU6050_Read_Yaw_Rate_No_Offset();  // Чтение без вычитания offset
        sum += gz;
        samples++;
        
        // Небольшая задержка
        uint32_t wait_start = tick_count;
        while ((uint32_t)(tick_count - wait_start) < 1);  // 1 мс
    }
    
    if (samples > 0) {
        gz_offset = sum / samples;
    }
    
    // Сброс фильтра
    gz_filtered = 0.0f;
}

// ЧТЕНИЕ СЫРОЙ угловой скорости (без вычитания offset)
float MPU6050_Read_Yaw_Rate_No_Offset(void)
{
    uint8_t data[2];
    MPU_Read(MPU6050_ADDR, GYRO_ZOUT_H_REG, data, 2);
    
    int16_t raw = (int16_t)((data[0] << 8) | data[1]);
    return raw / GYRO_SCALE;  // Град/с
}


// ЧТЕНИЕ угловой скорости (основная функция)
float MPU6050_Read_Yaw_Rate(void)
{
    // Читаем сырое значение
    float gz_raw = MPU6050_Read_Yaw_Rate_No_Offset();
    
    // Вычитаем калибровочное смещение
    gz_raw -= gz_offset;
    
    // Сохраняем сырое значение (для отладки)
    MPU6050.yaw_rate_raw = gz_raw;
    
    // Простая фильтрация (скользящее среднее)
   
    static float buffer[10] = {0}; 
static float sum = 0;          // Храним сумму здесь
static uint8_t idx = 0;

// Вычитаем старое значение, которое сейчас затрем
sum -= buffer[idx]; 

// Записываем новое
buffer[idx] = gz_raw; 

// Прибавляем новое к сумме
sum += buffer[idx]; 

idx = (idx + 1) % 10;

float gz_filtered = sum / 10.0f; // Всегда всего одна операция деления

    gz_filtered = -gz_filtered;  // Добавьте эту строку!
    
    
    // Сохраняем отфильтрованное
    MPU6050.yaw_rate = gz_filtered;
    
    return gz_filtered;
}


// ОБНОВЛЕНИЕ УГЛА (интегрирование)
void MPU6050_Update_Yaw(void)
{
    uint32_t now = tick_count;
    
    // Вычисляем дельту времени (в секундах)
    float dt = (float)(now - last_update) / 1000.0f;
    
    // Защита от слишком больших dt
    if (dt > 0.1f) dt = 0.001f;  // Если пропустили >100мс, считаем как 1мс
    
    // Читаем угловую скорость
    float gz = MPU6050_Read_Yaw_Rate();
    
    // Интегрируем: угол = угол + скорость * время
    yaw_angle += gz * dt;
    
    // Нормализация угла в диапазон [-180, 180]
    if (yaw_angle > 180.0f) yaw_angle -= 360.0f;
    if (yaw_angle < -180.0f) yaw_angle += 360.0f;
    
    // Сохраняем
    MPU6050.yaw_angle = yaw_angle;
    last_update = now;
}


// ПОЛУЧЕНИЕ ТЕКУЩЕГО УГЛА
float MPU6050_Get_Yaw(void)
{
    return MPU6050.yaw_angle;
}

// СБРОС УГЛА В НОЛЬ
void MPU6050_Reset_Yaw(void)
{
    yaw_angle = 0.0f;
    MPU6050.yaw_angle = 0.0f;
}


// ОТЛАДОЧНЫЙ ВЫВОД
void MPU6050_Print_Debug(void)
{
    static uint32_t last_print = 0;
    
    // Выводим раз в 500 мс
    if ((uint32_t)(tick_count - last_print) > 1000) {
        last_print = tick_count;
        
        char buf[80];
        sprintf(buf, "Yaw: %6.1f° | Rate: %6.2f°/s | Offset: %6.3f\r\n",
                MPU6050.yaw_angle, MPU6050.yaw_rate, gz_offset);
        
        // Предполагаем, что есть UART_SendString
        for(int i = 0; buf[i]; i++) {
            UART_SendChar(buf[i]);  // Или ваша функция отправки
        }
    }
}