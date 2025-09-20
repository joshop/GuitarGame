#pragma once

#include "stm32c0xx_hal.h"

/*  R1SER -> PA0
 *  R2SER -> PC14
 *  R3SER -> PA8
 *  R4SER -> PA3
 *  SRCLK -> PA4
 *  RCLK  -> PA5
 */

#define GPIO_R1SER GPIOA, GPIO_PIN_0
#define GPIO_R2SER GPIOC, GPIO_PIN_14
#define GPIO_R3SER GPIOA, GPIO_PIN_8
#define GPIO_R4SER GPIOA, GPIO_PIN_3
#define GPIO_SRCLK GPIOA, GPIO_PIN_4
#define GPIO_RCLK GPIOA, GPIO_PIN_5

// void shift_all(uint32_t state);
extern volatile uint8_t pwm_buffer[32];
extern volatile uint8_t pwm_threshold;
void shift_all();
void shiftreg_init();
