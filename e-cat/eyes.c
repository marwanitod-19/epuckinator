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
#include <audio_processing.h>

static int obst_move = 0;

#define NO_OBSTACLES			10
#define OBST_THRESHOLD  		50		//threshold after which a proximity sensor consider an object as an obstacle.
#define NB_SENSORS				8
#define ACTIVATED				1
#define	MOVE_AV_OBSTACLE		1		//State of e-cat is "moving away from obstacle"
#define OBSTACLE_AWAY			0		//State of e-cat is "moved away from obstacle"
#define	NORMAL_MODE				1
#define FRIENDLY_MODE			2
#define ROTATION_SPEED			8		//Specifically used for the reaction rotation
#define NORMAL					1000	//Sleep time after executing a move when in normal mode
#define FRIENDLY				500		//Sleep time after executing a move when in friendly mode
enum proximity{prox0 = 0, prox1, prox2, prox3, prox4, prox5, prox6, prox7};


int get_obst_move(void){
	return obst_move;
}

void reaction(int rot_speed, int time, int delay){
	rotator(rot_speed);
	chThdSleep(MS2ST(time));
	stroll(STROLL_SPEED,STROLL_SPEED);
	chThdSleep(MS2ST(delay));
}

static THD_WORKING_AREA(waEyes, 256);
static THD_FUNCTION(Eyes, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	uint8_t sat_sensor[NB_SENSORS] = { 0 };
	uint8_t highest_prox = NO_OBSTACLES;
	uint8_t sensor_count = 0;
	uint16_t inv_distance = 0;
	bool mv_in_progress = false;

	proximity_start();
	calibrate_ir();

	while(1){
		highest_prox = NO_OBSTACLES;
		for(int i = 0 ; i < NB_SENSORS ; i++){
			inv_distance = get_calibrated_prox(i);
			if(inv_distance > OBST_THRESHOLD){
				sat_sensor[i] = ACTIVATED;
				highest_prox = prox0;
				mv_in_progress = true;
				obst_move = MOVE_AV_OBSTACLE;
				if(i != 0 && get_calibrated_prox(i-1) < inv_distance){
					highest_prox = i;
					sensor_count++;
				}
			}
		}
		if(get_selector() == NORMAL_MODE && !get_speed_process_bool())
		{
			switch(highest_prox){
				case prox0:
					reaction(ROTATION_SPEED, 1000, NORMAL);
					break;
				case prox1:
					reaction(ROTATION_SPEED, 800, NORMAL);
					break;
				case prox2:
					reaction(ROTATION_SPEED, 500, NORMAL);
					break;
				case prox5:
					reaction(-ROTATION_SPEED, 500, NORMAL);
					break;
				case prox6:
					reaction(-ROTATION_SPEED, 800, NORMAL);
					break;
				case prox7:
					reaction(-ROTATION_SPEED, 1000, NORMAL);
					break;
				case NO_OBSTACLES:
					break;
			}


			if((highest_prox == prox3 || highest_prox == prox4) && !get_speed_process_bool()){
				if(sat_sensor[prox3] == ACTIVATED && sat_sensor[prox4] == ACTIVATED){
					stroll(2*STROLL_SPEED, 2*STROLL_SPEED); //cf definition of STROLL_SPEED for the 2* explanation
					meow();
					chThdSleep(MS2ST(200));
				}
			}
		}

		else{
			chThdSleep(MS2ST(2000));
		}


		if(get_selector() == FRIENDLY_MODE && !get_speed_process_bool()){
			switch(highest_prox){
				case prox0:
					reaction(-ROTATION_SPEED, 200, FRIENDLY);
					purr();
					break;
				case prox1:
					reaction(-ROTATION_SPEED, 500, FRIENDLY);
					break;
				case prox2:
					reaction(-ROTATION_SPEED, 800, FRIENDLY);
					purr();
					break;
				case prox3:
					reaction(-ROTATION_SPEED, 1000, FRIENDLY);
					break;
				case prox4://other side
					reaction(ROTATION_SPEED, 1000, FRIENDLY);
					purr();
					break;
				case prox5:
					reaction(ROTATION_SPEED, 800, FRIENDLY);
					break;
				case prox6:
					reaction(ROTATION_SPEED, 500, FRIENDLY);
					purr();
					break;
				case prox7:
					reaction(ROTATION_SPEED, 200, FRIENDLY);
					break;
				case NO_OBSTACLES:
					break;
			}
		}

		else{
			chThdSleep(MS2ST(1000));
		}
		sensor_count = 0;
		if(mv_in_progress){
			chThdSleep(MS2ST(1000));
			mv_in_progress = false;
		}
		else{
			chThdSleep(MS2ST(100));
		}
		obst_move = OBSTACLE_AWAY;
	}
}

void eyes_start(void){
	chThdCreateStatic(waEyes, sizeof(waEyes), HIGHPRIO, Eyes, NULL);
}
