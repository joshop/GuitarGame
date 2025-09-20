#include "cap_touch.h"
#include "hardware.h"
#include "its_tv_time.h"
#include "music.h"
#include "shift_register.h"
#include "stm32c0xx_hal.h"
#include "stm32c0xx_hal_tim.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

uint32_t pwmbits[3];
uint8_t which_period = 0;
volatile bool tick_flag;

const Note *all_notes;
uint32_t num_notes;
uint32_t cur_idx = 0;

const int32_t DISPLAY_DIVIDER = 4;
const int32_t DISPLAY_LEN = 9;

const uint8_t LEFT_DISP_IDXS[] = {25, 21, 17, 13, 9, 5, 1, 29, 28, 0};
const uint8_t RIGHT_DISP_IDXS[] = {27, 23, 30, 2, 6, 10, 14, 18, 22, 26};
const uint8_t COMBO_DISP_IDXS[] = {4, 8, 12, 16, 20, 24};
const uint8_t POPU_DISP_IDXS[] = {3, 11, 19, 31, 7, 15};

void place_aa_note(int32_t pos, const uint8_t *base, uint8_t bright) {
  uint8_t sub = pos % DISPLAY_DIVIDER;
  uint8_t start = pos / DISPLAY_DIVIDER;
  switch (sub) {
  case 0:
    if (start != 0)
      pwm_buffer[base[start - 1]] += bright / 2;
    pwm_buffer[base[start]] += bright / 2;
    break;
  case 1:
    if (start != 0)
      pwm_buffer[base[start - 1]] += bright / 4;
    pwm_buffer[base[start]] += 3 * bright / 4;
    break;
  case 2:
    pwm_buffer[base[start]] += bright;
    break;
  case 3:
    if (start != DISPLAY_LEN - 1)
      pwm_buffer[base[start + 1]] += bright / 4;
    pwm_buffer[base[start]] += 3 * bright / 4;
    break;
  default:
  }
}

uint8_t *notes_clicked;
void render_notes() {
  for (uint32_t i = 0; i < num_notes; i++) {

    int32_t dist = all_notes[i].start - cur_idx;
    int32_t dist2 = all_notes[i].end - cur_idx;
    int32_t pos1 = dist;
    int32_t pos2 = dist2;
    if (all_notes[i].end == 0) {
      if (notes_clicked[i])
        continue;
      pos2 = pos1;
    }
    pos2++;
    for (int32_t pos = pos1; pos < pos2; pos++) {
      if (pos < 0 || pos >= DISPLAY_LEN * DISPLAY_DIVIDER)
        continue;
      uint8_t chan = all_notes[i].channel;
      uint8_t base = pos == pos1 ? 100 : 2;
      /*if (chan == 0) {
        pwm_buffer[LEFT_DISP_IDXS[pos]] = base;
      } else {
        pwm_buffer[RIGHT_DISP_IDXS[pos]] = base;
      }*/
      if (chan == 0) {
        place_aa_note(pos, LEFT_DISP_IDXS + 1, base);
      } else {
        place_aa_note(pos, RIGHT_DISP_IDXS + 1, base);
      }
    }
  }
}

