#include <avr/io.h>
#include "userdef.h"
#define F_CPU 1000000UL // Thêm UL (Unsigned Long) cho tần số
#include <util/delay.h>

// Cấp phát bộ nhớ thực tế cho các biến extern
union RL Relays;
struct Motor Motor;
struct Button Buttons;

unsigned char Segcode[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E}; 

// --- KHỞI TẠO DỮ LIỆU MENU TĨNH ---
Menu MainMenu = {NULL, NULL, "Main Menu", "Sensors", &SensorMenu, NULL, "Actuators", &ActuatorMenu, NULL, "Settings", &SettingMenu};
Menu SensorMenu = {NULL, &MainMenu, "Sensors", "Temperature", &TempMenu, NULL, "Humidity", &HumidityMenu, NULL, " Date & Time ", NULL};
Menu TempMenu = {NULL, &SensorMenu, "Temperature", "ON", NULL, NULL, "OFF", NULL, NULL, "NONE", NULL};
Menu HumidityMenu = {NULL, &SensorMenu, "Humidity", "ON", NULL, NULL, "OFF", NULL, NULL, "NONE", NULL};
Menu ActuatorMenu = {NULL, &MainMenu, "Actuators", "Motors", &MotorsMenu, NULL, "Relays", &RelaysMenu, NULL, "LEDs", &LEDsMenu};
Menu MotorsMenu = {NULL, &ActuatorMenu, "Motors", "Motor 1", &Motor1Menu, NULL, "Motor 2", &Motor2Menu, NULL, "NONE", NULL};
Menu Motor1Menu = {Device_Motor1, &MotorsMenu, "Motor 1", "ON", NULL, NULL, "OFF", NULL, NULL, "NONE", NULL};
Menu Motor2Menu = {Device_Motor2, &MotorsMenu, "Motor 2", "ON", NULL, &ActuatorsActivation, "OFF", NULL, &ActuatorsActivation, "NONE", NULL};
Menu RelaysMenu = {Device_Relay, &ActuatorMenu, "Relays", "ON", NULL, &ActuatorsActivation, "OFF", NULL, &ActuatorsActivation, "NONE", NULL};
Menu LEDsMenu = {Device_LED, &ActuatorMenu, "LEDs", "ON", NULL, NULL, "OFF", NULL, NULL, "NONE", NULL};
Menu SettingMenu = {NULL, &MainMenu, "Settings", "Time", NULL, &ActuatorsActivation, "Speed", NULL, &ActuatorsActivation, "Channels", NULL};

// --- CÁC HÀM THỰC THI ---
void LCDInit()
{
    LCD_INS = 0x38; LCDE_H; _delay_us(1); LCDE_L; _delay_us(1); _delay_ms(2);
    LCD_INS = 0x0C; LCDE_H; _delay_us(1); LCDE_L; _delay_us(1); _delay_ms(2);
    LCD_INS = 0x01; LCDE_H; _delay_us(1); LCDE_L; _delay_us(1); _delay_ms(2);
    LCD_INS = 0x06; LCDE_H; _delay_us(1); LCDE_L; _delay_us(1); _delay_ms(20);
}

void ActuatorsActivation (char devices, char Status) 
{                                                      
    switch(devices)
    {
        case Device_Relay: 
            if(Status == ON) Relays.relayall = 0xFF; 
            else Relays.relayall = 0x00; 
            Relay_Activate;
            break;
        case Device_Motor1: 
            Motor.DC1 = Status; 
            Motor_Activate; 
            break;
        case Device_Motor2: 
            Motor.DC2 = Status; 
            Motor_Activate; 
            break;
        case Device_LED: 
            // Comment phần cứng LED để tránh lỗi biên dịch, Người B sẽ mở ra sau
            /* if(Status == ON) LED0 = 0xff; else LED0 = 0x00; */
            break;
    }
}

void printL (const char *str, unsigned line, unsigned col)
{
    unsigned char add;
    switch(line) {
        case 0: add=0x80; break;
        case 1: add=0xC0; break;
        case 2: add=0x94; break;
        case 3: add=0xD4; break;
        default: return; 
    }
    add += col; 
    LCD_INS = add; LCDE_H; _delay_us(1); LCDE_L; _delay_us(50);
    _delay_us(200); 

    while (*str != '\0')
    {
        LCD_DATA = *str++; LCDE_H; _delay_us(1); LCDE_L; _delay_us(10);
    }
}

void MenuDisplay(Menu *menu, unsigned char select)
{
    // Đã sửa lại đúng tên biến trong struct (Title, List1, List2, List3)
    printL(menu->Title, 0, 0); 
    
    // Thêm logic in con trỏ '>' để báo hiệu dòng đang được chọn
    printL(select == 1 ? "> " : "  ", 1, 0);
    printL(menu->List1, 1, 2); 
    
    printL(select == 2 ? "> " : "  ", 2, 0);
    printL(menu->List2, 2, 2); 
    
    printL(select == 3 ? "> " : "  ", 3, 0);
    printL(menu->List3, 3, 2); 
}

// --- HÀM MAIN ---
int main(void)
{
    unsigned char select = 1; 
    Menu *menu;
    
    // Đã sửa toán tử != thành toán tử gán =
    MCUCR = 0x80; 
    XMCRA = 0x00; 
    DDRG = 0x0F; 

    menu = &MainMenu; 
    LCDInit(); 
    MenuDisplay(menu, select); 

    while (1)
    {
        Readkey;
        
        if (!Buttons.UP) {
            while(!Buttons.UP) Readkey; 
            select = (select == 1) ? 3 : select - 1; 
            MenuDisplay(menu, select);
            _delay_ms(200); 
        }
        if (!Buttons.DOWN) {
            while(!Buttons.DOWN) Readkey; 
            select = (select == 3) ? 1 : select + 1; 
            MenuDisplay(menu, select);
            _delay_ms(200); 
        }
        if (!Buttons.LEFT) {
            while(!Buttons.LEFT) Readkey; 
            menu = (menu->pre == NULL) ? menu : menu->pre;
            select = 1; // Reset select về 1 khi back ra ngoài
            MenuDisplay(menu, select); // Đã thêm dòng vẽ lại menu
            _delay_ms(200); 
        }
        if (!Buttons.RIGHT) {
            while(!Buttons.RIGHT) Readkey; 
            switch(select)
            {
                case 1: menu = (menu->Menulist1 == NULL) ? menu : menu->Menulist1; break;
                case 2: menu = (menu->Menulist2 == NULL) ? menu : menu->Menulist2; break;
                case 3: menu = (menu->Menulist3 == NULL) ? menu : menu->Menulist3; break;
            }
            select = 1; // Reset select về 1 khi vào menu con
            MenuDisplay(menu, select); // Đã sửa tên hàm viết thường thành chữ hoa
            _delay_ms(200); 
        }
        if (!Buttons.OK) {
            while(!Buttons.OK) Readkey; 
            switch (select) {
                case 1:
                    if(menu->ActivationON != NULL) menu->ActivationON(menu->MenuID, ON);  
                    break;
                case 2:
                    if(menu->ActivationOFF != NULL) menu->ActivationOFF(menu->MenuID, OFF);
                    break;
            }
            MenuDisplay(menu, select);
            _delay_ms(200); 
        }
    }
    return 0;
}
