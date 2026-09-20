#include "peripherals.h"

void peripherals_init(void) {
    // Cau hinh cac chan PB0, PB1, PB2 la Output
    DDRB |= (1 << RELAY_PIN) | (1 << LED_PIN) | (1 << BUZZER_PIN);
    PORTB &= ~((1 << RELAY_PIN) | (1 << LED_PIN) | (1 << BUZZER_PIN)); // Tat toan bo thiet bi ban dau

    motor_init();
}

void relay_on(void) {
    PORTB |= (1 << RELAY_PIN) | (1 << LED_PIN); // Bat Relay va LED bao trang thai
}

void relay_off(void) {
    PORTB &= ~((1 << RELAY_PIN) | (1 << LED_PIN)); // Tat Relay va LED
}

void relay_toggle(void) {
    PORTB ^= (1 << RELAY_PIN) | (1 << LED_PIN);
}

void buzzer_beep(void) {
    PORTB |= (1 << BUZZER_PIN);
    _delay_ms(50);
    PORTB &= ~(1 << BUZZER_PIN);
}

void motor_init(void) {
    DDRB |= (1 << MOTOR_PIN);

    // Timer1, Fast PWM 8-bit (WGM10=1, WGM12=1), non-inverting tren OC1A
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << CS11); // prescaler = 8 -> tan so PWM ~7.8kHz

    OCR1A = 0; // Ban dau toc do = 0
}

void motor_set_speed(uint8_t percent) {
    if (percent > 100) percent = 100;
    OCR1A = (uint16_t)((uint32_t)percent * 255 / 100);
}