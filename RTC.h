/*
 * RTC.h
 *
 *  Created on: May 7, 2026
 *      Author: Yashraj
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "main.h"
#include "stdint.h"

void set_time(uint8_t hr, uint8_t min, uint8_t sec);
void set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day);
void get_time_date(char *time, char *date);




#endif /* INC_RTC_H_ */
