/*
 * led_display.h
 *
 *  Created on: Oct 18, 2026
 *      Author: nk
 */

#ifndef INC_LED_DISPLAY_H_
#define INC_LED_DISPLAY_H_

#include "main.h"

// Hàm cập nhật 2 số hiển thị cho Đường 1 (2 LED đầu) và Đường 2 (2 LED sau)
void update7SEG_Buffer(int num_road1, int num_road2);

// Hàm hiển thị 1 LED 7 đoạn tại vị trí index (0 -> 3)
void update7SEG(int index);

// Hàm quét tuần tự 4 LED 7 đoạn (gọi định kỳ bằng Timer 1)
void scan7SEG(void);

// Các hàm điều khiển 12 LED đơn (PA5-PA7: Đường Dọc, PA8-PA10: Đường Ngang)
void turnOffAllTrafficLeds(void);
void setTrafficLed_Vertical(int red, int amber, int green);
void setTrafficLed_Horizontal(int red, int amber, int green);

// Các hàm đảo trạng thái (chớp tắt 2Hz) dùng cho Mode 2, 3, 4
void toggleRedLeds(void);
void toggleAmberLeds(void);
void toggleGreenLeds(void);

#endif /* INC_LED_DISPLAY_H_ */
