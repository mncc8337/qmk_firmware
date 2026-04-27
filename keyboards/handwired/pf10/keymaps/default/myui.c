#include "myui.h"
#include "bitmap.h"

static uint8_t led_enabled;
static uint8_t led_speed;
static uint8_t led_mode;
static uint8_t led_h;
static uint8_t led_s;
static uint8_t led_v;
static uint8_t led_r;
static uint8_t led_g;
static uint8_t led_b;
static bool no_update_rgb_from_hsv = false;

static layer_t current_layer;

enum {
    MAIN_KC_UP = 0,
    MAIN_KC_DOWN,
    MAIN_KC_LEFT,
    MAIN_KC_RIGHT,
    MAIN_KC_Z,
    MAIN_KC_X,
    MAIN_KC_C,
    MAIN_KC_CNT,
};
static bool key_state[MAIN_KC_CNT];
static uint8_t key_pressed_count = 0;
static bool main_event(screen_t *self, uint16_t keycode, keyrecord_t *record) {
    int mapped = -1;
    switch(keycode) {
        case KC_UP:
            mapped = MAIN_KC_UP;
            break;
        case KC_DOWN:
            mapped = MAIN_KC_DOWN;
            break;
        case KC_LEFT:
            mapped = MAIN_KC_LEFT;
            break;
        case KC_RIGHT:
            mapped = MAIN_KC_RIGHT;
            break;
        case KC_Z:
            mapped = MAIN_KC_Z;
            break;
        case KC_X:
            mapped = MAIN_KC_X;
            break;
        case KC_C:
            mapped = MAIN_KC_C;
            break;
    }

    if(mapped >= 0) {
        key_state[mapped] = record->event.pressed;
        key_pressed_count += record->event.pressed;
        self->need_redraw = true;
    }

    return true;
}

static void main_enter(void) {
    oled_clear();
}

static void layer_switch_enter(void) {
    oled_clear();
    oled_set_cursor(0, 0);

    oled_write("conf", IS_LAYER_ON(LAYER_CONFIG));
    oled_write_char('|', false);
    oled_write("lock", IS_LAYER_ON(LAYER_LOCK));
    oled_write_ln("| xx | xx ", false);

    oled_write("nmpd", IS_LAYER_ON(LAYER_NUMPAD));
    oled_write_char('|', false);
    oled_write("nkia", IS_LAYER_ON(LAYER_NOKIA));
    oled_write_ln("| xx | xx ", false);

    oled_write(" xx | xx | xx |", false);
    oled_write_ln("base", true);
}

static void main_draw(screen_t *self) {
    // static uint32_t current_frame = 0;
    // static uint16_t current_waittime = 0;
    // static uint32_t anim_timer = 0;
    //
    // if(timer_elapsed32(anim_timer) >= current_waittime) {
    //     current_frame = (current_frame + 1) % current_anim->frame_count;
    //     current_waittime = current_anim->frame_durations_ms[current_frame];
    //     oled_set_cursor(0, 0);
    //     oled_write_raw((const char*)current_anim->frames[current_frame], 1024);
    //     anim_timer = timer_read32();
    // }
    // // never touch need_redraw flag so this func always run

    static uint16_t kps = 0;
    static uint32_t last_kps_calc = 0;
    static uint32_t last_ui_draw = 0;

    static int16_t displayed_width = 0;

    if(timer_elapsed32(last_kps_calc) > 100) {
        last_kps_calc = timer_read32();
        kps = key_pressed_count * 10;
        key_pressed_count = 0;
    }

    if(timer_elapsed32(last_ui_draw) > 33) {
        last_ui_draw = timer_read32();

        int16_t target_width = (kps * 128) / 57;
        if(target_width > 128) target_width = 128;

        if(displayed_width < target_width) {
            displayed_width += 8;
            if (displayed_width > target_width) displayed_width = target_width;
        } else if(displayed_width > target_width) {
            displayed_width -= 4;
            if(displayed_width < target_width) displayed_width = target_width;
        }

        oled_set_cursor(0, 0);
        oled_write_char(' ', false);
        oled_write_char('0' + key_state[MAIN_KC_UP], false);
        oled_write_char('\n', false);
        oled_write_char('0' + key_state[MAIN_KC_LEFT], false);
        oled_write_char('0' + key_state[MAIN_KC_DOWN], false);
        oled_write_char('0' + key_state[MAIN_KC_RIGHT], false);
        oled_write_char('\n', false);
        oled_write_char('0' + key_state[MAIN_KC_Z], false);
        oled_write_char('0' + key_state[MAIN_KC_X], false);
        oled_write_char('0' + key_state[MAIN_KC_C], false);
        oled_write_char('\n', false);
        oled_write("kps\n", false);

        uint8_t gauge[128] = {0};
        for(int i = 0; i < displayed_width; i++) {
            gauge[i] = 0xFF;
        }

        oled_set_cursor(0, 4);
        oled_write_raw((const char*)gauge, 128);
    }

    // self->need_redraw = false;
}

