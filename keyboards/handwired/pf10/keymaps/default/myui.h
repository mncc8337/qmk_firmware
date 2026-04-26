#pragma once

#include "./lib/ui.h"

typedef enum {
    ROOT_SCREEN_MAIN,
    ROOT_SCREEN_CONFIG
} root_screen_t;

void myui_init(void);

void myui_switch_root(root_screen_t root_screen);

bool myui_event(int keycode);

void myui_render(void);
