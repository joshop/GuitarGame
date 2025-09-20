#include "cap_touch.h"
#include "hardware.h"
#include "stm32c0xx_hal_gpio.h"
#include "stm32c0xx_hal_tim.h"

uint32_t thresh1, thresh2;
bool calib1, calib2;
/*
#define LOWPASS_LEN 4

uint32_t last_touch1[LOWPASS_LEN];
uint32_t last_touch2[LOWPASS_LEN];

uint32_t rolling(uint32_t *buf, uint32_t new) {
  uint32_t total = 0;
  for (uint8_t i = 1; i < LOWPASS_LEN; i++) {
    total += buf[i];
    buf[i - 1] = buf[i];
  }
  buf[LOWPASS_LEN - 1] = new;
  total += new;
  return total / LOWPASS_LEN;
}
*/

void captouch_init() {
  HAL_GPIO_WritePin(TOUCH1_OUT, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TOUCH2_OUT, GPIO_PIN_RESET);
  HAL_Delay(10);
  /*for (uint8_t i = 0; i < 8; i++) {
    thresh1 = rolling(last_touch1, read_touch1_raw());
    thresh2 = rolling(last_touch2, read_touch2_raw());
    calib1 = false;
    calib2 = false;
  }*/
  thresh1 = 10;
  thresh2 = 10;
  calib1 = true;
  calib2 = true;
}
uint32_t read_touch2_raw() {
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  HAL_GPIO_WritePin(TOUCH2_OUT, GPIO_PIN_SET);
  HAL_TIM_Base_Start(&htim2);
  while (!HAL_GPIO_ReadPin(TOUCH2_IN))
    ;
  uint32_t out = __HAL_TIM_GET_COUNTER(&htim2);
  HAL_TIM_Base_Stop(&htim2);
  HAL_GPIO_WritePin(TOUCH2_OUT, GPIO_PIN_RESET);
  return out;
}
uint32_t read_touch1_raw() {
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  HAL_GPIO_WritePin(TOUCH2_OUT, GPIO_PIN_SET);
  HAL_TIM_Base_Start(&htim2);
  while (!HAL_GPIO_ReadPin(TOUCH1_OUT))
    ;
  uint32_t out = __HAL_TIM_GET_COUNTER(&htim2);
  HAL_TIM_Base_Stop(&htim2);
  HAL_GPIO_WritePin(TOUCH2_OUT, GPIO_PIN_RESET);
  return out;
}

void read_touch_state(bool *touch1, bool *touch2) {
  *touch1 = read_touch1_raw() > thresh1;
  *touch2 = read_touch2_raw() > thresh2;
}