static void set_led_matrix_state(uint8_t enable) {
    if(enable) {
        rgb_matrix_enable_noeeprom();
    } else {
        rgb_matrix_disable_noeeprom();
    }
}

const char* get_led_mode_name(uint8_t mode) {
    switch(mode) {
        case RGB_MATRIX_NONE:                     return "Off";
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
        case RGB_MATRIX_RAINBOW_MOVING_CHEVRON:   return "R-Chevron";
        case RGB_MATRIX_CYCLE_PINWHEEL:           return "Cycle Pin";
        case RGB_MATRIX_CYCLE_SPIRAL:             return "Cycle Spi";
        case RGB_MATRIX_DUAL_BEACON:              return "Dual Beacon";
        case RGB_MATRIX_RAINBOW_BEACON:           return "Rain Beacon";
        case RGB_MATRIX_RAINBOW_PINWHEELS:        return "Rain Pin";
        case RGB_MATRIX_FLOWER_BLOOMING:          return "Blooming";
        case RGB_MATRIX_RAINDROPS:                return "Raindrops";
        case RGB_MATRIX_JELLYBEAN_RAINDROPS:      return "Jellybean";
        case RGB_MATRIX_HUE_BREATHING:            return "Hue Breath";
        case RGB_MATRIX_HUE_PENDULUM:             return "Hue Pend";
        case RGB_MATRIX_HUE_WAVE:                 return "Hue Wave";
        case RGB_MATRIX_PIXEL_FRACTAL:            return "Fractal";
        case RGB_MATRIX_PIXEL_FLOW:               return "Pixel Flow";
        case RGB_MATRIX_PIXEL_RAIN:               return "Rain Rain";
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
        case RGB_MATRIX_STARLIGHT:                return "Starlight";
        case RGB_MATRIX_STARLIGHT_SMOOTH:         return "Star Smooth";
        case RGB_MATRIX_STARLIGHT_DUAL_HUE:       return "Star Hue";
        case RGB_MATRIX_STARLIGHT_DUAL_SAT:       return "Star Sat";
        case RGB_MATRIX_RIVERFLOW:                return "Riverflow";
        default:                                  return "None";
    }
}

static void update_rgb_from_hsv(void);
static void led_set_hsv(uint8_t dummy) {
    rgb_matrix_sethsv_noeeprom(led_h, led_s, led_v);
    if(!no_update_rgb_from_hsv) {
        update_rgb_from_hsv();
    } else {
        no_update_rgb_from_hsv = false;
    }
}

static void update_rgb_from_hsv(void) {
    if(led_s == 0) {
        led_r = led_v;
        led_g = led_v;
        led_b = led_v;
        return;
    }

    uint8_t region = led_h / 43;
    uint8_t remainder = (led_h - (region * 43)) * 6;

    uint8_t p = (led_v * (255 - led_s)) >> 8;
    uint8_t q = (led_v * (255 - ((led_s * remainder) >> 8))) >> 8;
    uint8_t t = (led_v * (255 - ((led_s * (255 - remainder)) >> 8))) >> 8;

    switch(region) {
        case 0:  led_r = led_v; led_g = t;     led_b = p;     break;
        case 1:  led_r = q;     led_g = led_v; led_b = p;     break;
        case 2:  led_r = p;     led_g = led_v; led_b = t;     break;
        case 3:  led_r = p;     led_g = q;     led_b = led_v; break;
        case 4:  led_r = t;     led_g = p;     led_b = led_v; break;
        default: led_r = led_v; led_g = p;     led_b = q;     break;
    }
}

static void update_hsv_from_rgb(void) {
    uint8_t min_rgb = (led_r < led_g) ? ((led_r < led_b) ? led_r : led_b) : ((led_g < led_b) ? led_g : led_b);
    uint8_t max_rgb = (led_r > led_g) ? ((led_r > led_b) ? led_r : led_b) : ((led_g > led_b) ? led_g : led_b);

    led_v = max_rgb;

    if(led_v == 0) {
        led_h = 0;
        led_s = 0;
        return;
    }

    led_s = 255 * (uint32_t)(max_rgb - min_rgb) / led_v;

    if(led_s == 0) {
        led_h = 0;
        return;
    }

    int32_t delta = max_rgb - min_rgb;
    int32_t h = 0;

    if(max_rgb == led_r) {
        h = 43 * (led_g - led_b) / delta;
    } else if(max_rgb == led_g) {
        h = 85 + 43 * (led_b - led_r) / delta;
    } else {
        h = 171 + 43 * (led_r - led_g) / delta;
    }

    if(h < 0) {
        h += 255;
    }

    led_h = (uint8_t)h;
}

