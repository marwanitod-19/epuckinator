#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <moves.h>
#include <motors.h>
#include <time.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <usbcfg.h>
#include <chprintf.h>

#define CMtoSTEP	77.922 // 1 cm/s -> 77.922 step/s
#define NB_MOVES	5
enum moves {pause, stroll_move, look_around, circle_move, jump};
//
//int _gettimeofday( struct timeval *tv, void *tzvp )
//{
//    uint32_t t = 150000000;  // get uptime in nanoseconds
//    tv->tv_sec = t / 1000000000;  // convert to seconds
//    tv->tv_usec = ( t % 1000000000 ) / 1000;  // get remaining microseconds
//    return 0;  // return non-zero for error
//} // end _gettimeofday()
//

int randomizer(int nb_rand){
	int action = pause;
	action = RNG->DR % nb_rand;
	return action;
}

//Casual stroll in a given direction.
void stroll(int left_speed, int right_speed){
	right_motor_set_speed(left_speed*CMtoSTEP);
	left_motor_set_speed(right_speed*CMtoSTEP);
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

//Stops motors and ads a Thread sleep delay in ms.
void pause_until(int ms_time){
	make_pause();
	chThdSleep(MS2ST(ms_time));
}

//Makes an arc like move, random on which side is took.
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
	int direction = randomizer(2);
	rotator(-8);
	chThdSleep(MS2ST(500));
	rotator(8);
	chThdSleep(MS2ST(1000));
	if(direction == 0){
		//Cas o� il part � gauche
		rotator(-8);
		chThdSleep(MS2ST(1000));
	}
	else if(direction == 1){
		//Cas o� il part au milieu
		rotator(-8);
		chThdSleep(MS2ST(500));
	}
	else{
		//Cas o� il part � droite
		chThdSleep(MS2ST(200));
	}
}

//Simulated "jump" by just letting e-cat do jump-like moves.
//Make it look around first and choose where to jump randomly.
void make_jump(void){
	make_look_around();
	stroll(15,15);
	chThdSleep(MS2ST(200));
	pause_until(300);
	stroll(15,15);
	chThdSleep(MS2ST(200));
	pause_until(300);

}

static THD_WORKING_AREA(waMover, 256);
static THD_FUNCTION(Mover, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	//systime_t time;
	uint8_t action = pause;
	while(1){
		//time = chVTGetSystemTime();
		// Condition qui choisit entre rotation et stroll?
		action = randomizer(NB_MOVES);
		chprintf((BaseSequentialStream *)&SD3, "Action == %d \n", action);
		if(action == pause){
			make_pause();
			chThdSleep(MS2ST(1000));
		}
		if(action == stroll_move){
			stroll(5,5);
			chThdSleep(MS2ST(2500));
		}
		if(action == look_around){
			make_look_around();
			pause_until(500);
		}
		if(action == circle_move){
			make_circle(3, 3);
			chThdSleep(MS2ST(2000));
		}
		if(action == jump){
			//make_jump();
			//chThdSleep(MS2ST(2000));
		}
	}

}

void mover_start(void){
	chThdCreateStatic(waMover, sizeof(waMover), NORMALPRIO, Mover, NULL);
}
