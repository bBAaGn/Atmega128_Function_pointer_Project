#include "menu.h"
#include "peripherals.h"
#include "lcd.h"
#include "button.h"
#include <stddef.h>
#include <stdlib.h> // cho ham itoa()

// Khai bao truoc cac bien menu (dinh nghia day du o cuoi file)
extern MenuItem m_main1, m_main2, m_main3;
extern MenuItem m_sub1, m_sub2, m_sub3, m_sub4;

// Bien nho toc do dong co giua cac lan vao lai man hinh "Doi Toc Do"
static uint8_t g_motor_speed = 0;

// ===================== Cac ham callback tac vu =====================

void action_relay_on(void) {
    relay_on();
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("RELAY: ON");
    _delay_ms(1000);
}

void action_relay_off(void) {
    relay_off();
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("RELAY: OFF");
    _delay_ms(1000);
}

void action_relay_toggle(void) {
    relay_toggle();
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("RELAY: TOGGLE");
    _delay_ms(1000);
}

// "3. Thong Tin" - hien thi trang thai hien tai cua relay
void action_show_info(void) {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Trang thai Relay:");
    lcd_gotoxy(0, 1);
    if (PORTB & (1 << RELAY_PIN)) {
        lcd_puts("Dang: BAT");
    } else {
        lcd_puts("Dang: TAT");
    }
    _delay_ms(1500);
}

// "2. Che Do Auto" - placeholder, thay bang logic auto that khi co yeu cau cu the
void action_auto_placeholder(void) {
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Auto Mode:");
    lcd_gotoxy(0, 1);
    lcd_puts("Dang phat trien");
    _delay_ms(1500);
}

// "1.4 Doi Toc Do" - man hinh dieu chinh PWM dong co, tu quan ly vong lap doc phim rieng
void action_change_speed(void) {
    char buf[8];
    Button_t key;
    uint8_t running = 1;

    motor_set_speed(g_motor_speed);

    while (running) {
        lcd_clear();
        lcd_gotoxy(0, 0);
        lcd_puts("Toc do dong co:");

        lcd_gotoxy(0, 1);
        itoa(g_motor_speed, buf, 10);
        lcd_puts(buf);
        lcd_puts(" %  (BACK=Thoat)");

        // Cho co phim nhan
        do {
            key = button_read();
        } while (key == KEY_NONE);

        buzzer_beep();

        switch (key) {
            case KEY_UP:
                if (g_motor_speed <= 90) g_motor_speed += 10;
                motor_set_speed(g_motor_speed);
                break;
            case KEY_DOWN:
                if (g_motor_speed >= 10) g_motor_speed -= 10;
                motor_set_speed(g_motor_speed);
                break;
            case KEY_BACK:
                running = 0;
                break;
            default:
                break;
        }
    }
}

// ===================== Khai bao cau truc Cay Menu =====================

// Menu chinh
MenuItem m_main1 = {"1. Dieu Khien",   NULL, &m_sub1, &m_main2, NULL,     NULL};
MenuItem m_main2 = {"2. Che Do Auto",  NULL, NULL,    &m_main3, &m_main1, action_auto_placeholder};
MenuItem m_main3 = {"3. Thong Tin",    NULL, NULL,    NULL,     &m_main2, action_show_info};

// Menu con cua "1. Dieu Khien"
MenuItem m_sub1 = {"1.1 Bat Relay",   &m_main1, NULL, &m_sub2, NULL,     action_relay_on};
MenuItem m_sub2 = {"1.2 Tat Relay",   &m_main1, NULL, &m_sub3, &m_sub1,  action_relay_off};
MenuItem m_sub3 = {"1.3 Dao T.Thai",  &m_main1, NULL, &m_sub4, &m_sub2,  action_relay_toggle};
MenuItem m_sub4 = {"1.4 Doi Toc Do",  &m_main1, NULL, NULL,    &m_sub3,  action_change_speed};