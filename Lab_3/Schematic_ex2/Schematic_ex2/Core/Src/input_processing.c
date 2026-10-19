/*
 * input_processing.c
 *
 *  Created on: Oct 18, 2026
 *      Author: nk
 */
#include "main.h"
#include "input_reading.h"
#include "input_processing.h"
#include "led_display.h"
#include "timer.h"

enum ButtonState {
    BUTTON_RELEASED,
    BUTTON_PRESSED,
    BUTTON_PRESSED_MORE_THAN_1_SECOND
};

static enum ButtonState buttonState[3] = {
    BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED
};

// Mode mặc định khởi động là Mode 1 (1 -> 4)
extern int mode = 1;

void fsm_for_input_processing(void) {
    // Xử lý Nút nhấn 1 (Index 0): Chuyển Mode 1 -> 2 -> 3 -> 4 -> 1 (Yêu cầu Ex 5)
    switch (buttonState[0]) {
        case BUTTON_RELEASED:
            if (is_button_pressed(0)) {
                buttonState[0] = BUTTON_PRESSED;

                // Tăng mode tuần hoàn từ 1 đến 4
                mode++;
                if (mode > 4) {
                    mode = 1;
                }

                // Tắt hết đèn đơn để chuẩn bị sang mode mới và cập nhật số mode lên 7-seg
                turnOffAllTrafficLeds();
                update7SEG_Buffer(0, mode);
            }
            break;

        case BUTTON_PRESSED:
            if (!is_button_pressed(0)) {
                buttonState[0] = BUTTON_RELEASED;
            } else {
                if (is_button_pressed_1s(0)) {
                    buttonState[0] = BUTTON_PRESSED_MORE_THAN_1_SECOND;
                }
            }
            break;

        case BUTTON_PRESSED_MORE_THAN_1_SECOND:
            if (!is_button_pressed(0)) {
                buttonState[0] = BUTTON_RELEASED;
            }
            break;
    }
}

