#pragma once
#include "stdint.h"
#define TOUCH1_IN GPIOC, GPIO_PIN_15
#define TOUCH2_IN GPIOB, GPIO_PIN_7
#define TOUCH1_OUT GPIOA, GPIO_PIN_11
#define TOUCH2_OUT GPIOA, GPIO_PIN_12

#define TOUCH_THRESH 10

uint32_t read_touch1_raw();
uint32_t read_touch2_raw();
void captouch_init();
void read_touch_state(bool *touch1, bool *touch2);
