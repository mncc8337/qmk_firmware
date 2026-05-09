#pragma once

#include <stdint.h>

typedef struct {
    const uint32_t frame_count;
    const uint16_t *frame_durations_ms;
    const uint8_t (*frames)[1024];
} animation_t;

const animation_t kemomimi;

const animation_t *animations[1];
