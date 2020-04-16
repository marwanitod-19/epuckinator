#include <eyes.h>
#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <sensors/proximity.h>



void view(){


}

void follow(){


}

static THD_WORKING_AREA(waEyes, 256);
static THD_FUNCTION(Eyes, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	//uint8_t sat_sensor[8] = { 0 };
	uint8_t highest_prox = 0;
	uint32_t distance = 0;

	proximity_start();
	calibrate_ir();

	while(1){
		highest_prox = 0;
		for(int i = 0 ; i < 7 ; i++){
			distance = get_prox(i);
			if(distance < 6000){
				//sat_sensor[i] = 1;
				if(i != 0 && get_prox(i-1) > distance){
					highest_prox = i;
				}
			}
		}

		chprintf((BaseSequentialStream *)&SD3, "The Closest distance is at sensor %d and the ",
				"distance is %d \n", highest_prox, get_prox(highest_prox));
	}

	chThdSleep(MS2ST(1000));

}

void eyes_start(void){
	chThdCreateStatic(waEyes, sizeof(waEyes), NORMALPRIO, Eyes, NULL);
}
