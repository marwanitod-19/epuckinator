/*
 * pi_regulator.c
 *
 *  Created on: 2 mai 2020
 *      Author: Fahra
 */

#include "ch.h"
#include "hal.h"
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <moves.h>
#include <pi_regulator.h>
#include <audio_processing.h>



float pi_regulator(float phase_FL){
	float speed = 0;

	if (phase_FL > (GOAL_ANGLE + ERROR_THRESHOLD)){
		speed = 3;
	}
	else if (phase_FL < (GOAL_ANGLE - ERROR_THRESHOLD)){
		speed = -3;
	}
	else{
		//PI_REGULATOR_PROCESS = false;
	}

	return speed;
}

static THD_WORKING_AREA(waPiRegulator, 256);
static THD_FUNCTION(PiRegulator, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	float speed = 0;

	while(1){
		time = chVTGetSystemTime();

		speed = pi_regulator(get_phase_FL());

		if (get_speed_process_bool()){
			rotator(speed);
		}
		else{
			rotator(0);
		}

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void pi_regulator_start(void){
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator), NORMALPRIO, PiRegulator, NULL);
}