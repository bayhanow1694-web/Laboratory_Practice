#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"
#define RCC_GPIO_en          (*(uint32_t*)(0x40023800UL+0x30UL))
#define RCC_GPIOB_en          0x02UL
#define RCC_GPIOD_en          0x08UL

#define RCC_GPIOC_en          0x04UL

#define READ_GPIO_B9        (*(uint32_t*)(0x40020400UL+0x10UL) & 0x200UL)

#define RESET_GPIO_C12      (*(uint32_t*)(0x40020800UL+0x18UL)|= 0x10000000UL)
#define RESET_GPIO_D2       (*(uint32_t*)(0x40020C00UL+0x18UL)|= 0x40000UL)
#define RESET_GPIO_B7       (*(uint32_t*)(0x40020400UL+0x18UL)|= 0x800000UL)


void GPIO_Ini(void);
// void RCC_Init(void);