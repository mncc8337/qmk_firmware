#pragma once

#include QMK_KEYBOARD_H

enum layers {
    LAYER_BASE,
    LAYER_CONFIG,
    LAYER_KEYLOCK,
    LAYER_FN,
};

enum custom_keycodes {
    UI_UP = SAFE_RANGE,
    UI_DOWN,
    UI_LEFT,
    UI_RIGHT,
    UI_CLICK,
    UI_BACK
};

#if defined(OLED_ENABLE)
#include "myui.h"
#endif
