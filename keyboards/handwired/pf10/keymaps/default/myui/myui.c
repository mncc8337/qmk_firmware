#include "myui.h"
#include "bitmap.h"
#include "oled_driver.h"

static layer_t current_layer;

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

static const screen_t *layer_screen_map[] = {
    [LAYER_BASE] = &screen_main,
    [LAYER_NUMPAD] = &screen_main,
    [LAYER_NOKIA] = &screen_main,
    [LAYER_CONFIG] = &screen_config,
    [LAYER_LOCK] = &screen_main,
    [LAYER_SWITCH] = &screen_layer_switch,
};

void myui_init(void) {
    screen_led.parent = &screen_config;
    current_layer = LAYER_BASE;
    ui_init(&screen_main);
}

void myui_switch_layer(layer_t active_layer) {
    if(active_layer == current_layer) {
        return;
    }

    current_layer = active_layer;
    ui_open_screen((screen_t*)layer_screen_map[current_layer]);
}
