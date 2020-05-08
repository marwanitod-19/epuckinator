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
#define REACTION_ROT_SPEED		3	//The speed found for a correct rotation, bigger than this
									//and the e-cat will move past his GOAL_ANGLE too easily.



float speed_regulator(float phase_FL){
	float speed = 0;

	if (phase_FL > (GOAL_ANGLE + ERROR_THRESHOLD)){
		speed = -REACTION_ROT_SPEED; //phase_FL*20;
	}
	else if (phase_FL < (GOAL_ANGLE - ERROR_THRESHOLD)){
		speed =	REACTION_ROT_SPEED; // phase_FL*20;
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
	systime_t time_passed;
	time_passed = chVTGetSystemTime();

	float speed = 0;

	while(1){
		time = chVTGetSystemTime();
		speed = speed_regulator(get_phase_LR());
		if (get_speed_process_bool())// || !get_goal_bool()){
			rotator(speed);

		else{
			//rotator(0);
		}
		if (get_goal_bool()){
			if (chVTGetSystemTime()-time_passed > MS2ST(500)){
				//arrêter tout et stroll pendant un moment
				//chprintf((BaseSequentialStream *)&SDU1, "Goal reached \r");
				stroll(3*STROLL_SPEED,3*STROLL_SPEED);
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
			time_passed = chVTGetSystemTime();
		}
	//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void speed_regulator_start(void){
	chThdCreateStatic(waSpeedRegulator, sizeof(waSpeedRegulator), HIGHPRIO, SpeedRegulator, NULL);
}



