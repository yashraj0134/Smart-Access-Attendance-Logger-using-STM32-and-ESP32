/*
 * RTC.c
 *
 *  Created on: May 7, 2026
 *      Author: Yashraj
 */


#include "RTC.h"
#include <stdio.h>

extern RTC_HandleTypeDef hrtc;


void set_time(uint8_t hr, uint8_t min, uint8_t sec)
{
	  RTC_TimeTypeDef sTime = {0};

	   sTime.Hours = hr;
	   sTime.Minutes = min;
	   sTime.Seconds = sec;
	   sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	   sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	   if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	   {
	     Error_Handler();
	   }
}

void set_date(uint8_t year, uint8_t month, uint8_t date, uint8_t day)
{
	  RTC_DateTypeDef sDate = {0};

	  sDate.WeekDay = day;
	  sDate.Month = month;
	  sDate.Date = date;
	  sDate.Year = year;

	  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	  {
	    Error_Handler();
	  }

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2345);  // backup register
}


void get_time_date(char *time, char *date)

{
	RTC_DateTypeDef  gdate;
	RTC_TimeTypeDef  gtime;

	HAL_RTC_GetTime(&hrtc, &gtime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gdate, RTC_FORMAT_BIN);

	sprintf((char*) time, "%02d-%02d-%02d", gtime.Hours, gtime.Minutes, gtime.Seconds);
	sprintf((char*) date, "%02d-%02d-%02d", gdate.Date, gdate.Month, gdate.Year);
}
