#include "traffic_light_fsm.h"
#include "input_processing.h"
#include "traffic_setting.h"
#include "led_display.h"
#include "timer.h"

#define MODE_NORMAL       1
#define TIMER_TRAFFIC     0

typedef enum{
	TRAFFIC_S1,
	TRAFFIC_S2,
	TRAFFIC_S3,
	TRAFFIC_S4
}trafficState;

trafficState currentState = TRAFFIC_S1; //trang thai hien tai
static int remainTime = 0; //tg con lai cua trang thai hien tai
static int isNormalMode = 0; //ktra trang thai mode 1

//ham dung de bat den tuong ung voi 4 trang thai
static void apply_traffic_lights(void) {
	switch(currentState) {
	case TRAFFIC_S1:
		//duong doc xanh, duong ngang do
		setTrafficLed_Vertical(0, 0, 1);
		setTrafficLed_Horizontal(1, 0, 0);
		break;
	case TRAFFIC_S2:
		//duong doc vang, duong ngang do
		setTrafficLed_Vertical(0, 1, 0);
		setTrafficLed_Horizontal(1, 0, 0);
		break;
	case TRAFFIC_S3:
		//duong doc do, duong ngang xanh
		setTrafficLed_Vertical(1, 0, 0);
		setTrafficLed_Horizontal(0, 0, 1);
		break;
	case TRAFFIC_S4:
		//duong doc do, duong ngang vang
		setTrafficLed_Vertical(1, 0, 0);
		setTrafficLed_Horizontal(0, 1, 0);
		break;
	}
}

//ham dung de lay thoi gian cua tung trang thai
static int get_current_state_time(void) {
	if (currentState == TRAFFIC_S1 || currentState == TRAFFIC_S3) {
		return getGreenTime();
	}
	return getYellowTime();
}

//ham gioi han gia tri hien thi led
static int limit_display(int value) {
	if (value < 0) return 0;
	if (value > 99) return 99;
	return value;
}

//Hien thi thoi gian con lai cua 2 duong
static void display_count_down(void) {
	int vertical_time = 0;
	int horizontal_time = 0;

	switch(currentState) {
	case TRAFFIC_S1:
		vertical_time = remainTime; //green
		horizontal_time = remainTime + getYellowTime(); //red
		break;
	case TRAFFIC_S2:
		vertical_time = remainTime; //yellow
		horizontal_time = remainTime; //red
		break;
	case TRAFFIC_S3:
		vertical_time = remainTime + getYellowTime(); //red
		horizontal_time = remainTime; //green
		break;
	case TRAFFIC_S4:
		vertical_time = remainTime; //red
		horizontal_time = remainTime; //yellow
		break;
	}

	update7SEG_Buffer(limit_display(vertical_time), limit_display(horizontal_time));
	//2 led dau chua tg cua duong doc, 2 led sau chua tg cua duong ngang
}

//ham dung de chuyen sang trang thai tiep theo
static void next_traffic_state(void) {
	switch(currentState) {
	case TRAFFIC_S1:
		currentState = TRAFFIC_S2;
		break;
	case TRAFFIC_S2:
		currentState = TRAFFIC_S3;
		break;
	case TRAFFIC_S3:
		currentState = TRAFFIC_S4;
		break;
	case TRAFFIC_S4:
		currentState = TRAFFIC_S1;
		break;
	}
	remainTime = get_current_state_time();

	apply_traffic_lights();
	display_count_down();
}

//ham dung de xu ly fsm
void traffic_light_update(void) {
	if (mode != MODE_NORMAL) {
		isNormalMode = 0;
		return;
		//chi xu ly khi dang o mode mac dinh
	}

	if (!isNormalMode) {
		isNormalMode = 1;

		currentState = TRAFFIC_S1;
		remainTime = get_current_state_time();

		apply_traffic_lights();
		display_count_down();

		setTimer(TIMER_TRAFFIC, 1000);
		return;
	}

	if (isTimerExpired(TIMER_TRAFFIC)) {
		//neu da chay het tg thi set lai
		setTimer(TIMER_TRAFFIC, 1000);
		remainTime--;

		if (remainTime <= 0) {
			//neu het tg thi chuyen trang thai
			next_traffic_state();
		}
		else {
			//khong thi hien thi tg con lai
			display_count_down();
		}
	}
}


