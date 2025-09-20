#include "stm32c0xx_hal.h"
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim17;
extern volatile bool tick_flag;
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_SPI1_Init(void);
void MX_TIM1_Init(void);
void MX_TIM3_Init(void);
void MX_TIM2_Init(void);
void MX_TIM14_Init(void);
void MX_TIM16_Init(void);
void MX_TIM17_Init(void);
