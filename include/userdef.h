#ifndef USERDEF_H
#define USERDEF_H

#include <avr/io.h>
#include <stddef.h>

#define EXMEM_ADD 0x1100
#define CS11 11
#define CS12 12

#define LCDE_H (PORTG |= (1<<3))
#define LCDE_L (PORTG &= ~(1<<3))

#define LCD_DATA *(volatile unsigned char *) (EXMEM_ADD+CS11)
#define LCD_INS  *(volatile unsigned char *) (EXMEM_ADD+CS12)

// Khai báo cấu trúc phần cứng (Đã bổ sung dấu chấm phẩy)
struct bits {
    unsigned char RL0 : 1;
    unsigned char RL1 : 1;
    unsigned char RL2 : 1;
    unsigned char RL3 : 1;
    unsigned char RL4 : 1;
    unsigned char RL5 : 1;
    unsigned char RL6 : 1;
    unsigned char RL7 : 1;
}; 

union RL {
    unsigned char relayall;
    struct bits relaybits;
}; 
extern union RL Relays;

struct Motor {
    unsigned char DC1 : 2; 
    unsigned char DC2 : 2; 
    unsigned char DCFree : 4; 
}; 
extern struct Motor Motor;

struct Button {
    unsigned char UP : 1;
    unsigned char DOWN : 1;
    unsigned char LEFT : 1;
    unsigned char RIGHT : 1;
    unsigned char OK : 1;
    unsigned char BT1 : 1;
    unsigned char BT2 : 1;
    unsigned char BT3 : 1;
};
extern struct Button Buttons;

// Macro đọc phím & thiết bị (Đã chuẩn hóa con trỏ volatile)
#define Readkey (*(unsigned char *) (&Buttons) = *(volatile unsigned char *) (EXMEM_ADD+14))
#define Relay_Activate {} // Tạm đóng gói macro phần cứng để test Menu
#define Motor_Activate {} // Tạm đóng gói macro phần cứng để test Menu

#define ON 1
#define OFF 0
#define Device_Relay 0
#define Device_Motor1 1
#define Device_Motor2 2
#define Device_LED 3

// Cấu trúc Menu
typedef struct Linker {
    char MenuID; 
    struct Linker *pre;
    char Title[20];
    char List1[20];
    struct Linker *Menulist1; 
    void (*ActivationON)(char, char); 
    char List2[20];
    struct Linker *Menulist2; 
    void (*ActivationOFF)(char, char);
    char List3[20];
    struct Linker *Menulist3;
} Menu; 

extern Menu MainMenu, SensorMenu, ActuatorMenu, SettingMenu, TempMenu, HumidityMenu, MotorsMenu, Motor1Menu, Motor2Menu, RelaysMenu, LEDsMenu;

// Nguyên mẫu hàm
void ActuatorsActivation (char devices, char Status);
void LCDInit(void);
void printL(const char *str, unsigned line, unsigned col);
void MenuDisplay(Menu *menu, unsigned char select);

#endif
