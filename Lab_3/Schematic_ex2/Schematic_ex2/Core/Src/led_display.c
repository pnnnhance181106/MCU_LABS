/*
 * led_display.c
 *
 *  Created on: Oct 18, 2026
 *      Author: nk
 */
#include "main.h"
#include "led_display.h"

#define LED_ON  GPIO_PIN_RESET
#define LED_OFF GPIO_PIN_SET

// Bộ đệm lưu 4 chữ số của 4 LED 7 đoạn
static int led_buffer[4] = {0, 0, 0, 0};
static int index_led = 0;

// Mã 7 đoạn Anode chung (0: Sáng, 1: Tắt) cho các số từ 0 đến 9
static const uint8_t sevenSegTable[10] = {
    0x40, // 0: 0100 0000
    0x79, // 1: 0111 1001
    0x24, // 2: 0010 0100
    0x30, // 3: 0011 0000
    0x19, // 4: 0001 1001
    0x12, // 5: 0001 0010
    0x02, // 6: 0000 0010
    0x78, // 7: 0111 1000
    0x00, // 8: 0000 0000
    0x10  // 9: 0001 0000
};

static void display7SEG(int num) {
    if (num < 0 || num > 9) return;
    uint8_t code = sevenSegTable[num];
    HAL_GPIO_WritePin(SEG0_GPIO_Port, SEG0_Pin, (code & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG1_GPIO_Port, SEG1_Pin, (code & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG2_GPIO_Port, SEG2_Pin, (code & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG3_GPIO_Port, SEG3_Pin, (code & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG4_GPIO_Port, SEG4_Pin, (code & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG5_GPIO_Port, SEG5_Pin, (code & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG6_GPIO_Port, SEG6_Pin, (code & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Tách số có 2 chữ số (00 - 99) vào led_buffer[4]
void update7SEG_Buffer(int num_road1, int num_road2) {
    led_buffer[0] = (num_road1 / 10) % 10;
    led_buffer[1] = num_road1 % 10;
    led_buffer[2] = (num_road2 / 10) % 10;
    led_buffer[3] = num_road2 % 10;
}

void update7SEG(int index) {
    switch (index) {
        case 0:
            HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, LED_ON);
            HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, LED_OFF);
            display7SEG(led_buffer[0]);
            break;
        case 1:
            HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, LED_ON);
            HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, LED_OFF);
            display7SEG(led_buffer[1]);
            break;
        case 2:
            HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, LED_ON);
            HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, LED_OFF);
            display7SEG(led_buffer[2]);
            break;
        case 3:
            HAL_GPIO_WritePin(EN0_GPIO_Port, EN0_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN1_GPIO_Port, EN1_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN2_GPIO_Port, EN2_Pin, LED_OFF);
            HAL_GPIO_WritePin(EN3_GPIO_Port, EN3_Pin, LED_ON);
            display7SEG(led_buffer[3]);
            break;
        default:
            break;
    }
}

void scan7SEG(void) {
    update7SEG(index_led);
    index_led = (index_led + 1) % 4;
}

// Tắt toàn bộ 12 LED giao thông (PA5 -> PA10)
void turnOffAllTrafficLeds(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                             GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10, LED_OFF);
}

// Bật/tắt đèn Đường Dọc (PA5: Đỏ, PA6: Vàng, PA7: Xanh)
void setTrafficLed_Vertical(int red, int amber, int green) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, red   ? LED_ON : LED_OFF);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, amber ? LED_ON : LED_OFF);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, green ? LED_ON : LED_OFF);
}

// Bật/tắt đèn Đường Ngang (PA8: Đỏ, PA9: Vàng, PA10: Xanh)
void setTrafficLed_Horizontal(int red, int amber, int green) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,  red   ? LED_ON : LED_OFF);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,  amber ? LED_ON : LED_OFF);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, green ? LED_ON : LED_OFF);
}

// Đảo trạng thái LED Đỏ cả 2 đường (PA5 và PA8)
void toggleRedLeds(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_8);
}

// Đảo trạng thái LED Vàng cả 2 đường (PA6 và PA9)
void toggleAmberLeds(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_9);
}

// Đảo trạng thái LED Xanh cả 2 đường (PA7 và PA10)
void toggleGreenLeds(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7 | GPIO_PIN_10);
}

