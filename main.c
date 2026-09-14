#include "lcd.h"
#include "button.h"

int main(void) {
    lcd_init();
    button_init();
    
    lcd_gotoxy(0, 0);
    lcd_puts("TUAN 1: TEST OK");
    lcd_gotoxy(0, 1);
    lcd_puts("PRESS BUTTON...");

    while (1) {
        Button_t key = button_read();
        
        if (key != KEY_NONE) {
            lcd_gotoxy(0, 1);
            lcd_puts("                "); // Xóa dòng 2
            lcd_gotoxy(0, 1);
            
            switch (key) {
                case KEY_UP:    lcd_puts("KEY: UP"); break;
                case KEY_DOWN:  lcd_puts("KEY: DOWN"); break;
                case KEY_ENTER: lcd_puts("KEY: ENTER"); break;
                case KEY_BACK:  lcd_puts("KEY: BACK"); break;
                default: break;
            }
        }
    }
}