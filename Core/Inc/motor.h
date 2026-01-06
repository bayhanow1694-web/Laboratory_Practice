#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "stm32f411xe.h"

// ================== НАСТРОЙКИ ==================
#define MAX_PWM 999

#define WHEEL_DIAMETER_MM  67.5f
#define ENCODER_TICKS_REV  40.0f


#define MM_PER_TICK  ((WHEEL_DIAMETER_MM * 3.1415926f) / ENCODER_TICKS_REV)
// ================== ВНЕШНИЕ ПЕРЕМЕННЫЕ ==================
extern volatile int16_t base_speed;
extern volatile uint8_t robot_started;
extern volatile uint8_t btn_prev;
extern volatile uint32_t btn_time;
extern volatile uint32_t tick_count;
extern volatile float yaw_target;
extern volatile float yaw_i;
extern volatile uint8_t turning;
extern volatile float turn_target;
extern volatile uint8_t driving;
extern volatile float drive_target_mm;

#define OFFSET_LIMIT 20.0f
// ================== INLINE ==================
static inline int16_t clamp_i16(int16_t v, int16_t min, int16_t max)
{
    if (v > max) return max;
    if (v < min) return min;
    return v;
}

// ================== ПРОТОТИПЫ ==================
void Motor_Set(int16_t left, int16_t right);
void Course_Control_1ms(void);
void Button_Process_Main(void);
void Rotate_Process_1ms(void);
void Rotate_Start(float angle_deg);
void Reset_Distance(void);
float Get_Distance_MM(void);
void Drive_Start(float distance_mm);
void Drive_Process_1ms(void);


#endif
