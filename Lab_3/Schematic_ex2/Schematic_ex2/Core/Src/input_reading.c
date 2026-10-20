/*
 * input_reading.c
 *
 *  Created on: Oct 18, 2026
 *      Author: nk
 */
#include "main.h"
#include "input_reading.h"
#include "timer.h"

// Hệ thống sử dụng 3 nút nhấn (Mode, Increase, Set)
#define NO_OF_BUTTONS 3

// Thời gian nhấn giữ 1 giây (1000ms), tự động co giãn theo TIMER_CYCLE (Ex 4)
#define DURATION_FOR_AUTO_INCREASING (1000 / TIMER_CYCLE)

#define BUTTON_IS_PRESSED  GPIO_PIN_RESET
#define BUTTON_IS_RELEASED GPIO_PIN_SET

// Bộ đệm lưu kết quả cuối cùng sau khi đã chống rung
static volatile GPIO_PinState buttonBuffer[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};

// Hai bộ đệm phục vụ lọc chống rung (N = 2)
static GPIO_PinState debounceButtonBuffer1[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};
static GPIO_PinState debounceButtonBuffer2[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};

// Cờ báo hiệu nút được nhấn giữ hơn 1 giây
static volatile uint8_t flagForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};

// Bộ đếm thời gian nhấn giữ nút
static uint16_t counterForButtonPress1s[NO_OF_BUTTONS] = {0, 0, 0};

// Mảng ánh xạ chân GPIO của 3 nút nhấn (PA11, PA12, PA13)
static GPIO_TypeDef* buttonPort[NO_OF_BUTTONS] = {
    BUTTON_1_GPIO_Port, BUTTON_2_GPIO_Port, BUTTON_3_GPIO_Port
};
static uint16_t buttonPin[NO_OF_BUTTONS] = {
    BUTTON_1_Pin, BUTTON_2_Pin, BUTTON_3_Pin
};

void button_reading(void) {
    for (unsigned char i = 0; i < NO_OF_BUTTONS; i++) {
        debounceButtonBuffer2[i] = debounceButtonBuffer1[i];
        debounceButtonBuffer1[i] = HAL_GPIO_ReadPin(buttonPort[i], buttonPin[i]);

        if (debounceButtonBuffer1[i] == debounceButtonBuffer2[i]) {
            buttonBuffer[i] = debounceButtonBuffer1[i];

            if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
                // Nếu nút đang được nhấn, bắt đầu đếm đến 1 giây
                if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                    counterForButtonPress1s[i]++;
                } else {
                    // Bật cờ khi đã giữ nút đủ 1 giây
                    flagForButtonPress1s[i] = 1;
                }
            } else {
                // Nếu nhả nút, xóa bộ đếm và cờ 1 giây
                counterForButtonPress1s[i] = 0;
                flagForButtonPress1s[i] = 0;
            }
        }
    }
}

// Kiểm tra nút thứ index (0, 1, 2) có đang được nhấn hay không
unsigned char is_button_pressed(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0;
    return (buttonBuffer[index] == BUTTON_IS_PRESSED);
}

// Kiểm tra nút thứ index (0, 1, 2) có được nhấn giữ hơn 1 giây hay không
unsigned char is_button_pressed_1s(unsigned char index) {
    if (index >= NO_OF_BUTTONS) return 0xff;
    return (flagForButtonPress1s[index] == 1);
}


