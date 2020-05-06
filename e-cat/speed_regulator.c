/*
 * speed_regulator.c
 *
 *  Created on: 1 mai 2020
 *      Author: Fahra
 */


#include "ch.h"
#include "hal.h"
#include <usbcfg.h>
#include <chprintf.h>

#include <main.h>
#include <motors.h>
#include <moves.h>
#include <speed_regulator.h>
#include <audio_processing.h>
#include <selector.h>



float speed_regulator(float phase_FL){
	float speed = 0;

	if (phase_FL > (GOAL_ANGLE + ERROR_THRESHOLD)){
		speed = 4;
	}
	else if (phase_FL < (GOAL_ANGLE - ERROR_THRESHOLD)){
		speed = -4;
	}
	else{
		//PI_REGULATOR_PROCESS = false;
	}

	return speed;
}

static THD_WORKING_AREA(waSpeedRegulator, 256);
static THD_FUNCTION(SpeedRegulator, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	systime_t time2;
	time2 = chVTGetSystemTime();

	float speed = 0;

	while(1){
		time = chVTGetSystemTime();
		if(get_selector() == 0){
			speed = speed_regulator(get_phase_FL());
			if (get_speed_process_bool())// || !get_goal_bool()){
				rotator(speed);
			}
			else{
				//rotator(0);
			}
			if (get_goal_bool()){
				if (chVTGetSystemTime()-time2 > MS2ST(500)){
					//arrêter tout et stroll penrdant un moment
					//chprintf((BaseSequentialStream *)&SDU1, "Goal reached \r");
					stroll(15,15);
					chThdSleep(MS2ST(2000));

					//chThdSleep(MS2ST(4000));
					//stroll(0,0);
					//chThdSleep(MS2ST(2000));
					set_goal_bool(false);
				}
				else{

				}
			}
			else{
				time2 = chVTGetSystemTime();
			}
		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void speed_regulator_start(void){
	chThdCreateStatic(waSpeedRegulator, sizeof(waSpeedRegulator), HIGHPRIO, SpeedRegulator, NULL);
}



