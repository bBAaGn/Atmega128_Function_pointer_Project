#ifndef LCD_H_
#define LCD_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>

// Định nghĩa Cổng và Chân kết nối LCD (PORTA)
#define LCD_PORT PORTA
#define LCD_DDR  DDRA

#define LCD_RS PA0
#define LCD_EN PA1

void lcd_init(void);
void lcd_send_command(unsigned char cmd);
void lcd_send_data(unsigned char data);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_puts(char *str);
void lcd_clear(void);

#endif
