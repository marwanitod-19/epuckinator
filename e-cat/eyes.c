#include <eyes.h>
#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <moves.h>
#include <sensors/proximity.h>
//#include <sensors/VL53L0X/VL53L0X.h>

#define NO_OBSTACLES	10
#define OBST_THRESHOLD  50
enum proximity{prox0 = 2, prox1 = 4, prox2 = 8, prox3 = 12, prox4 = -12, prox5 = -8, prox6 = -4, prox7 = -2};

void view(){


}

void follow(){


}

static THD_WORKING_AREA(waEyes, 256);
static THD_FUNCTION(Eyes, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	//uint8_t sat_sensor[8] = { 0 };
	uint8_t highest_prox = NO_OBSTACLES;
	uint16_t inv_distance = 0;



	proximity_start();
	calibrate_ir();

	//VL53L0X_start();

	while(1){
		highest_prox = NO_OBSTACLES;
		for(int i = 0 ; i < 7 ; i++){
			inv_distance = get_calibrated_prox(i);
			chprintf((BaseSequentialStream *)&SD3, "Sensor %d sees an object at distance %d \n\n\n", i, get_calibrated_prox(i));
			if(inv_distance > OBST_THRESHOLD){
				//sat_sensor[i] = 1;
				if(i != 0 && get_calibrated_prox(i-1) < inv_distance){
					highest_prox = i;
				}
			}
		}

		switch(highest_prox){
			case prox0:
				rotator(2);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case prox1:
				rotator(4);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case prox2:
				rotator(8);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case prox3:
				rotator(12);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case prox4:
				rotator(-12);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case prox5:
				rotator(-8);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case prox6:
				rotator(-4);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case prox7:
				rotator(-2);
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
			case NO_OBSTACLES:
				break;
		}
		chprintf((BaseSequentialStream *)&SD3, "The Closest distance is at sensor %d and the distance is %d \n\n\n", highest_prox, get_calibrated_prox(highest_prox));
		chThdSleep(MS2ST(1000));
	}



}

void eyes_start(void){
	chThdCreateStatic(waEyes, sizeof(waEyes), NORMALPRIO, Eyes, NULL);
}
