#include <moves.h>
#include <motors.h>
#include <time.h>
#include <stdint.h>

#define CMtoSTEP	77.922 // 1 cm/s -> 77.922 step/s
#define NB_MOVES	3
enum moves {pause, stroll_move, look_around, circle_move};

int randomizer(void){
	srand(time(NULL));
	int action = pause;
	//random
	action = rand() % NB_MOVES;
	return action;
}

void rotator(){


}



//Casual stroll in a given direction
void stroll(){
	right_motor_set_speed(2*CMtoSTEP);
	left_motor_set_speed(2*CMtoSTEP);
}


void make_pause(){
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}


static THD_WORKING_AREA(waMover, 256);
static THD_FUNCTION(Mover, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	uint8_t action = pause;

	while(1){
		time = chVTGetSystemTime();
		// Condition qui choisit entre rotation et stroll?
		action = randomizer();
		if(action == pause)
			stroll();
		if(action == stroll_move)
			make_pause();
		if(action == look_around)
			printf("look_around");
		if(action == circle_move)
			printf("circle_move");
		chTHDSleep(MS2ST(1000));
	}

}
