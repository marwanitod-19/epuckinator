#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <moves.h>
#include <main.h>
#include <motors.h>
#include <time.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <audio_emitter.h>
#include <eyes.h>
#include <chthreads.h>
#include <audio_processing.h>

#define CMtoSTEP	77.922 // 1 cm/s -> 77.922 step/s
#define NB_MOVES	5

enum moves {pauser, stroll_move, look_around, circle_move, jump};

int randomizer(int nb_rand){
	int action = pauser;
	action = RNG->DR % nb_rand;
	return action;
}

//Casual stroll in a given direction.
void stroll(int right_speed, int left_speed){
	right_motor_set_speed(right_speed*CMtoSTEP);
	left_motor_set_speed(left_speed*CMtoSTEP);
}

//By default, rotates to its right for positive speeds.
void rotator(int speed){
	stroll(speed,-speed);
}

//Stops motors until other function is called.
void make_pause(void){
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

//Stops motors and adds a Thread sleep delay in ms.
void pause_until(int ms_time){
	make_pause();
	chThdSleep(MS2ST(ms_time));
}

//Makes an arc like move, random on which side is taken.
void make_circle(int size, int speed){
	int direction = randomizer(1);
	if (direction == 0){
		right_motor_set_speed(speed*CMtoSTEP);
		left_motor_set_speed((speed + size)*CMtoSTEP);
	}
	else{
		left_motor_set_speed(speed*CMtoSTEP);
		right_motor_set_speed((speed + size)*CMtoSTEP);
	}
}

//Looks at left and right side and chooses randomly between going left, right or straight.
void make_look_around(void){
	int direction = randomizer(2); //choisit entre gauche, droite et milieu
	rotator(-STROLL_SPEED);
	chThdSleep(MS2ST(500));	// Tourne à gauche
	rotator(STROLL_SPEED);
	chThdSleep(MS2ST(1000)); // Tourne à droite
	if(direction == 0){
		//Cas où il part à gauche
		rotator(-STROLL_SPEED);
		chThdSleep(MS2ST(1000));
		stroll(STROLL_SPEED,STROLL_SPEED);
		chThdSleep(MS2ST(1000));
	}
	else if(direction == 1){
		//Cas où il part au milieu
		rotator(-STROLL_SPEED);
		chThdSleep(MS2ST(500));
		stroll(STROLL_SPEED,STROLL_SPEED);
		chThdSleep(MS2ST(1000));
	}
	else{
		//Cas où il part à droite
		chThdSleep(MS2ST(200));
		stroll(STROLL_SPEED,STROLL_SPEED);
		chThdSleep(MS2ST(1000));
	}
}

//Simulated "jump" by just letting e-cat do jump-like moves.
//Make it look around first and choose where to jump randomly.
void make_jump(void){
	make_look_around();
	stroll(3*STROLL_SPEED,3*STROLL_SPEED);
	chThdSleep(MS2ST(200));
	pause_until(300);
	stroll(3*STROLL_SPEED,3*STROLL_SPEED);
	chThdSleep(MS2ST(200));
	pause_until(300);

}

static THD_WORKING_AREA(waMover, 256);
static THD_FUNCTION(Mover, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	uint8_t action = pauser;
	while(1){
		if (!get_speed_process_bool()){
			if(get_obst_move() == 0){
				action = randomizer(NB_MOVES);
				chprintf((BaseSequentialStream *)&SD3, "Action == %d \n", action);
				if(action == pauser && get_obst_move() == 0){
					make_pause();
					chThdSleep(MS2ST(1000));
				}
				if(action == stroll_move && get_obst_move() == 0){
					stroll(5,5);
					chThdSleep(MS2ST(2500));
				}
				if(action == look_around && get_obst_move() == 0){
					make_look_around();
					pause_until(500);
				}
				if(action == circle_move && get_obst_move() == 0){
					make_circle(3, 3);
					chThdSleep(MS2ST(2000));
				}
				if(action == jump){ //Not implemented
					//make_jump();
					//chThdSleep(MS2ST(2000));
				}
			}
			else{
				chThdSleep(MS2ST(2000));
			}
		}
		else{
			chThdSleep(MS2ST(2000));
		}
	}
}


void mover_start(void){
	chThdCreateStatic(waMover, sizeof(waMover), NORMALPRIO, Mover, NULL);
}
