#include QMK_KEYBOARD_H
#include "defines.h"

enum {
    TAP_DANCE_7, TAP_DANCE_8, TAP_DANCE_9,
    TAP_DANCE_6, TAP_DANCE_5, TAP_DANCE_4,
    TAP_DANCE_1, TAP_DANCE_2, TAP_DANCE_3,
    TAP_DANCE_0,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT(
        KC_Z, KC_ENTER, KC_ESCAPE, MO(LAYER_SWITCH),
        KC_X, KC_F,     KC_UP,     KC_MUTE,
        KC_C, KC_LEFT,  KC_DOWN,   KC_RIGHT
    ),
    [LAYER_NUMPAD] = LAYOUT(
        KC_7, KC_8, KC_9, KC_TRNS,
        KC_4, KC_5, KC_6, KC_TRNS,
        KC_1, KC_2, KC_3, KC_0
    ),
    [LAYER_NOKIA] = LAYOUT(
        TD(TAP_DANCE_1), TD(TAP_DANCE_2), TD(TAP_DANCE_3), KC_TRNS,
        TD(TAP_DANCE_4), TD(TAP_DANCE_5), TD(TAP_DANCE_6), KC_TRNS,
        TD(TAP_DANCE_7), TD(TAP_DANCE_8), TD(TAP_DANCE_9), TD(TAP_DANCE_0)
    ),
    [LAYER_FEATURE] = LAYOUT(
        KC_NO,      KC_NO,     KC_NO,     UIKC_CLICK_OR_LAYER_SWITCH,
        UIKC_BACK,  KC_NO,     UIKC_UP,   UIKC_BACK,
        UIKC_CLICK, UIKC_LEFT, UIKC_DOWN, UIKC_RIGHT
    ),
    [LAYER_CONFIG] = LAYOUT(
        KC_NO,      KC_NO,     KC_NO,     UIKC_CLICK_OR_LAYER_SWITCH,
        UIKC_BACK,  KC_NO,     UIKC_UP,   UIKC_BACK,
        UIKC_CLICK, UIKC_LEFT, UIKC_DOWN, UIKC_RIGHT
    ),
    [LAYER_LOCK] = LAYOUT(
        KC_NO, KC_NO, KC_NO, MO(LAYER_SWITCH),
        KC_NO, KC_NO, KC_NO, KC_SWITCH_ANIMATION,
        KC_NO, KC_NO, KC_NO, KC_NO
    ),
    [LAYER_SWITCH] = LAYOUT(
        TG(LAYER_CONFIG), TG(LAYER_LOCK),  KC_TRNS, KC_TRNS,
        TO(LAYER_NUMPAD), TO(LAYER_NOKIA), KC_TRNS, KC_TRNS,
        KC_TRNS,          KC_TRNS,         KC_TRNS, TO(LAYER_BASE)
    ),
};

#if defined(COMBO_ENABLE)
const uint16_t PROGMEM switch_animation_combo[] = {MO(LAYER_SWITCH), KC_MUTE, COMBO_END};

combo_t key_combos[] = {
    COMBO(switch_animation_combo, KC_SWITCH_ANIMATION),
};
#endif

#if defined(TAP_DANCE_ENABLE)
void dance_1_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 3) {
        case 1: tap_code(KC_ENTER); break;
        case 2: tap_code(KC_1); break;
    }
}

void dance_2_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 5) {
        case 1: tap_code(KC_A); break;
        case 2: tap_code(KC_B); break;
        case 3: tap_code(KC_C); break;
        case 4: tap_code(KC_2); break;
    }
}

void dance_3_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 5) {
        case 1: tap_code(KC_D); break;
        case 2: tap_code(KC_E); break;
        case 3: tap_code(KC_F); break;
        case 4: tap_code(KC_3); break;
    }
}

void dance_4_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 5) {
        case 1: tap_code(KC_G); break;
        case 2: tap_code(KC_H); break;
        case 3: tap_code(KC_I); break;
        case 4: tap_code(KC_4); break;
    }
}

void dance_5_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 5) {
        case 1: tap_code(KC_J); break;
        case 2: tap_code(KC_K); break;
        case 3: tap_code(KC_L); break;
        case 4: tap_code(KC_5); break;
    }
}

void dance_6_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 5) {
        case 1: tap_code(KC_M); break;
        case 2: tap_code(KC_N); break;
        case 3: tap_code(KC_O); break;
        case 4: tap_code(KC_6); break;
    }
}

