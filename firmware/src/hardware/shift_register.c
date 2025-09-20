#include "shift_register.h"
#include "stm32c0xx_hal_gpio.h"

void shiftreg_init() {
  HAL_GPIO_WritePin(GPIO_SRCLK, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIO_RCLK, GPIO_PIN_RESET);
}

static inline void delay_12() {
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
}

uint8_t volatile pwm_buffer[32];
uint8_t volatile pwm_threshold;

static inline void shift_bits(uint8_t *regs) {
  HAL_GPIO_WritePin(GPIO_R1SER, regs[0] & 0x01);
  HAL_GPIO_WritePin(GPIO_R2SER, regs[1] & 0x01);
  HAL_GPIO_WritePin(GPIO_R3SER, regs[2] & 0x01);
  HAL_GPIO_WritePin(GPIO_R4SER, regs[3] & 0x01);
  // HAL_GPIO_WritePin(GPIO_SRCLK, GPIO_PIN_SET);
  GPIOA->BSRR = GPIO_PIN_4;
  // delay_12();
  GPIOA->BSRR = GPIO_PIN_4 << 16;
  // delay_12();
}
static inline void update_regs() {
  // HAL_GPIO_WritePin(GPIO_RCLK, GPIO_PIN_SET);
  GPIOA->BSRR = GPIO_PIN_5;
  // delay_12();
  //  HAL_GPIO_WritePin(GPIO_RCLK, GPIO_PIN_RESET);
  GPIOA->BSRR = GPIO_PIN_5 << 16;
  // delay_12();
}
/*  R1SER -> PA0
 *  R2SER -> PC14
 *  R3SER -> PA8
 *  R4SER -> PA3
 *  SRCLK -> PA4
 *  RCLK  -> PA5
 */
/*
 #define GPIO_R1SER GPIOA, GPIO_PIN_0
 #define GPIO_R2SER GPIOC, GPIO_PIN_14
 #define GPIO_R3SER GPIOA, GPIO_PIN_8
 #define GPIO_R4SER GPIOA, GPIO_PIN_3
 #define GPIO_SRCLK GPIOA, GPIO_PIN_4
 #define GPIO_RCLK GPIOA, GPIO_PIN_5
 */
void shift_all() {
  volatile uint8_t *buf = pwm_buffer;
  uint32_t bsrr_a;
  uint32_t bsrr_c;
#pragma GCC unroll 8
  for (uint8_t i = 0; i < 8; i++) {
    bsrr_a = (buf[0] <= pwm_threshold) ? (1 << (0 + 16)) : (1 << 0);
    bsrr_c = (buf[1] <= pwm_threshold) ? (1 << (14 + 16)) : (1 << 14);
    bsrr_a |= (buf[2] <= pwm_threshold) ? (1 << (8 + 16)) : (1 << 8);
    bsrr_a |= (buf[3] <= pwm_threshold) ? (1 << (3 + 16)) : (1 << 3);
    GPIOC->BSRR = bsrr_c;
    GPIOA->BSRR = bsrr_a | GPIO_PIN_4;
    buf += 4;
    GPIOA->BSRR = GPIO_PIN_4 << 16;
  }
  GPIOA->BSRR = GPIO_PIN_5;
  GPIOA->BSRR = GPIO_PIN_5 << 16;
}
