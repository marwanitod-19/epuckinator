/*
 * eyes.h
 *
 *  Created on: 9 Apr 2020
 *      Author: melch
 */

#ifndef EYES_H_
#define EYES_H_

/*
 * 	Makes the e-cat turn at a rot_speed for a time value and then makes it stroll for a delay value.
 *
 * 	Input:	rot_speed: uses the rotator function from moves.c to rotate the e-cat the same way.
 * 			time: used to put the thread to sleep after the rotator function call
 * 			(time variable is used in milliseconds).
 * 			delay: used to put the thread to sleep after the call of the stroll function (from moves.c).
 */
void reaction(int rot_speed, int time, int delay);

/*
 * 	Initialize the Eyes thread.
 */
void eyes_start(void);

/*
 * 	Returns the state of the e-cat when it's avoiding an obstacle.
 *
 * 	Output:	Returns either MOVE_AV_OBSTACLE (move away from obstacles) if it's avoiding an obstacle
 * 	or OBSTACLE_AWAY if the obstacle is far enough from the e-cat.
 */
int get_obst_move(void);

#endif /* EYES_H_ */