uint32_t noteleft1, noteleft2;
uint32_t death_time = 0;
void play_deathsound() {
  float frequency_hz = 200;
  float base = 723.422;
  uint32_t prescaler;
  for (prescaler = 0; prescaler < 65536; prescaler++) {
    if (frequency_hz > base / (prescaler + 1))
      break;
  }
  uint16_t arr = (48000000.0f / ((prescaler + 1) * frequency_hz)) - 1;
  TIM_HandleTypeDef *tim_chan = &htim3;
  tim_chan->Init.Prescaler = prescaler;
  tim_chan->Init.CounterMode = TIM_COUNTERMODE_UP;
  tim_chan->Init.Period = arr;
  tim_chan->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim_chan->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(tim_chan) != HAL_OK) {
    Error_Handler();
  }
  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  // sConfigOC.Pulse = (channel == 1 ? 3 : 1) * arr / 4;
  sConfigOC.Pulse = 1400;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(tim_chan, &sConfigOC, TIM_CHANNEL_1) !=
      HAL_OK) {
    Error_Handler();
  }
}
void play_note(float frequency_hz, uint8_t channel, uint32_t ticks) {
  if (channel == 0 && death_time > 5) {
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_DeInit(&htim3);
    noteleft1 = ticks;
    return;
  }
  if (frequency_hz < 10) {
    if (channel == 0) {
      HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
      HAL_TIM_PWM_DeInit(&htim3);
    } else {
      HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
      HAL_TIM_PWM_DeInit(&htim17);
    }
  } else {
    float base = 723.422;
    uint32_t prescaler;
    for (prescaler = 0; prescaler < 65536; prescaler++) {
      if (frequency_hz > base / (prescaler + 1))
        break;
    }
    uint16_t arr = (48000000.0f / ((prescaler + 1) * frequency_hz)) - 1;
    TIM_HandleTypeDef *tim_chan = channel == 0 ? &htim3 : &htim17;
    tim_chan->Init.Prescaler = prescaler;
    tim_chan->Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_chan->Init.Period = arr;
    tim_chan->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_chan->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(tim_chan) != HAL_OK) {
      Error_Handler();
    }
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    // sConfigOC.Pulse = (channel == 1 ? 3 : 1) * arr / 4;
    sConfigOC.Pulse = channel == 1 ? (frequency_hz < 200 ? 400 : 800)
                                   : (frequency_hz > base ? 2800 : 1400);
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(tim_chan, &sConfigOC, TIM_CHANNEL_1) !=
        HAL_OK) {
      Error_Handler();
    }
  }
  if (channel == 0)
    noteleft1 = ticks;
  else
    noteleft2 = ticks;
  if (frequency_hz > 10) {
    if (noteleft1)
      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    if (noteleft2)
      HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
  }
}
uint8_t flash_cycle = 0;
const uint8_t TIMING_RANGE_EARLY = 15;
const uint8_t TIMING_RANGE_LATE = 8;
int main(void) {
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  // MX_TIM1_Init();
  MX_TIM14_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();

  HAL_Delay(100);
  captouch_init();
  shiftreg_init();
  bool touch1, touch2;
  bool new_touch1, new_touch2;
  new_touch1 = false;
  touch1 = false;
  new_touch2 = false;
  touch2 = false;
  HAL_TIM_Base_Start_IT(&htim14);
  HAL_TIM_Base_Start_IT(&htim16);
start_again:
  //  HAL_TIM_Base_Start_IT(&htim1);
  memset((uint8_t *)pwm_buffer, 1, 32);
  pwm_threshold = 0;
  // uint8_t x = 0;
  all_notes = tvtime_beats;
  num_notes = 501;
  notes_clicked = calloc(num_notes, 1);
  // play_note(880, 0, 60);
  // play_note(500, 1, 60);
  int disable1 = 0;
  int disable2 = 0;
  noteleft1 = disable1 ? 0 : 2.5 * 60;
  noteleft2 = disable2 ? 0 : 2.5 * 60;
  flash_cycle = 0;
  const AudioNote *cur_ch0 = tvtime_left;
  const AudioNote *cur_ch1 = tvtime_right;
  uint8_t lives = 18;
  uint32_t miss_cooldown = 0;
  uint8_t press_flash[2] = {0, 0};
  while (1) {
    if (lives == 0) {
      play_deathsound();
      HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
      HAL_TIM_PWM_DeInit(&htim17);
      HAL_Delay(700);
      HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
      HAL_TIM_PWM_DeInit(&htim3);
      cur_idx = 0;
      goto start_again;
    }
    new_touch1 = touch1;
    new_touch2 = touch2;
    read_touch_state(&touch1, &touch2);
    new_touch1 = touch1 && !new_touch1;
    new_touch2 = touch2 && !new_touch2;
    memset((uint8_t *)pwm_buffer, 0, 32);
    if (death_time > 10) {
      for (uint8_t i = 0; i < 6; i++) {
        pwm_buffer[COMBO_DISP_IDXS[i]] = 12 * (death_time - 10);
      }
    } else {
      for (uint8_t i = 0; i < 1 + (cur_idx * 6 / 10000); i++) {
        pwm_buffer[COMBO_DISP_IDXS[i]] = 100;
      }
    }
    // new_touch1 |= new_touch2;
    // new_touch2 |= new_touch1;
    if ((new_touch1 || new_touch2) && !miss_cooldown) {
      if (new_touch1)
        press_flash[0] = 7;
      if (new_touch2)
        press_flash[1] = 7;
      uint8_t all_miss = 1;
      for (uint32_t i = 0; i < num_notes; i++) {
        if (notes_clicked[i])
          continue;
        if (all_notes[i].channel && !new_touch2)
          continue;
        if (!all_notes[i].channel && !new_touch1)
          continue;
        int32_t dist = all_notes[i].start - cur_idx;
        if (!(dist > TIMING_RANGE_EARLY || dist < -TIMING_RANGE_LATE)) {
          all_miss = 0;
          notes_clicked[i] = 1;
          break;
        }
      }
      if (all_miss) {
        miss_cooldown = 11;
      }
    }
    if (press_flash[0]) {
      for (uint8_t i = 0; i < TIMING_RANGE_EARLY / DISPLAY_DIVIDER + 1; i++) {
        pwm_buffer[LEFT_DISP_IDXS[i]] += 10 * press_flash[0];
      }
      press_flash[0]--;
    }
    if (press_flash[1]) {
      for (uint8_t i = 0; i < TIMING_RANGE_EARLY / DISPLAY_DIVIDER + 1; i++) {
        pwm_buffer[RIGHT_DISP_IDXS[i]] += 10 * press_flash[1];
      }
      press_flash[1]--;
    }
    if (miss_cooldown)
      miss_cooldown--;
    if (death_time == 0) {
      for (uint32_t i = 0; i < num_notes; i++) {
        if (notes_clicked[i])
          continue;
        int32_t dist = all_notes[i].start - cur_idx;
        if (dist == -TIMING_RANGE_LATE - 1) {
          if (lives)
            lives--;
          death_time = 20;
          play_deathsound();
          // HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
          // HAL_TIM_PWM_DeInit(&htim3);
        }
      }
    }
    if (death_time)
      death_time--;
    render_notes();
    //  x++;
    //  HAL_Delay(5000);
    if (!disable1) {
      if (noteleft1)
        noteleft1--;
      if (!noteleft1) {
        play_note(cur_ch0->frequency, 0, cur_ch0->end);
        cur_ch0++;
      }
    }
    if (!disable2) {
      if (noteleft2)
        noteleft2--;
      if (!noteleft2) {
        play_note(cur_ch1->frequency, 1, cur_ch1->end);
        cur_ch1++;
      }
    }
    flash_cycle++;
    pwm_buffer[POPU_DISP_IDXS[0]] = lives > 6 ? 18 : lives;
    pwm_buffer[POPU_DISP_IDXS[3]] = lives > 6 ? 18 : lives;
    pwm_buffer[POPU_DISP_IDXS[1]] =
        lives > 12 ? 18 : (lives < 6 ? 0 : (lives - 6));
    pwm_buffer[POPU_DISP_IDXS[4]] =
        lives > 12 ? 18 : (lives < 6 ? 0 : (lives - 6));
    pwm_buffer[POPU_DISP_IDXS[2]] = lives < 12 ? 0 : (lives - 12);
    pwm_buffer[POPU_DISP_IDXS[5]] = lives < 12 ? 0 : (lives - 12);
    /*if (flash_cycle <= 18) {
      for (uint8_t i = 0; i < 6; i++) {
        pwm_buffer[POPU_DISP_IDXS[i]] = 0;
      }
    }*/

    if (flash_cycle == 24)
      flash_cycle = 0;
    /*if (noteleft2)
      noteleft2--;
    else
      HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);*/
    tick_flag = true;
    while (tick_flag)
      ;
    cur_idx++;
  }
}
const int8_t dpcm_lut[] = {-6, -1, 1, 6};
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim16) {
    pwm_threshold++;
    shift_all();
  } else if (htim == &htim14) {
    tick_flag = false;
  } else if (htim == &htim1) {
  }
}
