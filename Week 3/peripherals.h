#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define RELAY_PIN   PB0
#define LED_PIN     PB1
#define BUZZER_PIN  PB2
#define MOTOR_PIN   PB5   // OC1A - chan PWM dieu khien toc do dong co

void peripherals_init(void);
void relay_on(void);
void relay_off(void);
void relay_toggle(void);
void buzzer_beep(void);

void motor_init(void);
void motor_set_speed(uint8_t percent); // 0 - 100 (%)

#endif