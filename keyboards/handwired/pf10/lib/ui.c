#include "ui.h"

static uint8_t first_column_width = 8;

static screen_t* current_screen;

void ui_init(screen_t *default_screen) {
    current_screen = default_screen;
    current_screen->need_redraw = true;
}

void ui_open_screen(screen_t *screen) {
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

void ui_menu_set_first_column_width(uint8_t new_width) {
    first_column_width = new_width;
}

void ui_menu_draw(screen_t *self) {
    if(!self->need_redraw) return;

    oled_clear();

    menu_t *menu = &self->menu;

    uint8_t end_item = menu->first_shown_item + 7;
    if(end_item > menu->num_items) {
        end_item = menu->num_items;
    }

    uint8_t first_column_width_cpy = first_column_width;
    for(uint8_t i = menu->first_shown_item; i < end_item; i++) {
        first_column_width_cpy = first_column_width;
        oled_set_cursor(0, i - menu->first_shown_item);
        // draw the label
        if(i == menu->current_item) {
            oled_write("> ", false);
        } else {
            oled_write("  ", false);
        }
        oled_write(menu->items[i].label, false);
        if(i == menu->current_item && menu->items[i].type == ITEM_SCREEN) {
            oled_write("[*]", false);
        }

        // now draw the value
        oled_set_cursor(first_column_width_cpy, i - menu->first_shown_item);
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
                {
                    if(item->show_value) {
                        uint8_t val = *item->data_ptr;
                        oled_write_char('0' + val / 100 % 10, false);
                        oled_write_char('0' + val / 10 % 10, false);
                        oled_write_char('0' + val / 1 % 10, false);
                        first_column_width_cpy += 3;
                    }
                    uint8_t buff_size = 128 - first_column_width_cpy * 6;
                    char buffer[128];
                    uint8_t val = *item->data_ptr * buff_size / 255;
                    for(int i = 0; i < buff_size; i++) {
                        buffer[i] = (i <= val) * 0b00111100 + (i > val) * 0b00010000;
                    }
                    oled_write_raw(buffer, buff_size);
                }
                break;

            case ITEM_LIST:
                oled_write(item->name_getter(*item->data_ptr), false);
                break;

            default:
                break;
        }
    }

    self->need_redraw = false;
}

bool ui_menu_event(screen_t *self, uint16_t keycode, keyrecord_t *record) {
    if(keycode < UIKC_START || keycode > UIKC_END) {
        return true;
    }
    if(!record->event.pressed) {
        return true;
    }

    menu_t *self_menu = &self->menu;
    menu_item_t *item = &self_menu->items[self_menu->current_item];

    uint8_t resolution = 1;
    switch(keycode) {
        case UIKC_UP:
            if(self_menu->current_item > 0) {
                self_menu->current_item--;
            } else {
                self_menu->current_item = self_menu->num_items - 1;
            }
            self->need_redraw = true;
            break;

        case UIKC_DOWN:
            if(self_menu->current_item < self_menu->num_items - 1) {
                self_menu->current_item++;
            } else {
                self_menu->current_item = 0;
            }
            self->need_redraw = true;
            break;

        case UIKC_CLICK:
            switch(item->type) {
                case ITEM_TOGGLE:
                    *item->data_ptr = !(*item->data_ptr);
                    item->callback(*item->data_ptr);
                    self->need_redraw = true;
                    return false;

                case ITEM_ACTION:
                    if(item->action) item->action();
                    return false;

                case ITEM_SCREEN:
                    ui_open_screen(item->target_screen);
                    return false;

                default:
                    return true;
            }

        case UIKC_BACK:
            if(self->parent) {
                ui_open_screen(self->parent);
                return false;
            }
            return true;

        case UIKC_LEFT:
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

        case UIKC_RIGHT:
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

bool ui_event(uint16_t keycode, keyrecord_t *record) {
    if(current_screen->event) {
        return current_screen->event(current_screen, keycode, record);
    }
    return true;
}

void ui_render(void) {
    switch(current_screen->type) {
        case SCREEN_FREEDRAW:
            if(current_screen->draw) {
                current_screen->draw(current_screen);
            }
            break;

        case SCREEN_MENU:
            ui_menu_draw(current_screen);
            break;

        default:
            return;
    }
}

