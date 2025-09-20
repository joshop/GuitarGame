#pragma once
#include <stdint.h>

typedef struct Note {
  uint32_t start;
  uint32_t end;
  uint8_t channel;
} Note;

// extern const uint8_t tvtime_dpcm[];

typedef struct AudioNote {
  float frequency;
  uint32_t end;
} AudioNote;