static void load_led_config(void) {
    led_enabled = rgb_matrix_is_enabled();
    led_speed = rgb_matrix_get_speed();
    led_mode = rgb_matrix_get_mode();

    hsv_t color = rgb_matrix_get_hsv();
    led_h = color.h;
    led_s = color.s;
    led_v = color.v;
    update_rgb_from_hsv();
}

static void save_led_config(void) {
    if(led_enabled) {
        rgb_matrix_enable();
    } else {
        rgb_matrix_disable();
    }
    rgb_matrix_set_speed(led_speed);
    rgb_matrix_mode(led_mode);
    rgb_matrix_sethsv(led_h, led_s, led_v);
}

static void led_set_rgb(uint8_t dummy) {
    update_hsv_from_rgb();
    no_update_rgb_from_hsv = true;
    led_set_hsv(dummy);
}

static menu_item_t screen_led_items[] = {
    {
        ITEM_TOGGLE,
        "state",
        .data_ptr = &led_enabled,
        .callback = set_led_matrix_state,
    },
    {
        ITEM_SLIDER,
        "speed",
        .data_ptr = &led_speed,
        .callback = rgb_matrix_set_speed_noeeprom,
        .resolution = 4,
        .show_value = false,
    },
    {
        ITEM_LIST,
        "mode",
        .data_ptr = &led_mode,
        .callback = rgb_matrix_mode_noeeprom,
        .list_count = RGB_MATRIX_EFFECT_MAX,
        .name_getter = get_led_mode_name,
    },
    {
        ITEM_SLIDER,
        "hue",
        .data_ptr = &led_h,
        .callback = led_set_hsv,
        .resolution = 1,
        .show_value = true,
    },
    {
        ITEM_SLIDER,
        "sat",
        .data_ptr = &led_s,
        .callback = led_set_hsv,
        .resolution = 1,
        .show_value = true,
    },
    {
        ITEM_SLIDER,
        "val",
        .data_ptr = &led_v,
        .callback = led_set_hsv,
        .resolution = 1,
        .show_value = true,
    },
    {
        ITEM_SLIDER,
        "red",
        .data_ptr = &led_r,
        .callback = led_set_rgb,
        .resolution = 1,
        .show_value = true,
    },
    {
        ITEM_SLIDER,
        "green",
        .data_ptr = &led_g,
        .callback = led_set_rgb,
        .resolution = 1,
        .show_value = true,
    },
    {
        ITEM_SLIDER,
        "blue",
        .data_ptr = &led_b,
        .callback = led_set_rgb,
        .resolution = 1,
        .show_value = true,
    },
};
static screen_t screen_led = {
    SCREEN_MENU,
    true,
    ui_menu_event,
    load_led_config,
    save_led_config,
    NULL,
    .menu = {
        "led config",
        screen_led_items,
        ARRAY_SIZE(screen_led_items),
        0,
        0,
    }
};

static menu_item_t screen_config_items[] = {
    {
        ITEM_SCREEN,
        "leds config",
        .target_screen = &screen_led,
    },
    {
        ITEM_ACTION,
        "reset settings",
        .action = eeconfig_init,
    },
    {
        ITEM_ACTION,
        "reboot",
        .action = soft_reset_keyboard,
    },
    {
        ITEM_ACTION,
        "enter boot mode",
        .action = reset_keyboard,
    },
};
static screen_t screen_config = {
    SCREEN_MENU,
    true,
    ui_menu_event,
    NULL,
    NULL,
    NULL,
    .menu = {
        NULL,
        screen_config_items,
        ARRAY_SIZE(screen_config_items),
        0,
        0,
    }
};

static screen_t screen_base = {
    SCREEN_FREEDRAW,
    true,
    main_event,
    main_enter,
    NULL,
    NULL,
    .draw = main_draw,
};

static screen_t screen_layer_switch = {
    SCREEN_FREEDRAW,
    true,
    NULL,
    layer_switch_enter,
    NULL,
    NULL,
    .draw = NULL,
};

static const screen_t *layer_screen_map[] = {
    [LAYER_BASE] = &screen_base,
    [LAYER_NUMPAD] = &screen_base,
    [LAYER_NOKIA] = &screen_base,
    [LAYER_CONFIG] = &screen_config,
    [LAYER_LOCK] = &screen_base,
    [LAYER_SWITCH] = &screen_layer_switch,
};

void myui_init(void) {
    screen_led.parent = &screen_config;
    current_layer = LAYER_BASE;
    ui_init(&screen_base);
}

void myui_switch_layer(layer_t active_layer) {
    if(active_layer == current_layer) {
        return;
    }

    current_layer = active_layer;
    ui_open_screen((screen_t*)layer_screen_map[current_layer]);
}
