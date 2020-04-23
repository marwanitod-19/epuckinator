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
#include <motors.h>

//#include <sensors/VL53L0X/VL53L0X.h>

#define NO_OBSTACLES	10
#define OBST_THRESHOLD  50
enum proximity{prox0 = 0, prox1 = 1, prox2 = 2, prox3 = 3, prox4 = 4, prox5 = 5, prox6 = 6, prox7 = 7};

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
		for(int i = 0 ; i < 8 ; i++){
			inv_distance = get_calibrated_prox(i);
			chprintf((BaseSequentialStream *)&SD3, "Sensor %d sees an object at distance %d \n\r", i, get_calibrated_prox(i));
			if(inv_distance > OBST_THRESHOLD){
				//sat_sensor[i] = 1;
				highest_prox = 0;
				if(i != 0 && get_calibrated_prox(i-1) < inv_distance){
					highest_prox = i;
				}
			}
		}
		chprintf((BaseSequentialStream *)&SD3, "Prox number : %d \n\r", highest_prox);
		switch(highest_prox){
			case prox0:
				rotator(8);
				chThdSleep(MS2ST(1000));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case prox1:
				rotator(8);
				chThdSleep(MS2ST(800));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case prox2:
				rotator(8);
				chThdSleep(MS2ST(500));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case prox3:
				rotator(8);
				chThdSleep(MS2ST(200));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case prox4:
				rotator(-8);
				chThdSleep(MS2ST(200));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case prox5:
				rotator(-8);
				chThdSleep(MS2ST(500));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case prox6:
				rotator(-8);
				chThdSleep(MS2ST(800));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case prox7:
				rotator(-8);
				chThdSleep(MS2ST(1000));
				right_motor_set_pos(right_motor_get_pos()-50);
				left_motor_set_pos(left_motor_get_pos()-50);
				break;
			case NO_OBSTACLES:
				break;
		}
		chprintf((BaseSequentialStream *)&SD3, "The Closest distance is at sensor %d and the distance is %d \n\r\r", highest_prox, get_calibrated_prox(highest_prox));
		chThdSleep(MS2ST(100));
	}



}

void eyes_start(void){
	chThdCreateStatic(waEyes, sizeof(waEyes), NORMALPRIO, Eyes, NULL);
}
