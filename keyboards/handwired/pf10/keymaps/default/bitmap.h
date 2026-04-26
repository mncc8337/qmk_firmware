#pragma once

#include <stdint.h>

typedef struct {
    const uint8_t frame_count;
    const uint16_t *frame_durations_ms;
    const uint8_t (*frames)[1024];
} animation_t;

const animation_t KEMOMIMI_GIF;
