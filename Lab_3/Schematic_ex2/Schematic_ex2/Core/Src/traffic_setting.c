#include "traffic_setting.h"
#include "input_reading.h"
#include "input_processing.h"
#include "led_display.h"

#define MODE_RED 2

static int redTime = 20; //Thoi gian red hien tai
static int tempRedTime = 20; //tg dang dieu chinh

//luu trang thai nut truoc do
static unsigned char previous_button_2 = 0;
static unsigned char previous_button_3 = 0;

static int previousMode = -1;

int getRedTime(void) {
	return redTime;
}

int getTempRedTime(void) {
	return tempRedTime;
}

void traffic_setting_update(void) {
	//doc trang thai da chong doi
	unsigned char button_2 = is_button_pressed(1);
	unsigned char button_3 = is_button_pressed(2);

	//phat hien neu co su kien nhan moi, chi cap nhat trang thai dung 1 lan
	unsigned button_2_pressed = button_2 && !previous_button_2;
	unsigned button_3_pressed = button_3 && !previous_button_3;

	//cap nhat trang thai nut nhan moi
	previous_button_2 = button_2;
	previous_button_3 = button_3;

	//kiem tra mode dang chay
	if (mode != previousMode) {
		previousMode = mode;
		if (mode == MODE_RED) {
			tempRedTime = redTime;
		}
		return;
	}


	if (mode != MODE_RED) {
		return;
	}

	if (button_2_pressed) {
		tempRedTime++;
		if (tempRedTime > 99) {
			tempRedTime = 1;
		}
		update7SEG_Buffer(MODE_RED, tempRedTime);
	}

	if (button_3_pressed) {
		redTime = tempRedTime;
		update7SEG_Buffer(MODE_RED, redTime);
	}
}
