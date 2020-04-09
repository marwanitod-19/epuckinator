#include <moves.h>
#include <motors.h>

#define CMtoSTEP	77.922 // 1 cm/s -> 77.922 step/s

void randomizer(){


}

void rotator(){


}

//Casual stroll in a given direction
void stroll(){
	right_motor_set_speed(2*CMtoSTEP);
	left_motor_set_speed(CMtoSTEP);
}
