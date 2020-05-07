#ifndef MOVES_H_
#define MOVES_H_

//int _gettimeofday( struct timeval *tv, void *tzvp );


/*
 * 	Returns a random number between 0 and nb_rand. Based on the true random number generator module included in the
 * 	e-puck.
 *
 * 	Input:	nb_rand: upper limit of possible random numbers to get.
 *
 * 	Output:	random number generated between [0, nb_rand].
 */
int randomizer(int nb_rand);

/*
 * 	Function that sets at 0 the speed of both motors.
 */
void make_pause(void);

/*
 * 	Does a make_pause() and puts the thread to sleep for ms_time.
 *
 * 	Input:	ms_time: time to put the thread to sleep in milliseconds after a make_pause()
 */
void pause_until(int ms_time);

/*
 * 	Function that makes the e-cat rotate, a positive speed will make it rotate CW and
 * 	a negative speed CCW
 *
 * 	Input:	speed: speed at which the e-cat will rotate
 */
void rotator(int speed);

/*
 * 	Function  making the e-cat go straight at a given speed.
 *
 * 	Input : left_speed:	applies the speed in cm/s to the right wheel.
 * 			right_speed:applies the speed in cm/s to the right wheel.
 */
void stroll(int right_speed, int left_speed);

/*
 * 	Function initializing the Mover thread.
 */
void mover_start(void);

/*
 * 	Function making the e-cat going in circle for a set period of time (2 seconds)
 *
 * 	Input :	speed:	set the base speed of both wheels in cm/s.
 * 			size: 	set the delta in speed needed to make an arc like movement.
 */
void make_circle(int size, int speed);

/*
 * 	Function that makes the e-cat move like it is looking at its left and right side
 * 	and makes it decide if it wants to go either to the left, right or go straight ahead.
 * 	Uses the randomizer function to make the decision.
 */
void make_look_around(void);

/*
 * 	Function that makes the e-cat do make_look_around move and then a small jump-like move.
 */
void make_jump(void);

/*
 * 	Function that lets us communicate which of the previous moves is selected to other modules/threads.
 *
 * 	Return:	int with the preset_move number.
 */
int get_preset_move(void);

#endif /* MOVES_H_ */
