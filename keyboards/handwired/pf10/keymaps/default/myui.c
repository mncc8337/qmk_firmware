#include <stdint.h>
#include <string.h>
#include "myui.h"
#include "bitmap.h"
#include "defines.h"
#include "lib/ui.h"
#include "oled_driver.h"
#include "rgb_matrix.h"
#include "util.h"

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

static screen_t* current_screen;
static root_screen_t current_root_screen;

static void open_screen(screen_t *screen) {
    if(current_screen == screen || !screen) {
        return;
    }

    if(current_screen->exit_callback) {
        current_screen->exit_callback();
    }

    current_screen = screen;
    current_screen->need_redraw = true;

    if(current_screen->enter_callback) {
        current_screen->enter_callback();
    }
}

static bool menu_event(screen_t *self, int keycode) {
    menu_t *self_menu = &self->menu;
    menu_item_t *item = &self_menu->items[self_menu->current_item];

    uint8_t resolution = 1;
    switch(keycode) {
        case UI_UP:
            if(self_menu->current_item > 0) {
                self_menu->current_item--;
            } else {
                self_menu->current_item = self_menu->num_items - 1;
            }
            self->need_redraw = true;
            break;

        case UI_DOWN:
            if(self_menu->current_item < self_menu->num_items - 1) {
                self_menu->current_item++;
            } else {
                self_menu->current_item = 0;
            }
            self->need_redraw = true;
            break;

        case UI_CLICK:
            switch(item->type) {
                case ITEM_TOGGLE:
                    *item->data_ptr = !(*item->data_ptr);
                    item->callback(*item->data_ptr);
                    self->need_redraw = true;
                    return false;

                case ITEM_ACTION:
                    if (item->action) item->action();
                    return false;

                case ITEM_SCREEN:
                    open_screen(item->target_screen);
                    return false;

                default:
                    return true;
            }

        case UI_BACK:
            if(self->parent) {
                open_screen(self->parent);
                return false;
            }
            return true;

        case UI_LEFT:
            if(item->type == ITEM_SLIDER) {
                resolution = item->resolution;
            }
            if(item->type == ITEM_SLIDER || item->type == ITEM_LIST) {
                (*item->data_ptr) -= resolution;
                if(item->type == ITEM_LIST && *item->data_ptr >= item->list_count) {
                    *item->data_ptr = item->list_count - 1;
                }
                item->callback(*item->data_ptr);
                self->need_redraw = true;
                return false;
            }
            return true;

        case UI_RIGHT:
            if(item->type == ITEM_SLIDER) {
                resolution = item->resolution;
            }
            if(item->type == ITEM_SLIDER || item->type == ITEM_LIST) {
                (*item->data_ptr) += resolution;
                if(item->type == ITEM_LIST && *item->data_ptr >= item->list_count) {
                    *item->data_ptr = 0;
                }
                item->callback(*item->data_ptr);
                self->need_redraw = true;
                return false;
            }
            return true;

        default:
            return true;
    }

    if(self_menu->current_item < self_menu->first_shown_item) {
        self_menu->first_shown_item = self_menu->current_item;
    } else if(self_menu->current_item >= self_menu->first_shown_item + 7) {
        self_menu->first_shown_item = self_menu->current_item - 6;
    }

    return false;
}

static animation_t *current_anim = (animation_t*)&KEMOMIMI_GIF;
static bool main_event(screen_t *self, int keycode) {
    return true;
}

