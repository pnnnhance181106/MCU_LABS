#include "mode_display.h"
#include "input_processing.h"
#include "led_display.h"
#include "timer.h"
#include "traffic_setting.h"

/*Thoi gian mac dinh de test ex6*/

static int previousMode = -1;
void switch_mode(void) {
	if (previousMode != mode) {
		previousMode = mode;
		turnOffAllTrafficLeds();
		setTimer(2, 250);
		switch(mode) {
		case 1:
			update7SEG_Buffer(0, 0);
			break;
		case 2:
			update7SEG_Buffer(2, getTempRedTime());
			break;
		case 3:
			update7SEG_Buffer(3, getTempYellowTime());
			break;
		case 4:
			update7SEG_Buffer(4, getTempGreenTime());
			break;
		}
	}
	if (isTimerExpired(2)) {
		//nhap nhay moi 250ms
		setTimer(2, 250);
        switch (mode) {
            case 2:
                toggleRedLeds();
                break;

            case 3:
                toggleAmberLeds();
                break;

            case 4:
                toggleGreenLeds();
                break;

            default:
                break;
        }
	}
}
