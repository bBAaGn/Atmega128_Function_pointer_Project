#include "lcd.h"

static void lcd_enable_pulse(void) {
    LCD_PORT |= (1 << LCD_EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(50);
}

void lcd_send_command(unsigned char cmd) {
    LCD_PORT &= ~(1 << LCD_RS); // RS = 0 (Gửi lệnh)
    
    // Gửi 4-bit cao
    LCD_PORT = (LCD_PORT & 0x0F) | (cmd & 0xF0);
    lcd_enable_pulse();
    
    // Gửi 4-bit thấp
    LCD_PORT = (LCD_PORT & 0x0F) | ((cmd << 4) & 0xF0);
    lcd_enable_pulse();
    
    _delay_ms(2);
}

void lcd_send_data(unsigned char data) {
    LCD_PORT |= (1 << LCD_RS); // RS = 1 (Gửi dữ liệu)
    
    // Gửi 4-bit cao
    LCD_PORT = (LCD_PORT & 0x0F) | (data & 0xF0);
    lcd_enable_pulse();
    
    // Gửi 4-bit thấp
    LCD_PORT = (LCD_PORT & 0x0F) | ((data << 4) & 0xF0);
    lcd_enable_pulse();
    
    _delay_us(50);
}

void lcd_init(void) {
    LCD_DDR |= 0xF3; // PA0, PA1, PA4-PA7 là Output
    _delay_ms(20);
    
    // Khởi tạo chế độ 4-bit
    LCD_PORT &= ~(1 << LCD_RS);
    LCD_PORT = (LCD_PORT & 0x0F) | 0x20;
    lcd_enable_pulse();
    
    lcd_send_command(0x28); // 2 dòng, font 5x8, mode 4-bit
    lcd_send_command(0x0C); // Bật màn hình, tắt con trỏ
    lcd_send_command(0x06); // Tự động tăng con trỏ
    lcd_clear();
}

void lcd_gotoxy(unsigned char x, unsigned char y) {
    unsigned char firstCharAdr[] = {0x80, 0xC0};
    lcd_send_command(firstCharAdr[y] + x);
}

void lcd_puts(char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

void lcd_clear(void) {
    lcd_send_command(0x01);
    _delay_ms(2);
}