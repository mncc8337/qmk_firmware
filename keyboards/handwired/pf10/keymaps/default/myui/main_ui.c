#include "defines.h"
#include "myui.h"
#include "bitmap.h"
#include "oled_driver.h"

typedef enum {
    STATE_KEYSTATE = -1,
    STATE_ANIMATION0,
    MAX_STATE
} current_state_t;

current_state_t current_state = STATE_KEYSTATE;

static bool key_state[MATRIX_ROWS][MATRIX_COLS] = {
    { false, false, false, false },
    { false, false, false, false },
    { false, false, false, false },
};
static uint8_t key_pressed_count = 0;

static void main_enter(void) {
    oled_clear();
}

static void draw_animation(void) {
    static uint32_t current_frame = 0;
    static uint16_t current_waittime = 0;
    static uint32_t anim_timer = 0;

    const animation_t *current_anim = animations[current_state];

    if(timer_elapsed32(anim_timer) >= current_waittime) {
        current_frame = (current_frame + 1) % current_anim->frame_count;
        current_waittime = current_anim->frame_durations_ms[current_frame];
        oled_set_cursor(0, 0);
        oled_write_raw((const char*)current_anim->frames[current_frame], 1024);
        anim_timer = timer_read32();
    }
}

static void draw_keystate(void) {
    const uint8_t CELL_WIDTH = 32;
    const uint8_t CELL_HEIGHT = 18;
    const uint8_t CELL_MARGIN = 2;
    const uint8_t BOX_WIDTH = CELL_WIDTH - (CELL_MARGIN * 2);
    const uint8_t BOX_HEIGHT = CELL_HEIGHT - (CELL_MARGIN * 2);
    const uint8_t MAX_KPS = 64;

    static uint16_t current_kps = 0;
    static uint32_t last_kps_calc = 0;
    static uint32_t last_ui_draw = 0;

    static uint16_t displayed_width = 0;

    if(timer_elapsed32(last_kps_calc) > 100) {
        last_kps_calc = timer_read32();
        current_kps = key_pressed_count * 10;
        key_pressed_count = 0;
    }

    if(timer_elapsed32(last_ui_draw) > 33) {
        last_ui_draw = timer_read32();

        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {

                bool is_pressed = key_state[row][col];
                uint8_t start_x = (col * CELL_WIDTH) + CELL_MARGIN;
                uint8_t start_y = (row * CELL_HEIGHT) + CELL_MARGIN;

                for(uint8_t dx = 0; dx < BOX_WIDTH; dx++) {
                    for(uint8_t dy = 0; dy < BOX_HEIGHT; dy++) {

                        uint8_t px = start_x + dx;
                        uint8_t py = start_y + dy;
                        bool pixel_state = false;

                        if(is_pressed) {
                            pixel_state = true;
                        } else {
                            if(
                                dx == 0
                                || dx == (BOX_WIDTH - 1)
                                || dy == 0
                                || dy == (BOX_HEIGHT - 1)
                            ) {
                                pixel_state = true;
                            }
                        }

                        oled_write_pixel(px, py, pixel_state);
                    }
                }
            }
        }

        int16_t gauge_fill_width = current_kps * 128 / MAX_KPS;
        if(gauge_fill_width > 128) gauge_fill_width = 128;

        if(displayed_width < gauge_fill_width) {
            displayed_width += 8;
            if (displayed_width > gauge_fill_width)
                displayed_width = gauge_fill_width;
        } else if(displayed_width > gauge_fill_width) {
            displayed_width -= 4;
            if(displayed_width < gauge_fill_width)
                displayed_width = gauge_fill_width;
        }

        const uint8_t gauge_y_start = 56;

        for(uint8_t x = 0; x < 128; x++) {
            for (uint8_t y = gauge_y_start; y < 64; y++) {
                oled_write_pixel(x, y, (x < displayed_width));
            }
        }
    }
}

static void main_draw(screen_t *self) {
    if(current_state > STATE_KEYSTATE) {
        draw_animation();
    } else {
        draw_keystate();
    }

    // never touch need_redraw flag so this func always run
}

static bool main_event(screen_t *self, uint16_t keycode, keyrecord_t *record) {
    if(keycode == MO(LAYER_SWITCH)) {
        return true;
    }

    if(keycode == KC_SWITCH_ANIMATION) {
        if(record->event.pressed) {
            current_state++;
            if(current_state >= MAX_STATE) {
                current_state = STATE_KEYSTATE;
                oled_clear();
            }
        }
        return false;
    }

    key_state[record->event.key.row][record->event.key.col] = record->event.pressed;
    key_pressed_count += record->event.pressed;

    return true;
}

screen_t screen_main = {
    SCREEN_FREEDRAW,
    true,
    main_event,
    main_enter,
    NULL,
    NULL,
    .draw = main_draw,
};
