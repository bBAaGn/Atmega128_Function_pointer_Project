#include "menu.h"
#include "lcd.h"
#include "button.h"
#include <stddef.h>

static MenuItem *current_item = NULL;

void menu_init(MenuItem *root) {
    current_item = root;
}

// Tìm mục đầu tiên trong cùng một cấp (đầu danh sách)
static MenuItem* get_first_sibling(MenuItem *item) {
    if (!item) return NULL;
    while (item->prev != NULL) {
        item = item->prev;
    }
    return item;
}

// Đếm vị trí chỉ số (index) của item hiện tại trong danh sách cùng cấp (bắt đầu từ 0)
static uint8_t get_item_index(MenuItem *item) {
    uint8_t index = 0;
    while (item->prev != NULL) {
        index++;
        item = item->prev;
    }
    return index;
}

void menu_render(void) {
    if (!current_item) return;

    // Tìm đầu danh sách cùng cấp
    MenuItem *first = get_first_sibling(current_item);
    uint8_t current_index = get_item_index(current_item);

    // Thuật toán Cửa sổ trượt (Scrolling Window) cho LCD 2 dòng
    uint8_t top_index = 0;
    if (current_index >= 1) {
        top_index = current_index - 1; // Giữ con trỏ luôn ở dòng 2 nếu vượt quá 1
    }

    // Tìm item ứng với dòng trên cùng (Line 0 của LCD)
    MenuItem *top_item = first;
    for (uint8_t i = 0; i < top_index && top_item != NULL; i++) {
        top_item = top_item->next;
    }

    lcd_clear();

    // Dòng 1 (Line 0)
    if (top_item != NULL) {
        lcd_gotoxy(0, 0);
        if (top_item == current_item) lcd_puts(">"); else lcd_puts(" ");
        lcd_puts(top_item->title);
    }

    // Dòng 2 (Line 1)
    if (top_item->next != NULL) {
        lcd_gotoxy(0, 1);
        if (top_item->next == current_item) lcd_puts(">"); else lcd_puts(" ");
        lcd_puts(top_item->next->title);
    }
}

void menu_navigate(uint8_t key) {
    switch (key) {
        case KEY_UP:
            if (current_item->prev != NULL) {
                current_item = current_item->prev;
                menu_render();
            }
            break;

        case KEY_DOWN:
            if (current_item->next != NULL) {
                current_item = current_item->next;
                menu_render();
            }
            break;

        case KEY_ENTER:
            if (current_item->child != NULL) {
                current_item = current_item->child; // Vào menu con
                menu_render();
            } else if (current_item->action != NULL) {
                current_item->action(); // Chạy hàm tác vụ nếu có
            }
            break;

        case KEY_BACK:
            if (current_item->parent != NULL) {
                current_item = current_item->parent; // Trở về menu cha
                menu_render();
            }
            break;
    }
}