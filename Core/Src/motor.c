
#include "motor.h"
#include "MCU6050.h"
#include <math.h>
#include "task.h"
#include <stdio.h>
#include "handlers.h"

// PID КОЭФФИЦИЕНТЫ (Подстройка)
static float Kp = 8.0f;   // На 1 градус ошибки меняем ШИМ на 8 единиц
static float Kd = 3.5f;   // Сильное демпфирование, чтобы не раскачивался
static float Ki = 0.2f;   // Небольшой интеграл для компенсации разницы моторов
// Фильтр низких частот для yaw_rate (простейший)
static float yaw_rate_filtered = 0.0f;

// УПРАВЛЕНИЕ МОТОРАМИ
void Motor_Set(int16_t left, int16_t right)
{
    // Жесткое ограничение диапазонов PWM
    left  = clamp_i16(left,  -MAX_PWM, MAX_PWM);
    right = clamp_i16(right, -MAX_PWM, MAX_PWM);

    // Управление каналами таймера
    if (left >= 0) {
        TIM1->CCR1 = left;
        TIM1->CCR2 = 0;
    } else {
        TIM1->CCR1 = 0;
        TIM1->CCR2 = -left;
    }

    if (right >= 0) {
        TIM1->CCR3 = right;
        TIM1->CCR4 = 0;
    } else {
        TIM1->CCR3 = 0;
        TIM1->CCR4 = -right;
    }
}


// КУРСОВОЙ PID
void Course_Control_1ms(void)
{
    if (!robot_started)
    {
        yaw_i = 0.0f;
        Motor_Set(0, 0);
        return;
    }

    // Читаем гироскоп 
    float yaw = MPU6050.yaw_angle;
    yaw_rate_filtered = yaw_rate_filtered * 0.90f + MPU6050.yaw_rate * 0.10f; 

    // Ошибка курса 
    float yaw_error = yaw_target - yaw;
    if (yaw_error > 180.0f) yaw_error -= 360.0f;
    else if (yaw_error < -180.0f) yaw_error += 360.0f;

    // Интеграл 
    if (fabs(yaw_error) < 3.0f)
        yaw_i += yaw_error;
    else
        yaw_i *= 0.9f;

    if (yaw_i > I_LIMIT) yaw_i = I_LIMIT;
    else if (yaw_i < -I_LIMIT) yaw_i = -I_LIMIT;

    if ((yaw_error > 0 && yaw_i < 0) || (yaw_error < 0 && yaw_i > 0))
        yaw_i = 0;

    //PID по курсу
    float corr = (Kp * yaw_error) + (Ki * yaw_i) - (Kd * yaw_rate_filtered);
    int16_t correction_pwm = (int16_t)corr;
    correction_pwm = clamp_i16(correction_pwm, -MAX_CORRECTION, MAX_CORRECTION);

    // Выравнивание моторов по энкодерам 
    uint32_t e1, e2;
    __disable_irq();
    e1 = encoder1_count;
    e2 = encoder2_count;
    __enable_irq();

    // Разница моторов: положительная → левый мотор быстрее
    float motor_error = (float)(e1 - e2) * 10.5f; // коэффициент 0.5 можно подбирать
    int16_t motor_comp = (int16_t)motor_error;

    // Формируем PWM для моторов
    int16_t base_left  = base_speed - motor_comp;
    int16_t base_right = base_speed + motor_comp;

    int16_t left  = base_left + correction_pwm;
    int16_t right = base_right - correction_pwm;

    // Ограничение минимальной скорости
    if (left < 294) left = 304;
    if (right < 280) right = 290;

    Motor_Set(left, right);
}


void Rotate_Start(float angle_deg)
{
   
  MPU6050_Reset_Yaw(); 
    turn_target =  angle_deg;
    turning = 1;
}

void Rotate_Process_1ms(void)
{
    if (!turning)
        return;

    float yaw = MPU6050_Get_Yaw();
    float error = turn_target - yaw;

    // Нормализация ошибки к диапазону -180...+180
    if (error > 180.0f) error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    // Достигли угла
    if (fabs(error) < 0.3f)
    {
        Motor_Set(0, 0);
        turning = 0;
        return;
    }

    //PD регулятор
    float Kp = 4.0f;
    float kd = 0.1f;

    // скорость поворота с демпфированием
    int16_t speed = (int16_t)(Kp * error - kd * MPU6050.yaw_rate);

    // базовый PWM для моторов (левый тугой, правый лёгкий)
    int16_t left_base  = 304; // левый мотор
    int16_t right_base = 290; // правый мотор

    int16_t left_pwm, right_pwm;

    if (speed >= 0)
    {
        left_pwm  = left_base  + speed;
        right_pwm = -(right_base + speed);
    }
    else
    {
        left_pwm  = -left_base + speed;
        right_pwm = right_base - speed;
    }

    // ограничение 
    left_pwm  = clamp_i16(left_pwm,  -380, 380);
    right_pwm = clamp_i16(right_pwm, -380, 380);

    // отправка на моторы
    Motor_Set(left_pwm, right_pwm);
}



// КНОПКА СТАРТ / СТОП
void Button_Process_Main(void)
{
    uint8_t btn = (GPIOC->IDR & GPIO_IDR_ID13) ? 1 : 0;

    if (btn_prev && !btn)
    {
        if ((tick_count - btn_time) > 200)
        {
            robot_started = !robot_started;
            btn_time = tick_count;
            
        }
    }
    btn_prev = btn;
}

void Reset_Distance(void)
{
    __disable_irq();
    encoder1_count = 0;
    encoder2_count = 0;
    __enable_irq();
}
float Get_Distance_MM(void)
{
    uint32_t e1, e2;

    __disable_irq();
    e1 = encoder1_count;
    e2 = encoder2_count;
    __enable_irq();

    return ((e1 + e2) * 0.5f) * MM_PER_TICK;
}
void Drive_Start(float distance_mm)
{
    Reset_Distance();
    drive_target_mm = distance_mm;
    driving = 1;
}
void Drive_Process_1ms(void)
{
    if (!driving)
        return;

    float dist = Get_Distance_MM();
    float remain = drive_target_mm - dist;

    // МЯГКОЕ ТОРМОЖЕНИЕ
    if (remain < 150.0f)   // за 15 см начинаем тормозить
    {
        base_speed = 340;
    }
    if (remain < 60.0f)
    {
        base_speed = 280;
    }

    // ЦЕЛЬ ДОСТИГНУТА
    if (remain <= 0.0f)
    {
        Motor_Set(0, 0);
        driving = 0;
        base_speed = 450;   // вернуть номинал
        return;
    }

    // --- ДЕРЖИМ КУРС ---
    Course_Control_1ms();
}
