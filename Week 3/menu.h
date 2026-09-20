#ifndef MENU_H_
#define MENU_H_

#include <avr/io.h>

// Cấu trúc một mục Menu
typedef struct MenuItem {
    char *title;                    // Tiêu đề mục menu
    struct MenuItem *parent;        // Mục cha
    struct MenuItem *child;         // Mục con đầu tiên
    struct MenuItem *next;          // Mục kế tiếp cùng cấp
    struct MenuItem *prev;          // Mục trước đó cùng cấp
    void (*action)(void);           // Con trỏ hàm thực thi (nếu có)
} MenuItem;

// Các hàm điều hướng & hiển thị
void menu_init(MenuItem *root);
void menu_navigate(uint8_t key_code);
void menu_render(void);

#endif