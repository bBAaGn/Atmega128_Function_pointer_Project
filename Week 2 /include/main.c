#include "lcd.h"
#include "button.h"
#include "menu.h"

extern MenuItem m_main1;

int main(void) {
    lcd_init();
    button_init();

    menu_init(&m_main1);
    menu_render();

    while (1) {
        Button_t key = button_read();
        if (key != KEY_NONE) {
            menu_navigate(key);
        }
    }
}