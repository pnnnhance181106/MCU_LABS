/*
 * timer.h
 *
 *  Created on: Oct 18, 2026
 *      Author: nk
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"

// Chu kỳ ngắt phần cứng của Timer 2 (đơn vị: ms)
// Khi đổi cấu hình Timer trong .ioc (ví dụ sang 1ms), chỉ cần sửa số này!
#define TIMER_CYCLE 10

// Số lượng bộ định thời phần mềm (Software Timers) dùng cho toàn bài:
// Timer 0: Đếm 1 giây cho đèn giao thông (Mode 1)
// Timer 1: Quét 4 LED 7 đoạn (ví dụ mỗi 250ms hoặc 50ms)
// Timer 2: Chớp tắt LED 2Hz (chu kỳ 500ms -> đảo trạng thái mỗi 250ms)
#define NO_OF_TIMERS 3

void setTimer(int index, int duration);
int isTimerExpired(int index);
void timerRun(void);

#endif /* INC_TIMER_H_ */
