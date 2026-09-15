#include <stdint.h>

/* ================= ĐỊA CHỈ THANH GHI (MEMORY MAP) ================= */
#define RCC_BASE        0x40021000UL
#define GPIOA_BASE      0x40010800UL
#define GPIOC_BASE      0x40011000UL
#define TIM2_BASE       0x40000000UL

/* Con trỏ thanh ghi RCC */
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

/* Con trỏ thanh ghi GPIOA (Chân Data PA8 của DS18B20) */
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

/* Con trỏ thanh ghi GPIOC (LED báo trạng thái PC13) */
#define GPIOC_CRH       (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR       (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

/* Con trỏ thanh ghi Timer 2 */
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2C))
#define TIM2_CNT        (*(volatile uint32_t *)(TIM2_BASE + 0x24))

/* ================= 1. HÀM TẠO TRỄ MICROSECOND (TIMER 2) ================= */
void Delay_Timer_Init(void) {
    RCC_APB1ENR |= (1UL << 0); // Bật xung clock cho TIM2
    TIM2_PSC = 8 - 1;          // Xung nhịp mặc định 8MHz / 8 = 1MHz -> Mỗi tick = 1us
    TIM2_ARR = 0xFFFF;         // Giá trị tràn tối đa 16-bit
    TIM2_CR1 |= (1UL << 0);    // Bật Timer (CEN = 1)
}

void delay_us(uint16_t us) {
    TIM2_CNT = 0;
    while (TIM2_CNT < us);
}

void delay_ms(uint16_t ms) {
    while (ms--) {
        delay_us(1000);
    }
}

/* ================= 2. ĐIỀU KHIỂN CHÂN PA8 & LED PC13 ================= */
void Hardware_Init(void) {
    // Cấp clock cho GPIOA và GPIOC
    RCC_APB2ENR |= (1UL << 2) | (1UL << 4);

    // Cấu hình PC13 làm Output Push-Pull 2MHz (Chân LED)
    GPIOC_CRH &= ~(0xFUL << 20);
    GPIOC_CRH |=  (0x2UL << 20);
    GPIOC_ODR |=  (1UL << 13); // Mặc định tắt LED (LED trên board tích cực mức LOW)
}

// Chuyển PA8 thành Output Open-Drain (Vừa kéo 0 vừa thả nổi cho trở kéo lên 1)
void OneWire_Set_Output(void) {
    GPIOA_CRH &= ~(0xFUL << 0);
    GPIOA_CRH |=  (0x6UL << 0); // MODE = 10 (Output 2MHz), CNF = 01 (Open-Drain)
}

// Chuyển PA8 thành Input Floating (Đọc dữ liệu từ cảm biến)
void OneWire_Set_Input(void) {
    GPIOA_CRH &= ~(0xFUL << 0);
    GPIOA_CRH |=  (0x4UL << 0); // MODE = 00 (Input), CNF = 01 (Floating)
}

/* ================= 3. BẮT XUNG RESET & PRESENCE (TIMING P1.8) ================= */
uint8_t OneWire_Reset(void) {
    uint8_t presence = 0;

    // Bước 1: Kéo bus xuống 0 tối thiểu 480us
    OneWire_Set_Output();
    GPIOA_ODR &= ~(1UL << 8);
    delay_us(480);

    // Bước 2: Thả bus về 1 (Input), đợi 70-80us xem DS18B20 có kéo bus xuống 0 không
    OneWire_Set_Input();
    delay_us(80);

    // Bước 3: Đọc mức logic tại PA8
    if ((GPIOA_IDR & (1UL << 8)) == 0) {
        presence = 1; // Nhận diện thành công cảm biến!
    }

    // Bước 4: Đợi hết khoảng thời gian của chu kỳ Reset (tối thiểu 400us nữa)
    delay_us(400);

    return presence;
}

/* ================= 4. HÀM MAIN KIỂM TRA MÔ PHỎNG ================= */
int main(void) {
    Delay_Timer_Init();
    Hardware_Init();

    while (1) {
        // Liên tục kiểm tra sự hiện diện của DS18B20 mỗi 500ms
        if (OneWire_Reset()) {
            // Có cảm biến: LED PC13 bật sáng (Kéo chân PC13 xuống 0V)
            GPIOC_ODR &= ~(1UL << 13);
        } else {
            // Không có cảm biến: Tắt LED (Kéo chân PC13 lên 3.3V)
            GPIOC_ODR |= (1UL << 13);
        }

        delay_ms(500);
    }

    return 0;
}