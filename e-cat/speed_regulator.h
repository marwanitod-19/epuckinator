/*
 * speed_regulator.h
 *
 *  Created on: 1 mai 2020
 *      Author: Fahra
 */

#ifndef SPEED_REGULATOR_H_
#define SPEED_REGULATOR_H_

/*
 * 	Function that sets on which side to go for the e-cat to face away from the sound source.
 *
 * 	Input:	phase_FL (front-left microphone phase shift).
 * 	Output:	speed, it is either 3 cm/s or -3 cm/s.
 */
float speed_regulator(float phase_FL);

/*
 * 	Initialization of the SpeedRegulator thread.
 */
void speed_regulator_start(void);


#endif /* SPEED_REGULATOR_H_ */
