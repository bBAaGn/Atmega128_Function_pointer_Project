#include "lcd.h"
#include "button.h"
#include "menu.h"
#include "peripherals.h"

extern MenuItem m_main1;

int main(void) {
    // Khởi tạo toàn bộ phần cứng
    lcd_init();
    button_init();
    peripherals_init();

    // Khởi tạo Menu Engine
    menu_init(&m_main1);
    menu_render();

    while (1) {
        Button_t key = button_read();
        if (key != KEY_NONE) {
            buzzer_beep();      // Phát tiếng beep ngắn phản hồi phím bấm
            menu_navigate(key); // Cập nhật trạng thái menu & thực thi action
        }
    }
}