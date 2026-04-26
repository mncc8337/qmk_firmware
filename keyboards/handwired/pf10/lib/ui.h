#pragma once
#include QMK_KEYBOARD_H

typedef enum {
    ITEM_ACTION,
    ITEM_TOGGLE,
    ITEM_SLIDER,
    ITEM_LIST,
    ITEM_SCREEN,
} item_type_t;

typedef struct menu_item_t {
    item_type_t type;
    const char* label;

    union {
        void (*action)(void); // type ITEM_ACTION
        struct { // type ITEM_SLIDER or ITEM_TOGGLE or ITEM_LIST
            uint8_t *data_ptr;
            void (*callback)(uint8_t);
            union {
                struct { // type ITEM_SLIDER
                    uint8_t resolution;
                    bool show_value;
                };
                struct { // type ITEM_LIST
                    uint8_t list_count;
                    const char *(*name_getter)(uint8_t);
                };
            };
        };
        struct screen *target_screen; // type ITEM_SCREEN
    };
} menu_item_t;

typedef struct menu {
    const char *title;
    menu_item_t *items;
    uint8_t num_items;
    uint8_t current_item;
    uint8_t first_shown_item;
} menu_t;

typedef enum {
    SCREEN_MENU,
    SCREEN_FREEDRAW,
} screen_type_t;

typedef struct screen {
    screen_type_t type;
    bool need_redraw;
    bool (*event)(struct screen *self, int keycode);
    void (*enter_callback)(void);
    void (*exit_callback)(void);
    struct screen *parent;

    union {
        menu_t menu; // type SCREEN_MENU
        void (*draw)(struct screen *self); // type SCREEN_FREEDRAW
    };
} screen_t;