void dance_7_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 6) {
        case 1: tap_code(KC_P); break;
        case 2: tap_code(KC_Q); break;
        case 3: tap_code(KC_R); break;
        case 4: tap_code(KC_S); break;
        case 5: tap_code(KC_7); break;
    }
}

void dance_8_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 5) {
        case 1: tap_code(KC_T); break;
        case 2: tap_code(KC_U); break;
        case 3: tap_code(KC_V); break;
        case 4: tap_code(KC_8); break;
    }
}

void dance_9_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 6) {
        case 1: tap_code(KC_W); break;
        case 2: tap_code(KC_X); break;
        case 3: tap_code(KC_Y); break;
        case 4: tap_code(KC_Z); break;
        case 5: tap_code(KC_9); break;
    }
}

void dance_0_fin(tap_dance_state_t *state, void *user_data) {
    switch(state->count % 4) {
        case 1: tap_code(KC_SPACE); break;
        case 2: tap_code(KC_BACKSPACE); break;
        case 3: tap_code(KC_0); break;
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TAP_DANCE_1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_1_fin, NULL),
    [TAP_DANCE_2] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_2_fin, NULL),
    [TAP_DANCE_3] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_3_fin, NULL),
    [TAP_DANCE_4] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_4_fin, NULL),
    [TAP_DANCE_5] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_5_fin, NULL),
    [TAP_DANCE_6] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_6_fin, NULL),
    [TAP_DANCE_7] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_7_fin, NULL),
    [TAP_DANCE_8] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_8_fin, NULL),
    [TAP_DANCE_9] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_9_fin, NULL),
    [TAP_DANCE_0] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_0_fin, NULL),
};
#endif

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [LAYER_BASE] = {
        ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
        ENCODER_CCW_CW(MS_WHLU, MS_WHLD),
    },
    [LAYER_NUMPAD] = {
        ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
        ENCODER_CCW_CW(MS_WHLU, MS_WHLD),
    },
    [LAYER_NOKIA] = {
        ENCODER_CCW_CW(KC_VOLD, KC_VOLU),
        ENCODER_CCW_CW(MS_WHLU, MS_WHLD),
    },
    [LAYER_CONFIG] = {
        ENCODER_CCW_CW(UIKC_LEFT, UIKC_RIGHT),
        ENCODER_CCW_CW(UIKC_UP, UIKC_DOWN),
    },
    [LAYER_LOCK] = {
        ENCODER_CCW_CW(RM_SPDD, RM_SPDU),
        ENCODER_CCW_CW(RM_PREV, RM_NEXT),
    },
    [LAYER_SWITCH] = {
        ENCODER_CCW_CW(KC_NO, KC_NO),
        ENCODER_CCW_CW(KC_NO, KC_NO),
    }
};
#endif

void keyboard_post_init_kb(void) {
    gpio_set_pin_output(GP25);
    gpio_write_pin_low(GP25);

    keyboard_post_init_user();
}

static uint16_t custom_lt_timer = 0;
static bool custom_lt_pressed = false;
static bool custom_lt_layer_on = false;

void matrix_scan_kb(void) {
    if(custom_lt_pressed && !custom_lt_layer_on) {
        if(timer_elapsed(custom_lt_timer) > TAPPING_TERM) {
            layer_on(LAYER_SWITCH);
            custom_lt_layer_on = true;
        }
    }

    matrix_scan_user();
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
    // implement momentary layer/tap for UIKC_CLICK_OR_LAYER_SWITCH
    // since qmk does not support keycode
    // larger than 0xFF on LT()
    if(keycode == UIKC_CLICK_OR_LAYER_SWITCH) {
        if(record->event.pressed) {
            custom_lt_timer = timer_read();
            custom_lt_pressed = true;
            custom_lt_layer_on = false;
        } else {
            custom_lt_pressed = false;

            if(custom_lt_layer_on) {
                layer_off(LAYER_SWITCH);
                custom_lt_layer_on = false;
            } else {
                keyrecord_t simulated_press = *record;

                simulated_press.event.pressed = true;
                myui_event(UIKC_CLICK, &simulated_press);

                simulated_press.event.pressed = false;
                myui_event(UIKC_CLICK, &simulated_press);
            }
        }
        return false;
    }

    if(!myui_event(keycode, record)) {
        return false;
    }
#endif

    return process_record_user(keycode, record);
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    myui_switch_layer(get_highest_layer(state));
    return layer_state_set_user(state);
}
