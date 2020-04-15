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
#define NB_MOVES	4
enum moves {pause, stroll_move, look_around, circle_move};
//
//int _gettimeofday( struct timeval *tv, void *tzvp )
//{
//    uint32_t t = 150000000;  // get uptime in nanoseconds
//    tv->tv_sec = t / 1000000000;  // convert to seconds
//    tv->tv_usec = ( t % 1000000000 ) / 1000;  // get remaining microseconds
//    return 0;  // return non-zero for error
//} // end _gettimeofday()
//

int randomizer(void){
	int action = pause;
	action = RNG->DR % NB_MOVES;
	return action;
}

//Casual stroll in a given direction
void stroll(int left_speed, int right_speed){
	right_motor_set_speed(left_speed*CMtoSTEP);
	left_motor_set_speed(right_speed*CMtoSTEP);

}

//By default, rotates to its right for positive speeds
void rotator(int speed){
	stroll(speed,-speed);
}


void make_pause(void){
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

void make_circle(int size, int speed){
	right_motor_set_speed(speed*CMtoSTEP);
	left_motor_set_speed((speed + size)*CMtoSTEP);
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
		action = randomizer();
		chprintf((BaseSequentialStream *)&SD3, "Action == %d \n", action);
		if(action == pause)
			make_pause();
		if(action == stroll_move)
			stroll(5,5);
		if(action == look_around)
			rotator(-3);
		if(action == circle_move)
			make_circle(3, 3);
		chThdSleep(MS2ST(1000));
	}

}

void mover_start(void){
	chThdCreateStatic(waMover, sizeof(waMover), NORMALPRIO, Mover, NULL);
}
