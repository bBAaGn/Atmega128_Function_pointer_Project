#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "peripherals.h"
// Lưu ý: Đảm bảo thư viện LCD của bạn đã được bao gồm chính xác
#include "lcd.h" 

// Định nghĩa các phím bấm trên PORTD (Internal Pull-up)
#define BTN_UP      PD0
#define BTN_DOWN    PD1
#define BTN_ENTER   PD2
#define BTN_BACK    PD3

uint8_t relay_status = 0; // 0: OFF, 1: ON

void BUTTON_Init(void) {
    // Cấu hình PD0-PD3 là Input
    DDRD &= ~((1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_ENTER) | (1 << BTN_BACK));
    // Bật điện trở kéo lên nội (Pull-up)
    PORTD |= (1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_ENTER) | (1 << BTN_BACK);
}

void Update_Display(void) {
    LCD_Clear();
    LCD_SetCursor(0, 0);
    if (relay_status) {
        LCD_String("RELAY: ON");
    } else {
        LCD_String("RELAY: OFF");
    }
}

int main(void) {
    // Khởi tạo ngoại vi
    PERIPHERALS_Init();
    BUTTON_Init();
    LCD_Init();

    // Hiển thị ban đầu
    Update_Display();

    while (1) {
        // Kiểm tra phím ENTER (NútPD2 được bấm xuống GND)
        if (!(PIND & (1 << BTN_ENTER))) {
            _delay_ms(20); // Anti-bounce (chống dội phím)
            if (!(PIND & (1 << BTN_ENTER))) {
                relay_status = !relay_status; // Đảo trạng thái Relay
                
                // Cập nhật ngoại vi
                RELAY_SetState(relay_status);
                LED_SetState(relay_status);
                BUZZER_Beep(100); // Phát tiếng "tít" 100ms
                
                // Cập nhật LCD
                Update_Display();
                
                // Chờ nhả phím
                while (!(PIND & (1 << BTN_ENTER)));
            }
        }
        _delay_ms(10);
    }
    return 0;
}
