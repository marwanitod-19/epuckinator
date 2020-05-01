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

void make_jump(void);

thread_t* get_thd_ptr(void);

int get_preset_move(void);

#endif /* MOVES_H_ */
