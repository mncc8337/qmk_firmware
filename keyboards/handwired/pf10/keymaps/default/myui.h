#pragma once

#include "defines.h"
#include "./lib/ui.h"

void myui_init(void);
void myui_switch_layer(layer_t active_layer);

static inline bool myui_event(uint16_t keycode, keyrecord_t *record) {
    return ui_event(keycode, record);
}

static inline void myui_render(void) {
    ui_render();
}
