#pragma once

#include QMK_KEYBOARD_H

typedef enum {
    LAYER_BASE,
    LAYER_NUMPAD,
    LAYER_NOKIA,
    LAYER_CONFIG,
    LAYER_LOCK,
    LAYER_SWITCH,
} layer_t;

// DO NOT MOVE THIS
#if defined(OLED_ENABLE)
#include "myui.h"
#endif

// DO NOT MOVE THIS
enum custom_keycodes {
    UIKC_CLICK_OR_LAYER_SWITCH = SAFE_RANGE,
};
