#include "myui.h"
#include "oled_driver.h"

static void layer_switch_enter(void) {
    oled_clear();
    oled_set_cursor(0, 0);

    oled_write("conf", IS_LAYER_ON(LAYER_CONFIG));
    oled_write_char('|', false);
    oled_write("lock", IS_LAYER_ON(LAYER_LOCK));
    oled_write("| xx |", false);
    oled_write_ln(" ls ", true);

    oled_write("nmpd", IS_LAYER_ON(LAYER_NUMPAD));
    oled_write_char('|', false);
    oled_write("nkia", IS_LAYER_ON(LAYER_NOKIA));
    oled_write_ln("| xx | xx ", false);

    oled_write(" xx | xx | xx |", false);
    oled_write_ln("base", true);
}

screen_t screen_layer_switch = {
    SCREEN_FREEDRAW,
    true,
    NULL,
    layer_switch_enter,
    NULL,
    NULL,
    .draw = NULL,
};

