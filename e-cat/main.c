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
#include <audio_emitter.h>
#include <audio/play_sound_file.h>
#include <sdio.h>
#include <audio/audio_thread.h>
#include <fat.h>

#include <audio_processing.h>
#include <communications.h>
#include <arm_math.h>
#include <fft.h>
#include <speed_regulator.h>


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

int main(void)
{
	halInit();
	chSysInit();
	mpu_init();

	serial_start();
	usb_start();
	dac_start();
	sdio_start();
	mountSDCard();
	motors_init();
	mover_start();
	speed_regulator_start();

	messagebus_init(&bus, &bus_lock, &bus_condvar);
	eyes_start();
	playSoundFileStart();
	setSoundFileVolume(40);

	//systime_t time;

	//Random number generator register and clock activation
	RCC->AHB2RSTR &= !RCC_AHB2RSTR_RNGRST;
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	RNG->CR |= RNG_CR_RNGEN;

	//temp tab used to store values in complex_float format
	//needed bx doFFT_c
	//                       ATTENTION      -----------> static complex_float temp_tab[FFT_SIZE]; <-------------
	//send_tab is used to save the state of the buffer to send (double buffering)
	//to avoid modifications of the buffer while sending it
	static float send_tab[FFT_SIZE];

	//starts the microphones processing thread.
	//it calls the callback given in parameter when samples are ready
	mic_start(&processAudioData);

	/* Infinite loop. */
	while (1) {
		//time = chVTGetSystemTime();
		//waits until a result must be sent to the computer
		wait_send_to_computer();
		//we copy the buffer to avoid conflicts
		arm_copy_f32(get_audio_buffer_ptr(LEFT_OUTPUT), send_tab, FFT_SIZE);
		SendFloatToComputer((BaseSequentialStream *) &SD3, send_tab, FFT_SIZE);

		//chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}
#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
