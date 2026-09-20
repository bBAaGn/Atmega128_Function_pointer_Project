#include "button.h"

void button_init(void) {
    // Cấu hình PD0..PD3 là Input
    BTN_DDR &= ~((1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_ENTER) | (1 << BTN_BACK));
    // Bật điện trở kéo lên nội (Pull-up)
    BTN_PORT |= (1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_ENTER) | (1 << BTN_BACK);
}

Button_t button_read(void) {
    if (!(BTN_PIN & (1 << BTN_UP))) {
        _delay_ms(20); // Debounce chống rung
        if (!(BTN_PIN & (1 << BTN_UP))) {
            while (!(BTN_PIN & (1 << BTN_UP))); // Chờ nhả phím
            return KEY_UP;
        }
    }
    if (!(BTN_PIN & (1 << BTN_DOWN))) {
        _delay_ms(20);
        if (!(BTN_PIN & (1 << BTN_DOWN))) {
            while (!(BTN_PIN & (1 << BTN_DOWN)));
            return KEY_DOWN;
        }
    }
    if (!(BTN_PIN & (1 << BTN_ENTER))) {
        _delay_ms(20);
        if (!(BTN_PIN & (1 << BTN_ENTER))) {
            while (!(BTN_PIN & (1 << BTN_ENTER)));
            return KEY_ENTER;
        }
    }
    if (!(BTN_PIN & (1 << BTN_BACK))) {
        _delay_ms(20);
        if (!(BTN_PIN & (1 << BTN_BACK))) {
            while (!(BTN_PIN & (1 << BTN_BACK)));
            return KEY_BACK;
        }
    }
    return KEY_NONE;
}