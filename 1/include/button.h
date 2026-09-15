#ifndef BUTTON_H_
#define BUTTON_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>

#define BTN_PORT PORTD
#define BTN_PIN  PIND
#define BTN_DDR  DDRD

#define BTN_UP    PD0
#define BTN_DOWN  PD1
#define BTN_ENTER PD2
#define BTN_BACK  PD3

typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_ENTER,
    KEY_BACK
} Button_t;

void button_init(void);
Button_t button_read(void);

#endif