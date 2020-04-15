#ifndef MOVES_H_
#define MOVES_H_

//int _gettimeofday( struct timeval *tv, void *tzvp );

//int randomizer(void);

void make_pause(void);

void rotator(int speed);

//Casual stroll in a given direction
void stroll(int left_speed, int right_speed);

void mover_start(void);

void make_circle(int size, int speed);



#endif /* MOVES_H_ */
