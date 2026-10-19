/*
 * traffic_setting.h
 *
 *  Created on: Oct 19, 2026
 *      Author: dell
 */

#ifndef INC_TRAFFIC_SETTING_H_
#define INC_TRAFFIC_SETTING_H_

int getRedTime(void);
int getYellowTime(void);
int getGreenTime(void);
int getTempRedTime(void);
int getTempYellowTime(void);
int getTempGreenTime(void);

void traffic_setting_update(void);




#endif /* INC_TRAFFIC_SETTING_H_ */
