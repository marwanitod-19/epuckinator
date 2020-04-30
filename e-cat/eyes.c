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
#include <audio_emitter.h>
#include <selector.h>

//#include <sensors/VL53L0X/VL53L0X.h>
static int obst_move = 0;

#define NO_OBSTACLES	10
#define OBST_THRESHOLD  50
enum proximity{prox0 = 0, prox1, prox2, prox3, prox4, prox5, prox6, prox7};

void view(){


}

void follow(){


}

int get_obst_move(void){
	return obst_move;
}

static THD_WORKING_AREA(waEyes, 256);
static THD_FUNCTION(Eyes, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	uint8_t sat_sensor[8] = { 0 };
	uint8_t highest_prox = NO_OBSTACLES;
	uint8_t sensor_count = 0;
	uint16_t inv_distance = 0;
	bool mv_in_progress = false;



	proximity_start();
	calibrate_ir();

	//VL53L0X_start();

	while(1){
			highest_prox = NO_OBSTACLES;
			for(int i = 0 ; i < 8 ; i++){
				inv_distance = get_calibrated_prox(i);
				//chprintf((BaseSequentialStream *)&SD3, "Sensor %d sees an object at distance %d \n\r", i, get_calibrated_prox(i));
				if(inv_distance > OBST_THRESHOLD){
					sat_sensor[i] = 1;
					highest_prox = prox0;
					mv_in_progress = true;
					obst_move = 1;
					if(i != 0 && get_calibrated_prox(i-1) < inv_distance){
						highest_prox = i;
						sensor_count++;
					}
				}
			}
			if(get_selector() != 1)
			{
				switch(highest_prox){
					case prox0:
						rotator(8);
						chThdSleep(MS2ST(1000));
						stroll(5,5);
						chThdSleep(MS2ST(1000));
						break;
					case prox1:
						rotator(8);
						chThdSleep(MS2ST(800));
						stroll(5,5);
						chThdSleep(MS2ST(1000));
						break;
					case prox2:
						rotator(8);
						chThdSleep(MS2ST(500));
						stroll(5,5);
						chThdSleep(MS2ST(1000));
						break;
		//			case prox3:
		//				rotator(8);
		//				chThdSleep(MS2ST(200));
		//				stroll(5,5);
		//				chThdSleep(MS2ST(1000));
		//				break;
		//			case prox4:
		//				rotator(-8);
		//				chThdSleep(MS2ST(200));
		//				stroll(5,5);
		//				chThdSleep(MS2ST(1000));
		//				break;
					case prox5:
						rotator(-8);
						chThdSleep(MS2ST(500));
						stroll(5,5);
						chThdSleep(MS2ST(1000));
						break;
					case prox6:
						rotator(-8);
						chThdSleep(MS2ST(800));
						stroll(5,5);
						chThdSleep(MS2ST(1000));
						break;
					case prox7:
						rotator(-8);
						chThdSleep(MS2ST(1000));
						stroll(5,5);
						chThdSleep(MS2ST(1000));
						break;
					case NO_OBSTACLES:
						break;
				}

				if(highest_prox == 3 || highest_prox == 4 ){
					if(sat_sensor[3] == 1 && sat_sensor[4] == 1){
						chprintf((BaseSequentialStream *)&SD3, "Meow \n\r");
						stroll(15, 15);
						meow();
						chThdSleep(MS2ST(200));
					}
				}
				sensor_count = 0;
				//chprintf((BaseSequentialStream *)&SD3, "The Closest distance is at sensor %d and the distance is %d \n\r\r", highest_prox, get_calibrated_prox(highest_prox));
				if(mv_in_progress){
					chThdSleep(MS2ST(1000));
					mv_in_progress = false;
				}
				else
					chThdSleep(MS2ST(100));
			}

			else{
				switch(highest_prox){
					case prox0:
						rotator(-8);
						chThdSleep(MS2ST(200));
						stroll(5,5);
						chThdSleep(MS2ST(500));
						break;
					case prox1:
						rotator(-8);
						chThdSleep(MS2ST(500));
						stroll(5,5);
						chThdSleep(MS2ST(500));
						break;
					case prox2:
						rotator(-8);
						chThdSleep(MS2ST(800));
						stroll(5,5);
						purr();
						chThdSleep(MS2ST(500));
						break;
					case prox3:
						rotator(-8);
						chThdSleep(MS2ST(1000));
						stroll(5,5);
						chThdSleep(MS2ST(500));
						break;
					case prox4://other side
						rotator(8);
						chThdSleep(MS2ST(1000));
						stroll(5,5);
						chThdSleep(MS2ST(500));
						break;
					case prox5:
						rotator(8);
						chThdSleep(MS2ST(800));
						stroll(5,5);
						chThdSleep(MS2ST(500));
						break;
					case prox6:
						rotator(8);
						chThdSleep(MS2ST(500));
						stroll(5,5);
						chThdSleep(MS2ST(500));
						break;
					case prox7:
						rotator(8);
						chThdSleep(MS2ST(200));
						stroll(5,5);
						chThdSleep(MS2ST(500));
						break;
					case NO_OBSTACLES:
						break;
				}
				sensor_count = 0;
				if(mv_in_progress){
					//chThdSleep(MS2ST(1000));
					mv_in_progress = false;
				}
				else
					chThdSleep(MS2ST(100));
				//chThdSleep(MS2ST(3000));
			}
				obst_move = 0;
	}
}

void eyes_start(void){
	chThdCreateStatic(waEyes, sizeof(waEyes), HIGHPRIO, Eyes, NULL);
}
