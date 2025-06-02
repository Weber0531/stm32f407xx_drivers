/*
 * 016RTC_LCD.c
 *
 *  Created on: May 27, 2025
 *      Author: weber
 */


#include <stdio.h>
#include "DS1307.h"


char* get_day_of_week(uint8_t i){
	char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

	return days[i-1];
}

// hh:mm:ss
char* time_to_string(RTC_time_t *rtc_time){

}

int main(void){
	RTC_time_t current_time;
	RTC_date_t current_date;

	printf("RTC test\n");

	if(ds1307_init()) {
		printf("RTC init is failed\n");
		while(1);
	}

	current_date.day = MONDAY;
	current_date.date = 2;
	current_date.month = 6;
	current_date.year = 25;

	current_time.hours = 12;
	current_time.minutes = 1;
	current_time.seconds = 25;
	current_time.time_format = TIME_FORMAT_12HRS_AM;

	ds1307_set_current_date(&current_date);
	ds1307_set_current_time(&current_time);

	ds1307_get_current_date(&current_date);
	ds1307_get_current_time(&current_time);

	char *am_pm;
	if(current_time.time_format != TIME_FORMAT_24HRS) {
		am_pm = (current_time.time_format) ? "PM" : "AM";
		printf("Current time = %s %s\n", time_to_string(&current_time), am_pm); // 12:01:25 AM
	} else {
		printf("Current time = %s\n", time_to_string(&current_time)); // 00:01:25
	}

	// 02/06/25 <Monday>
	printf("Current date = %s <%s>\n",date_to_string(&current_date), get_day_of_week(current_date.day));

	return 0;
}





