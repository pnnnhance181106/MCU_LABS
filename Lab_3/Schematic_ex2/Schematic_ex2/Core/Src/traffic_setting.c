#include "traffic_setting.h"
#include "input_reading.h"
#include "input_processing.h"
#include "led_display.h"

#define MODE_RED 2
#define MODE_YELLOW 3
#define MODE_GREEN 4

static int redTime = 10; //Thoi gian red hien tai
static int yellowTime = 3; //tg yellow hien tai
static int greenTime = 7; //tg green hien tai

static int tempRedTime = 10; //tg dang dieu chinh
static int tempYellowTime = 3;
static int tempGreenTime = 7;

//luu trang thai nut truoc do
static unsigned char previous_button_2 = 0;
static unsigned char previous_button_3 = 0;

static int previousMode = -1;

int getRedTime(void) {
	return redTime;
}

int getYellowTime(void) {
	return yellowTime;
}

int getGreenTime(void) {
	return greenTime;
}

int getTempRedTime(void) {
	return tempRedTime;
}

int getTempYellowTime(void) {
	return tempYellowTime;
}

int getTempGreenTime(void) {
	return tempGreenTime;
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
		switch(mode) {
		case MODE_RED:
			tempRedTime = redTime;
			break;
		case MODE_YELLOW:
			tempYellowTime = yellowTime;
			break;
		case MODE_GREEN:
			tempGreenTime = greenTime;
			break;
		}
		return;
	}

	int *tempTime = NULL; //chua dia chi temp[color]
	int *currTime = NULL; //chua dia chi tg[color]
	switch(mode) {
	case MODE_RED:
		tempTime = &tempRedTime;
		currTime = &redTime;
		break;
	case MODE_YELLOW:
		tempTime = &tempYellowTime;
		currTime = &yellowTime;
		break;
	case MODE_GREEN:
		tempTime = &tempGreenTime;
		currTime = &greenTime;
		break;
	}


	if (button_2_pressed) {
		(*tempTime)++;
		if (*tempTime > 99) {
			*tempTime = 1;
		}
		update7SEG_Buffer(MODE_RED, *tempTime);
	}

	if (button_3_pressed) {
		*currTime = *tempTime;
		update7SEG_Buffer(MODE_RED, *currTime);
	}
}
