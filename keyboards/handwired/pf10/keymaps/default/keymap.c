#include QMK_KEYBOARD_H
#include "defines.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT(
        LT(LAYER_FN, KC_A), KC_B,    KC_C,    RM_TOGG,
        KC_E,               KC_F,    KC_UP,   KC_MUTE,
        KC_I,               KC_LEFT, KC_DOWN, KC_RIGHT
    ),
    [LAYER_CONFIG] = LAYOUT(
        MO(LAYER_FN), KC_NO,   KC_NO,   UI_CLICK,
        UI_LEFT,      KC_NO,   UI_UP,   UI_BACK,
        UI_RIGHT,     UI_BACK, UI_DOWN, UI_CLICK
    ),
    [LAYER_KEYLOCK] = LAYOUT(
        MO(LAYER_FN), KC_NO, KC_NO, KC_NO,
        KC_NO,        KC_NO, KC_NO, KC_NO,
        KC_NO,        KC_NO, KC_NO, KC_NO
    ),
    [LAYER_FN] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, TG(LAYER_CONFIG),
        KC_TRNS, KC_TRNS, KC_TRNS, TG(LAYER_KEYLOCK),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),
};

void keyboard_post_init_kb(void) {
    gpio_set_pin_output(GP25);
    gpio_write_pin_low(GP25);
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    static uint8_t active_keys = 0;
    if(record->event.pressed) {
        active_keys++;
        gpio_write_pin_high(GP25);
    } else {
        if(active_keys > 0) {
            active_keys--;
        }
        if(active_keys == 0) {
            gpio_write_pin_low(GP25);
        }
    }

#if defined(OLED_ENABLE)
    if(record->event.pressed) {
        switch(keycode) {
            case UI_UP:
            case UI_DOWN:
            case UI_LEFT:
            case UI_RIGHT:
            case UI_CLICK:
            case UI_BACK:
                if(!myui_event(keycode)) {
                    return false;
                }
        }
    }
#endif

    return true;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    uint8_t highest_layer = get_highest_layer(state);

    // update the ui according to the highest layer
    switch(highest_layer) {
        case LAYER_BASE:
            myui_switch_root(ROOT_SCREEN_MAIN);
            break;

        case LAYER_CONFIG:
            myui_switch_root(ROOT_SCREEN_CONFIG);
            break;
    }

    return state;
}

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [LAYER_BASE] = {
        ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
        ENCODER_CCW_CW(MS_WHLU, MS_WHLD),
    },
    [LAYER_CONFIG] = {
        ENCODER_CCW_CW(UI_LEFT, UI_RIGHT),
        ENCODER_CCW_CW(UI_UP, UI_DOWN),
    },
    [LAYER_KEYLOCK] = {
        ENCODER_CCW_CW(RM_SPDD, RM_SPDU),
        ENCODER_CCW_CW(RM_PREV, RM_NEXT),
    },
    [LAYER_FN] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    }
};
#endif
