/*
 * timer.c
 *
 *  Created on: Oct 18, 2026
 *      Author: nk
 */
#include "main.h"
#include "timer.h"
#include "input_reading.h"

static int timer_counter[NO_OF_TIMERS] = {0};
static int timer_flag[NO_OF_TIMERS] = {0};

// duration truyền vào tính bằng mili-giây (ms), ví dụ: setTimer(2, 250);
void setTimer(int index, int duration) {
    if (index >= 0 && index < NO_OF_TIMERS) {
        timer_counter[index] = duration / TIMER_CYCLE;
        timer_flag[index] = 0;
    }
}

// Kiểm tra xem timer đã đếm xong chưa (trả về 1 nếu đã hết giờ)
int isTimerExpired(int index) {
    if (index >= 0 && index < NO_OF_TIMERS) {
        if (timer_flag[index] == 1) {
            return 1;
        }
    }
    return 0;
}

// Hàm giảm bộ đếm mỗi lần có ngắt Timer
void timerRun(void) {
    for (int i = 0; i < NO_OF_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] <= 0) {
                timer_flag[i] = 1;
            }
        }
    }
}

// Hàm ngắt Timer 2 được gọi mỗi 10ms (Mục 4.3.3)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        timerRun();
        button_reading();
    }
}

