/*
 * pi_regulator.c
 *
 *  Created on: 2 mai 2020
 *      Author: Fahra
 */

#include "ch.h"
#include "hal.h"
//#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <moves.h>
#include <pi_regulator.h>
#include <audio_processing.h>



float pi_regulator(float phase_FL){
	float speed = 0;

/*
	float error;
	static float sum_error = 0;
	static float last_error = 0;
	static float last_last_error = 0;
	static float last_last_last_error = 0;
	float value;


	error = phase_FL - GOAL_ANGLE;
	sum_error += error;

	if(sum_error > MAX_SUM_ERROR){
		sum_error = MAX_SUM_ERROR;
	}else if(sum_error < -MAX_SUM_ERROR){
		sum_error = -MAX_SUM_ERROR;
	}

	value = 0.10*error + 0.3*last_error + 0.3*last_last_error + 0.3*last_last_last_error;


	speed = KP*value + KI*sum_error; // KP*error + KI*sum_error + KD*(error - last_error);
	//chprintf((BaseSequentialStream *) &SDU1, "error: %f		speed: %f\r", error, speed);

	last_last_last_error = last_last_error;
	last_last_error = last_error;
	last_error = error;
*/


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
	//int16_t speed_correction = 0;

	while(1){
		time = chVTGetSystemTime();

		speed = pi_regulator(get_phase_FL());
		//chprintf((BaseSequentialStream *) &SDU1, "%d \r", get_pi_process_bool());

		if (get_pi_process_bool()){
			rotator(speed);
			//chprintf((BaseSequentialStream *) &SDU1, "Front-Left phase : %f \r", get_phase_FL());
		}
		else{
			rotator(0);
		}



//	        //computes the speed to give to the motors
//	        //distance_cm is modified by the image processing thread
//	        speed = pi_regulator(get_distance_cm(), GOAL_DISTANCE);
//	        //computes a correction factor to let the robot rotate to be in front of the line
//	        speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));
//
//	        //if the line is nearly in front of the camera, don't rotate
//	        if(abs(speed_correction) < ROTATION_THRESHOLD){
//	        	speed_correction = 0;
//	        }
//
//	        //applies the speed from the PI regulator and the correction for the rotation
//			right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
//			left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}

void pi_regulator_start(void){
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator), NORMALPRIO, PiRegulator, NULL);
}
