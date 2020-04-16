#ifndef MOVES_H_
#define MOVES_H_

//int _gettimeofday( struct timeval *tv, void *tzvp );

int randomizer(int nb_rand);

void make_pause(void);

void rotator(int speed);

//Casual stroll in a given direction
void stroll(int left_speed, int right_speed);

void mover_start(void);

void make_circle(int size, int speed);

void make_look_around(void);

#endif /* MOVES_H_ */
