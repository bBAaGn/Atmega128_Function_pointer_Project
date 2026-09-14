//Dán nội dung khai báo struct MenuItem và con trỏ hàm vào file này (file README mặc định có thể để nguyên hoặc xóa đi đều được).
//Định nghĩa struct and function pointer 
//Code chính nạp vào Atmega128
//Tổng hợp ND con trỏ hàm: P1.7: 37:23
#include <avr/io.h>
#include <stddef.h>
#define EXMEM_ADD 0x1100
/*
#define porta *(unsigned char *)0x3B
#define EXMEM *(unsigned short *)(EXMEM_ADD+0)
*/
#define CS11 11
#define CS12 12

#define LCDE_H (PORTG |= (1<<3) )
#define LCDE_L (PORTG &= ~(1<<3))
/*
#define LED0 *(unsigned char *) (EXMEM_ADD+CS0)
#define LED1 *(unsigned char *) (EXMEM_ADD+CS1)
#define LED2 *(unsigned char *) (EXMEM_ADD+CS2)
#define LED3 *(unsigned char *) (EXMEM_ADD+CS3)
*/
#define LCD_DATA *(unsigned char *) (EXMEM_ADD+CS11)
#define LCD_INS (*(unsigned char )*) (EXMEM_ADD+CS12)
#define Relay_Activate (*(volatile unsigned char *) (EXMEM_ADD+CS10) = *(unsigned char *) (&RelayS))
#define Readkey (*(unsigned char *) (&Buttons) = *(unsigned char *) (EXMEM_ADD+CS14) )

struct bits {
unsigned char RL0 : 1;
      unsigned char RL1 : 1;
      unsigned char RL2 : 1;
      unsigned char RL3 : 1;
      unsigned char RL4 : 1;
      unsigned char RL5 : 1;
      unsigned char RL6 : 1;
      unsigned char RL7 : 1;
}

#define STOP 0
#define FORWARD 1
#define REVERSE 2

union RL{
    unsigned char relayall;
    struct bits relaybits;
}
union RL Relays;

struct Motor {
    
    unsigned char DC1 : 2; // 00: dừng, 01: tiến, 10: lùi
    unsigned char DC2 : 2; // 00: dừng, 01: tiến, 10: lùi
    unsigned char DCFree : 4; // Dành cho các bit còn lại
}
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

//Quy ước, định nghĩa các trạng thái ON/OFF cho các thiết bị được điều khiển trong hệ thống. ON = 1, OFF = 0
#define ON 1
#define OFF 0

//Tham số ngõ vào các thiết bị được điều khiển trong hệ thống, có thể mở rộng thêm nếu cần thiết.
#define Device_Relay 0
#define Device_Motor1 1
#define Device_Motor2 2
#define Device_LED 3

//Khai báo struct MenuItem và con trỏ hàm
//Linker: con trỏ liên kết của 1 con trỏ cấu trúc
typedef struct Linker {
    //Tìm hiểu thêm về cách dùng MenuID để xác định menu, có thể dùng để xác định menu hiện tại hoặc menu cha của menu hiện tại.
    //Video p1.7: 27:54
    char MenuID; //ID của menu, có thể dùng để xác định menu
    struct Linker *pre;
    char Title[20];
    char List1[20];
    //Định nghĩa con trỏ hàm ActivationON và ActivationOFF để trỏ đến các hàm xử lý khi nhấn nút OK trong menu. Các con trỏ này sẽ được gán giá trị tương ứng với các hàm xử lý cụ thể cho từng mục trong menu.
    //Nhận input là địa chỉ của thiết bị và trạng thái ON/OFF, và thực hiện các hành động tương ứng để điều khiển thiết bị đó.
    struct Linker *Menulist1; void (*ActivationON)(char, char); //cách trỏ hàm của con trỏ
    char List2[20];
    struct Linker *Menulist2; void (*ActivationOFF)(char, char);
    char List3[20];
    struct Linker *Menulist3;
} Menu; //Dùng typedef struct Linker Menu; để định nghĩa kiểu dữ liệu Menu, giúp dễ dàng sử dụng trong các phần khác của chương trình.
extern Menu MainMenu, SensorMenu, ActuatorMenu, SettingMenu, TempMenu, HumidityMenu, MotorsMenu, Motor1Menu, Motor2Menu, RelaysMenu, LEDsMenu;

Menu MainMenu = {NULL, // ID của menu chính là NULL vì nó là menu gốc
    NULL, // Menu cha của menu chính là NULL vì nó là menu gốc 
    "Main Menu",
    "Sensors", &SensorMenu, NULL,
    "Actuators", &ActuatorMenu, NULL,
    "Settings", &SettingMenu};

Menu SensorMenu = {NULL,
    &MainMenu, "Sensors", 
    "Temperature", &TempMenu, NULL,
    "Humidity", &HumidityMenu, NULL,
    " Date & Time ", NULL};
Menu TempMenu = {NULL,
    &SensorMenu, "Temperature", 
    "ON", NULL, NULL,   
    "OFF", NULL, NULL,
    "NONE", NULL};
Menu HumidityMenu = {NULL,
    &SensorMenu, "Humidity", 
    "ON", NULL, NULL,   
    "OFF", NULL, NULL,
    "NONE", NULL};

Menu ActuatorMenu = {NULL,
    &MainMenu, "Actuators", 
    "Motors", &MotorsMenu, NULL,
    "Relays", &RelaysMenu, NULL,
    "LEDs", &LEDsMenu};
Menu MotorsMenu = {NULL,
    &ActuatorMenu, "Motors", 
    "Motor 1", &Motor1Menu, NULL,
    "Motor 2", &Motor2Menu, NULL,
    "NONE", NULL};
Menu Motor1Menu = {Device_Motor1,
    &MotorsMenu, "Motor 1", 
    "ON", NULL, NULL,
    "OFF", NULL, NULL,
    "NONE", NULL};
Menu Motor2Menu = {Device_Motor2,
    &MotorsMenu, "Motor 2", 
    "ON", NULL, &ActuatorsActivation,
    "OFF", NULL, &ActuatorsActivation,
    "NONE", NULL};
    
Menu RelaysMenu = {Device_Relay,
    &ActuatorMenu, "Relays", 
    "ON", NULL, &ActuatorsActivation, //Địa chỉ của hàm ActuatorsActivation được gán cho con trỏ hàm ActivationON để xử lý khi nhấn nút OK trong menu Relays.
    "OFF", NULL, &ActuatorsActivation,
    "NONE", NULL};
Menu LEDsMenu = {Device_LED,
    &ActuatorMenu, "LEDs", 
    "ON", NULL, NULL,
    "OFF", NULL, NULL,
    "NONE", NULL};
Menu SettingMenu = {NULL, 
    &MainMenu, "Settings", 
    "Time", NULL, &ActuatorsActivation,
    "Speed", NULL, &ActuatorsActivation,
    "Channels", NULL};

// Đặt hàm con trỏ hàm để điều khiển các thiết bị
// Có thể định nghĩa ngắn hơn hoặc dài hơn tùy theo nhu cầu, nhưng phải đảm bảo rằng các thiết bị được điều khiển đúng cách.                         
//Hoạt động dựa vào 2 biến devices và Status
void ActuatorsActivation (char devices, char Status);
void LCDInit(void);
void PrintL(const char *str, unsigned line, unsigned col);

