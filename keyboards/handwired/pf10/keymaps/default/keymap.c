#include "oled_driver.h"
#include QMK_KEYBOARD_H

enum layers {
    LAYER_BASE,
    LAYER_FIDGET
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT(
        KC_A, KC_B, KC_C, RM_NEXT,
        KC_E, KC_F, KC_UP, RM_PREV,
        KC_I, KC_LEFT, KC_DOWN, KC_RIGHT
    ),
    [LAYER_FIDGET] = LAYOUT(
        KC_NO, KC_NO, KC_NO, KC_TRNS,
        KC_NO, KC_NO, KC_NO, KC_TRNS,
        KC_NO, KC_NO, KC_NO, KC_NO
    ),
};

static uint8_t active_keys = 0;

void keyboard_post_init_user(void) {
    gpio_set_pin_output(GP25);
    gpio_write_pin_low(GP25);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
    return true;
}

#ifdef COMBO_ENABLE
enum combo_events {
    COMBO_FIDGET_MODE,
};

const uint16_t PROGMEM fidget_combo[] = {RM_NEXT, RM_PREV, COMBO_END};

combo_t key_combos[] = {
    [COMBO_FIDGET_MODE] = COMBO(fidget_combo, TG(LAYER_FIDGET)),
};
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if(index == 0) {
        if(clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    } else if(index == 1) {
        if(clockwise) {
            tap_code(MS_WHLD);
        } else {
            tap_code(MS_WHLU);
        }
    }
    return false;
}
#endif


#ifdef OLED_ENABLE
#include "bitmap.h"

const char* get_led_mode_name(void) {
    switch(rgb_matrix_get_mode()) {
        case RGB_MATRIX_SOLID_COLOR:              return "Static";
        case RGB_MATRIX_ALPHAS_MODS:              return "Alpha-Mod";
        case RGB_MATRIX_GRADIENT_UP_DOWN:         return "Grad U-D";
        case RGB_MATRIX_GRADIENT_LEFT_RIGHT:      return "Grad L-R";
        case RGB_MATRIX_BREATHING:                return "Breathing";
        case RGB_MATRIX_BAND_SAT:                 return "Band Sat";
        case RGB_MATRIX_BAND_VAL:                 return "Band Val";
        case RGB_MATRIX_BAND_PINWHEEL_SAT:        return "Pinwheel Sat";
        case RGB_MATRIX_BAND_PINWHEEL_VAL:        return "Pinwheel Val";
        case RGB_MATRIX_BAND_SPIRAL_SAT:          return "Spiral Sat";
        case RGB_MATRIX_BAND_SPIRAL_VAL:          return "Spiral Val";
        case RGB_MATRIX_CYCLE_ALL:                return "Cycle All";
        case RGB_MATRIX_CYCLE_LEFT_RIGHT:         return "Cycle L-R";
        case RGB_MATRIX_CYCLE_UP_DOWN:            return "Cycle U-D";
        case RGB_MATRIX_CYCLE_OUT_IN:             return "Cycle O-I";
        case RGB_MATRIX_CYCLE_OUT_IN_DUAL:        return "Cycle Dual";
        case RGB_MATRIX_CYCLE_PINWHEEL:           return "Cycle Pin";
        case RGB_MATRIX_CYCLE_SPIRAL:             return "Cycle Spi";
        case RGB_MATRIX_DUAL_BEACON:              return "Dual Beacon";
        case RGB_MATRIX_RAINBOW_BEACON:           return "Rain Beacon";
        case RGB_MATRIX_RAINBOW_PINWHEELS:        return "Rain Pin";
        case RGB_MATRIX_PIXEL_RAIN:               return "Rain Rain";
        case RGB_MATRIX_PIXEL_FRACTAL:            return "Fractal";
        case RGB_MATRIX_TYPING_HEATMAP:           return "Heatmap";
        case RGB_MATRIX_DIGITAL_RAIN:             return "Matrix";
        case RGB_MATRIX_SOLID_REACTIVE_SIMPLE:    return "S-React";
        case RGB_MATRIX_SOLID_REACTIVE:           return "React";
        case RGB_MATRIX_SOLID_REACTIVE_WIDE:      return "React Wide";
        case RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE: return "React M-W";
        case RGB_MATRIX_SOLID_REACTIVE_CROSS:     return "React Cross";
        case RGB_MATRIX_SOLID_REACTIVE_MULTICROSS:return "React M-C";
        case RGB_MATRIX_SOLID_REACTIVE_NEXUS:     return "Nexus";
        case RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS:return "Nexus Multi";
        case RGB_MATRIX_SPLASH:                   return "Splash";
        case RGB_MATRIX_MULTISPLASH:              return "Splash Multi";
        case RGB_MATRIX_SOLID_SPLASH:             return "Splash Solid";
        case RGB_MATRIX_SOLID_MULTISPLASH:        return "Splash S-M";
        default:                                  return "None";
    }
}

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    return OLED_ROTATION_180;
}

static void render_logo(void) {
    static uint8_t current_frame = 0;
    static uint32_t anim_timer = 0;

    if(timer_elapsed32(anim_timer) > 50) {
        anim_timer = timer_read32();
        current_frame = (current_frame + 1) % epd_bitmap_frames_len;
    }

    oled_set_cursor(0, 0);
    oled_write_raw(epd_bitmap_frames[current_frame], 1024);
}

bool oled_task_user(void) {
    render_logo();
    oled_set_cursor(0, oled_max_lines() - 1);
    oled_write(get_led_mode_name(), false);
    return false;
}
#endif
