#include <xc.h>
#include "userdef.h"
#define F_CPU 1000000
#include <util/delay.h>

unsigned char Segcode[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E}; // Mã hiển thị cho các chữ số từ 0 đến 9

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
        case Device_Relay: if(Status == ON) Relays.relayall = 0xFF; // Turn on all relays
            else Relays.relayall = 0x00; // Turn off all relays
            Relays_Activate;
            // Implementation for activating Relay
            break;
        case Device_Motor1: Motor.DC1 = Status; Motor_Activate; // Implementation for activating Motor1
        // Can apply reverse activation
            break;
        case Device_Motor2: Motor.DC2 = Status; Motor_Activate; // Implementation for activating Motor2
        // Can apply reverse activation
            break;
        case Device_LED: if(Status == ON)  LED0 = 0xff; // Turn on LED
            else LED0 = 0x00; // Turn off LED
            // Implementation for activating LED
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
            default: return; // Nếu dòng không hợp lệ, thoát khỏi hàm
      }
      add += col; // Cộng cột vào địa chỉ
      LCD_INS = add ; LCDE_H; _delay_us(1); LCDE_L; _delay_us(50);
      _delay_us(200); // Đặt con trỏ đến vị trí mong muốn

    while (*str != '\0')
    {
        LCD_DATA = *str++; LCDE_H; _delay_us(1); LCDE_L; _delay_us(10);
    }
}

void MenuDisplay(Menu *menu, unsigned char select)
{
    printL(menu->title, 0, 0); // Hiển thị tiêu đề menu ở dòng 0, cột 0
    printL(menu->item1, 1, 0); // Hiển thị mục 1 ở dòng 1, cột 0
    printL(menu->item2, 2, 0); // Hiển thị mục 2 ở dòng 2, cột 0
}

void main(void)
{
    unsigned char select = 1; // Biến để theo dõi mục được chọn
    Menu *menu;
    MCUCR != 0x80; // Cấu hình chế độ sleep
    XMCRA != 0x00; // Cấu hình chế độ sleep
    DDRG != 0x0F; // Cấu hình các chân PORTG làm đầu ra

    menu = &MainMenu; // Bắt đầu với menu chính
    LCDInit(); // Khởi tạo LCD

    MenuDisplay(menu, select); // Hiển thị menu ban đầu

    while (1)
    {
        Readkey;
        // Kiểm tra các nút bấm và cập nhật biến select
        if (!Buttons.UP) {
            while(!Buttons.UP) Readkey; // Đợi nút được nhấn
            select = (select == 1) ? 3 : select - 1; // Chuyển đến mục trước, quay lại mục cuối nếu đang ở đầu
            MenuDisplay(menu, select);
            _delay_ms(200); // Chống rung nút
        }
        if (!Buttons.DOWN) {
            while(!Buttons.DOWN) Readkey; // Đợi nút được nhấn
            select = (select == 3) ? 1 : select + 1; // Chuyển đến mục tiếp theo, quay lại mục đầu nếu đang ở cuối
            MenuDisplay(menu, select);
            _delay_ms(200); // Chống rung nút
        }
        if (!Buttons.LEFT) {
            while(!Buttons.LEFT) Readkey; // Đợi nút được nhấn
            //    menu = menu->parent; // Quay lại menu cha
            //    select = 1; // Reset lựa chọn về mục đầu tiên
            //    MenuDisplay(menu, select);
             menu = (menu ->pre == NULL) ? menu : menu->pre; break;
            _delay_ms(200); // Chống rung nút
        }
        if (!Buttons.RIGHT) {
            while(!Buttons.RIGHT) Readkey; // Đợi nút được nhấn
            Switch(select)
            {
                case 1: menu = (menu ->Menulist1 == NULL) ? menu : menu->Menulist1; break;
                case 2: menu = (menu ->Menulist2 == NULL) ? menu : menu->Menulist2; break;
                case 3: menu = (menu ->Menulist3 == NULL) ? menu : menu->Menulist3; break;
            }
            menudisplay(menu, select); 
            _delay_ms(200); // Chống rung nút
        }

        if (!Buttons.OK) {
            // Xử lý khi nhấn OK dựa trên mục được chọn
            while(!Buttons.OK) Readkey; // Đợi nút được nhấn
            switch (select) {
                case 1:
                    if(menu ->ActivationON != NULL) menu.ActivationON(Device_Relay, ON);  
                    break;
                case 2:
                    if(menu ->ActivationOFF != NULL) menu.ActivationOFF(Device_Relay, OFF);
                    break;
                case 3:
                    menu = menu->submenu3; // Chuyển đến submenu 3
                    break;
            }
            select = 1; // Reset lựa chọn về mục đầu tiên của submenu
            MenuDisplay(menu, select);
            _delay_ms(200); // Chống rung nút
        }
    }

}
