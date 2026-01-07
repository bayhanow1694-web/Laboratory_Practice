#include "init.h"
#include "task.h"
#include "handlers.h"
#include "I2C.h"
#include <stdint.h>
#include <stdio.h>
#include "MCU6050.h"
#include "motor.h"
#include <stdarg.h>

// Определения глобальных переменных (они объявлены как extern в motor.h)
volatile uint32_t tick_count = 0;

// Энкодеры
volatile uint32_t encoder1_count = 0;
volatile uint32_t encoder2_count = 0;
volatile uint8_t encoder1_delay = 0;
volatile uint8_t encoder2_delay = 0;
volatile uint8_t driving = 0;
volatile float drive_target_mm = 0;

// Гироскоп
volatile float gz_offset = 0.0f;

// Управление
volatile int16_t base_speed = 450;
volatile uint8_t robot_started = 0;
volatile uint8_t btn_prev = 1;
volatile uint32_t btn_time = 0;
volatile float yaw_target = 0.0f;
volatile float yaw_i = 0.0f;

// Для motor.c
volatile int16_t left = 0;
volatile int16_t right = 0;
//поворот
volatile uint8_t turning = 0;
volatile float turn_target = 0.0f;
// ---------- МАРШРУТ ----------
typedef struct {
    uint32_t drive_mm;   // ехать прямо (мс)
    float turn_angle;         // угол поворота
} RouteStep_t;

// 10 раз: прямо + поворот (пример)
// RouteStep_t route[] = {
//     {500,  0.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
// };
RouteStep_t route[] = {
    {500,  90.0f},
    {605, 89.5f},
    {30,  -120.5f},
    {460, 59.5f},
    {455,  59.5f},
    {450, 59.5f},
    {450,  60.5f},
    {445, -120.0f},
    {25,  89.5f},
    {595, 89.5f},
};
// RouteStep_t route[] = {
//     {0,  90.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
//     {0,  0.0f},
//     {0, 0.0f},
// };
#define ROUTE_LEN (sizeof(route) / sizeof(route[0]))

// ---------- СОСТОЯНИЯ ----------
enum {
    ST_WAIT = 0,
    ST_CALIBRATE,
    ST_DRIVE,
    ST_TURN
};

uint8_t route_idx = 0;
uint32_t state_time = 0;
void UART_Printf(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    UART_SendString(buf);
}


int main(void)
{
    GPIO_Init();
    RCC_Init();
    TIM10_Init();
    TIM1_PWM_Init();
    IRO_INInt_PB5_PB6();
    I2C_Config();
    UART2_Init(); 

    // Инициализация MPU6050
    if (MPU6050_Init() != 0) {
        UART_SendString("MPU6050 init failed!\r\n");
        while(1);
    }

    UART_SendString("MPU6050 initialized\r\n");
    UART_SendString("Press button to start calibration and movement\r\n");

    uint8_t state = ST_WAIT;
    uint8_t is_calibrated = 0;

    while (1)
    {
        Button_Process_Main();

        if (!robot_started)
        {
            Motor_Set(0, 0);
            driving = 0;
            turning = 0;
            state = ST_WAIT;
            is_calibrated = 0;
            continue;
        }

        MPU6050_Update_Yaw();

        switch (state)
        {
            case ST_WAIT:
                route_idx = 0;
                state = ST_CALIBRATE;
                break;

            case ST_CALIBRATE:
                Motor_Set(0, 0);
                if (!is_calibrated)
                {
                    UART_SendString("Calibrating...\r\n");
                    MPU6050_Calibrate();
                    MPU6050_Reset_Yaw();

                    yaw_target = 0.0f;
                    yaw_i = 0.0f;

                    is_calibrated = 1;

                    // Запускаем движение по дистанции
                    Drive_Start(route[route_idx].drive_mm); // пересчет в мм
                    state = ST_DRIVE;
                }
                break;

            case ST_DRIVE:
                {
                static uint32_t last_debug = 0;
                static uint32_t last_ctrl  = 0;

                // --- Управление движением каждые 1 мс ---
                if ((tick_count - last_ctrl) >= 1)
                {
                last_ctrl = tick_count;
                Drive_Process_1ms();
                }

                // --- Отладка UART каждые 100 мс ---
                if ((tick_count - last_debug) >= 100)
               {
               last_debug = tick_count;
               float dist = Get_Distance_MM();
               float remain = drive_target_mm - dist;

                UART_Printf(
                "[DRIVE] dist=%.1f mm  remain=%.1f mm  yaw=%.2f\r\n",
                dist,
                remain,
                MPU6050.yaw_angle
                );
                }

                //Достигли цели движения
                if (!driving)
                {
                Motor_Set(0, 0);

                 //Коррекция поворота на текущее отклонение
                 float adjusted_turn = route[route_idx].turn_angle - MPU6050.yaw_angle;
                Rotate_Start(adjusted_turn);

                state = ST_TURN;
                }
            }
            break;

            case ST_TURN:
                {
                static uint32_t last_turn_dbg = 0;

                //Управление поворотом каждые 1 мс 
                Rotate_Process_1ms();

                // Отладка UART каждые 100 мс
               if ((tick_count - last_turn_dbg) >= 100)
               {
               last_turn_dbg = tick_count;
               UART_Printf(
               "[TURN] target=%.1f deg  yaw=%.2f deg  e1=%lu e2=%lu\r\n",
               route[route_idx].turn_angle,
               MPU6050.yaw_angle,
               encoder1_count,
               encoder2_count
                );
                }

                //  Достигли угла
                if (!turning)
                {
                UART_Printf(
               "[TURN DONE] target=%.1f  result=%.2f deg\r\n\r\n",
                route[route_idx].turn_angle,
                MPU6050.yaw_angle
                );

                route_idx++;
                if (route_idx >= ROUTE_LEN)
                route_idx = 0;

                // Следующий шаг маршрута — сначала калибровка перед движением
                is_calibrated = 0;
                state = ST_CALIBRATE;
                }
            }
             break;

         }
    }
}