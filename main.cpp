#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

/* ================= 1. GIAO TIẾP 1-WIRE (CHÂN PD4) ================= */
#define ONEWIRE_DDR     DDRD
#define ONEWIRE_PORT    PORTD
#define ONEWIRE_PIN     PIND
#define ONEWIRE_BIT     PD4

void OneWire_Low(void) {
    ONEWIRE_DDR |= (1 << ONEWIRE_BIT);
    ONEWIRE_PORT &= ~(1 << ONEWIRE_BIT);
}

void OneWire_Release(void) {
    ONEWIRE_DDR &= ~(1 << ONEWIRE_BIT);
    ONEWIRE_PORT &= ~(1 << ONEWIRE_BIT);
}

uint8_t OneWire_Reset(void) {
    uint8_t presence = 0;
    OneWire_Low();
    _delay_us(480);
    OneWire_Release();
    _delay_us(80);
    if (!(ONEWIRE_PIN & (1 << ONEWIRE_BIT))) {
        presence = 1;
    }
    _delay_us(400);
    return presence;
}

void OneWire_WriteBit(uint8_t bit) {
    if (bit) {
        OneWire_Low();
        _delay_us(2);
        OneWire_Release();
        _delay_us(60);
    } else {
        OneWire_Low();
        _delay_us(60);
        OneWire_Release();
        _delay_us(2);
    }
}

uint8_t OneWire_ReadBit(void) {
    uint8_t bit = 0;
    OneWire_Low();
    _delay_us(2);
    OneWire_Release();
    _delay_us(10);
    if (ONEWIRE_PIN & (1 << ONEWIRE_BIT)) {
        bit = 1;
    }
    _delay_us(50);
    return bit;
}

void OneWire_WriteByte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        OneWire_WriteBit(byte & (1 << i));
    }
}

uint8_t OneWire_ReadByte(void) {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (OneWire_ReadBit()) {
            byte |= (1 << i);
        }
    }
    return byte;
}

/* ================= 2. GIAO TIẾP UART0 (PE1 - TX) ================= */
void UART0_Init(void) {
    UBRR0H = 0;
    UBRR0L = 51; // Baud 9600 tại f = 8MHz
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit, 1 stop bit
}

void UART0_SendChar(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void UART0_SendString(const char *str) {
    while (*str) {
        UART0_SendChar(*str++);
    }
}

/* ================= 3. ĐỌC MÃ ROM VÀ NHIỆT ĐỘ DS18B20 ================= */
uint8_t DS18B20_ReadROM(uint8_t *rom_buffer) {
    if (!OneWire_Reset()) return 0;
    OneWire_WriteByte(0x33); // Lệnh Read ROM
    for (uint8_t i = 0; i < 8; i++) {
        rom_buffer[i] = OneWire_ReadByte();
    }
    return 1;
}

float DS18B20_ReadTemp(void) {
    uint8_t lsb, msb;
    int16_t raw_temp;

    // Phát lệnh chuyển đổi nhiệt độ (Convert T)
    if (!OneWire_Reset()) return -999.0f;
    OneWire_WriteByte(0xCC); // Skip ROM
    OneWire_WriteByte(0x44); // Convert T
    _delay_ms(750);          // Chờ cảm biến tính toán

    // Đọc bộ nhớ Scratchpad
    if (!OneWire_Reset()) return -999.0f;
    OneWire_WriteByte(0xCC); // Skip ROM
    OneWire_WriteByte(0xBE); // Read Scratchpad

    lsb = OneWire_ReadByte();
    msb = OneWire_ReadByte();

    raw_temp = (int16_t)((msb << 8) | lsb);
    return (float)raw_temp * 0.0625f;
}

/* ================= 4. KHỞI TẠO NÚT BẤM VÀ ĐÈN LED ================= */
void IO_Init(void) {
    // Port A: Output điều khiển 8 LED báo động
    DDRA = 0xFF;
    PORTA = 0x00;

    // Chân PD0 (Nút Tăng), PD1 (Nút Giảm): Cấu hình Input và bật trở kéo nội
    DDRD &= ~((1 << PD0) | (1 << PD1));
    PORTD |= (1 << PD0) | (1 << PD1);
}

/* ================= 5. CHƯƠNG TRÌNH CHÍNH ================= */
int main(void) {
    char buffer[64];
    uint8_t rom[8];
    int8_t temp_threshold = 30; // Ngưỡng nhiệt độ mặc định ban đầu (30 độ C)

    IO_Init();
    UART0_Init();

    UART0_SendString("\r\n========================================\r\n");
    UART0_SendString(" HE THONG CANH BAO NHIET DO - TUAN 3\r\n");
    UART0_SendString("========================================\r\n");

    // Đọc và in mã ROM 64-bit định danh cảm biến
    if (DS18B20_ReadROM(rom)) {
        sprintf(buffer, "DS18B20 ROM ID: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                rom[0], rom[1], rom[2], rom[3], rom[4], rom[5], rom[6], rom[7]);
        UART0_SendString(buffer);
        if (rom[0] == 0x28) {
            UART0_SendString("Kiem tra: Cam bien DS18B20 hop le (Family Code: 0x28)\r\n\r\n");
        }
    } else {
        UART0_SendString("Loi: Khong the doc ma ROM tu bus 1-Wire!\r\n\r\n");
    }

    while (1) {
        // 1. Quét phím bấm tăng ngưỡng (PD0 - chân 25)
        if (!(PIND & (1 << PD0))) {
            _delay_ms(20); // Chống rung phím
            if (!(PIND & (1 << PD0))) {
                temp_threshold++;
                sprintf(buffer, ">> NGUONG DA TANG LEN: %d C <<\r\n", temp_threshold);
                UART0_SendString(buffer);
                while (!(PIND & (1 << PD0))); // Chờ nhả phím
            }
        }

        // 2. Quét phím bấm giảm ngưỡng (PD1 - chân 26)
        if (!(PIND & (1 << PD1))) {
            _delay_ms(20); // Chống rung phím
            if (!(PIND & (1 << PD1))) {
                temp_threshold--;
                sprintf(buffer, ">> NGUONG DA GIAM XUONG: %d C <<\r\n", temp_threshold);
                UART0_SendString(buffer);
                while (!(PIND & (1 << PD1))); // Chờ nhả phím
            }
        }

        // 3. Đọc nhiệt độ thực tế từ cảm biến
        float temp = DS18B20_ReadTemp();

        if (temp == -999.0f) {
            UART0_SendString("Loi mat ket noi voi DS18B20!\r\n");
            PORTA = 0xAA; // Chớp xen kẽ báo lỗi đường truyền
        } else {
            int int_part = (int)temp;
            int frac_part = (int)((temp - int_part) * 100);
            if (frac_part < 0) frac_part = -frac_part;

            // 4. So sánh với ngưỡng để kích hoạt báo động qua 8 LED
            if (temp >= temp_threshold) {
                PORTA = 0xFF; // BẬT SÁNG TOÀN BỘ 8 LED (Báo động)
                sprintf(buffer, "[CANH BAO QUAT NHIET] Nhiet do: %d.%02d C | Nguong: %d C\r\n",
                        int_part, frac_part, temp_threshold);
            } else {
                PORTA = 0x00; // TẮT HẾT LED (Nhiệt độ an toàn)
                sprintf(buffer, "[BINH THUONG] Nhiet do: %d.%02d C | Nguong: %d C\r\n",
                        int_part, frac_part, temp_threshold);
            }
            UART0_SendString(buffer);
        }

        _delay_ms(300);
    }

    return 0;
}