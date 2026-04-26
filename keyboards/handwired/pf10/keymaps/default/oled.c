#if defined(OLED_ENABLE)
#include "myui.h"

oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    myui_init();
    return OLED_ROTATION_180;
}

bool oled_task_user(void) {
    myui_render();
    return false;
}

#endif