static void draw_menu(screen_t *screen) {
    if(!screen->need_redraw) return;

    oled_clear();

    menu_t *menu = &screen->menu;

    uint8_t end_item = menu->first_shown_item + 7;
    if(end_item > menu->num_items) {
        end_item = menu->num_items;
    }

    for(uint8_t i = menu->first_shown_item; i < end_item; i++) {
        oled_set_cursor(0, i - menu->first_shown_item);
        // draw the label
        if(i == menu->current_item) {
            oled_write("> ", false);
        } else {
            oled_write("  ", false);
        }
        oled_write(menu->items[i].label, false);

        // now draw the value
        const unsigned FIRST_PART_LEN = 10;
        oled_set_cursor(FIRST_PART_LEN, i - menu->first_shown_item);
        menu_item_t *item = &menu->items[i];
        switch(menu->items[i].type) {
            case ITEM_TOGGLE:
                if(*(bool*)item->data_ptr) {
                    oled_write("enabled", false);
                } else {
                    oled_write("disabled", false);
                }
                break;

            case ITEM_SLIDER:
                if(!item->show_value) {
                    for(int j = 0; j < 21 - FIRST_PART_LEN; j++) {
                        if(*(uint8_t*)item->data_ptr > j * 255 / (21 - FIRST_PART_LEN)) {
                            oled_write_char('=', false);
                        } else {
                            oled_write_char('-', false);
                        }
                    }
                } else {
                    uint8_t val = *item->data_ptr;
                    oled_write_char('0' + val / 100 % 10, false);
                    oled_write_char('0' + val / 10 % 10, false);
                    oled_write_char('0' + val / 1 % 10, false);
                }
                break;

            case ITEM_LIST:
                oled_write(item->name_getter(*item->data_ptr), false);
                break;

            default:
                break;
        }
    }

    screen->need_redraw = false;
}

static void main_draw(screen_t *screen) {
    static uint8_t current_frame = 0;
    static uint16_t current_waittime = 0;
    static uint32_t anim_timer = 0;

    if(timer_elapsed32(anim_timer) >= current_waittime) {
        anim_timer = timer_read32();
        current_frame = (current_frame + 1) % current_anim->frame_count;
        current_waittime = current_anim->frame_durations_ms[current_frame];
        oled_set_cursor(0, 0);
        oled_write_raw((const char*)current_anim->frames[current_frame], 1024);
    }

    // never touch need_redraw flag so this func always run
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
        case RGB_MATRIX_BAND_PINWHEEL_SAT:        return "PinwheelSat";
        case RGB_MATRIX_BAND_PINWHEEL_VAL:        return "PinwheelVal";
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
        case RGB_MATRIX_MULTISPLASH:              return "SplashMulti";
        case RGB_MATRIX_SOLID_SPLASH:             return "SplashSolid";
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

static menu_item_t led_items[] = {
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
    menu_event,
    load_led_config,
    save_led_config,
    NULL,
    .menu = {
        "led config",
        led_items,
        ARRAY_SIZE(led_items),
        0,
        0,
    }
};

static menu_item_t main_items[] = {
    {
        ITEM_SCREEN,
        "led cfg",
        .target_screen = &screen_led,
    },
};
static screen_t screen_config = {
    SCREEN_MENU,
    true,
    menu_event,
    NULL,
    NULL,
    NULL,
    .menu = {
        "main",
        main_items,
        ARRAY_SIZE(main_items),
        0,
        0,
    }
};

static screen_t screen_main = {
    SCREEN_FREEDRAW,
    true,
    main_event,
    NULL,
    .draw = main_draw
};

static const screen_t *root_screen_map[] = {
    [ROOT_SCREEN_MAIN] = &screen_main,
    [ROOT_SCREEN_CONFIG] = &screen_config,
};

void myui_init(void) {
    screen_led.parent = &screen_config;
    current_screen = &screen_main;
    current_screen->need_redraw = true;
    current_root_screen = ROOT_SCREEN_MAIN;
}

void myui_switch_root(root_screen_t root_screen) {
    if(root_screen == current_root_screen) {
        return;
    }

    current_root_screen = root_screen;
    open_screen((screen_t*)root_screen_map[root_screen]);
}

bool myui_event(int keycode) {
    return current_screen->event(current_screen, keycode);
}

void myui_render(void) {
    switch(current_screen->type) {
        case SCREEN_FREEDRAW:
            current_screen->draw(current_screen);
            break;

        case SCREEN_MENU:
            draw_menu(current_screen);
            break;

        default:
            return;
    }
}
