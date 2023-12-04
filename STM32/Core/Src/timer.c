/*
 * timer.c
 *
 *  Created on: Nov 20, 2023
 *      Author: admin
 */
#include "timer.h"
#include "main.h"

int timer0_counter;
int timer0_flag = 0;
int TIMER_CYCLE = 10;

void setTimer0(int duration){
	timer0_counter = duration / TIMER_CYCLE;
	timer0_flag = 0;
}

void timerRun(){
	if (timer0_counter > 0){
		timer0_counter--;
		if (timer0_counter == 0)
			timer0_flag = 1;
	}
}

