#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <stm32f407xx.h>
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>
#include <moves.h>
#include <eyes.h>

#include <audio_processing.h>
#include <communications.h>
#include <arm_math.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

static void timer12_start(void){
    //General Purpose Timer configuration   
    //timer 12 is a 16 bit timer so we can measure time
    //to about 65ms with a 1Mhz counter
    static const GPTConfig gpt12cfg = {
        1000000,        /* 1MHz timer clock in order to measure uS.*/
        NULL,           /* Timer callback.*/
        0,
        0
    };

    gptStart(&GPTD12, &gpt12cfg);
    //let the timer count to max value
    gptStartContinuous(&GPTD12, 0xFFFF);
}

int main(void)
{
	halInit();
	chSysInit();
	mpu_init();

	serial_start();
	usb_start();
	motors_init();
	//mover_start();
	messagebus_init(&bus, &bus_lock, &bus_condvar);
	eyes_start();

	systime_t time;

	//RNG->CR |= RNG_CR_IE;
	RCC->AHB2RSTR &= !RCC_AHB2RSTR_RNGRST;
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	RNG->CR |= RNG_CR_RNGEN;

	//stroll();

	while(1){
		//chprintf((BaseSequentialStream *)&SD3, "Main While \n");
		chprintf((BaseSequentialStream *)&SD3, "Left motor step count is %d \n\n\n", left_motor_get_pos());
		chprintf((BaseSequentialStream *)&SD3, "Right motor step count is %d \n\n\n", right_motor_get_pos());
		time = chVTGetSystemTime();
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}

}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
