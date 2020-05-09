#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <motors.h>
#include <audio/microphone.h>
#include <audio_processing.h>
#include <communications.h>
#include <arm_math.h>
#include <stdbool.h>
#include <fft.h>


//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];
static float micRight_cmplx_input[2 * FFT_SIZE];
static float micFront_cmplx_input[2 * FFT_SIZE];
static float micBack_cmplx_input[2 * FFT_SIZE];
//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];
static float micRight_output[FFT_SIZE];
static float micFront_output[FFT_SIZE];
static float micBack_output[FFT_SIZE];

#define FREQ_MAXERROR	2	//To check if the frequency difference is more than 30hz between two mics
#define PHASE_MAXERROR	0.5	//To check if the phase difference is more than 1 rad between two mics

#define MIN_VALUE_THRESHOLD	10000

#define MIN_FREQ		10	//we don't analyze before this index to not use resources for nothing
#define FREQ_FORWARD	16	//250Hz
#define FREQ_LEFT		19	//296Hz
#define FREQ_RIGHT		23	//359HZ
#define FREQ_BACKWARD	26	//406Hz
#define MAX_FREQ		30	//we don't analyze after this index to not use resources for nothing

#define FREQ_FORWARD_L		(FREQ_FORWARD-1)
#define FREQ_FORWARD_H		(FREQ_FORWARD+1)
#define FREQ_LEFT_L			(FREQ_LEFT-1)
#define FREQ_LEFT_H			(FREQ_LEFT+1)
#define FREQ_RIGHT_L		(FREQ_RIGHT-1)
#define FREQ_RIGHT_H		(FREQ_RIGHT+1)
#define FREQ_BACKWARD_L		(FREQ_BACKWARD-1)
#define FREQ_BACKWARD_H		(FREQ_BACKWARD+1)

static float max_rightnorm = MIN_VALUE_THRESHOLD;
static int16_t max_rightnorm_index = -1;

static float max_leftnorm = MIN_VALUE_THRESHOLD;
static int16_t max_leftnorm_index = -1;

static float max_frontnorm = MIN_VALUE_THRESHOLD;
static int16_t max_frontnorm_index = -1;

static float phase_FL;
static float phase_FR;
static float phase_LR;

static bool	SPEED_REGULATOR_PROCESS = false;

static bool GOAL_REACHED = false;

float get_phase_FL(void){
	return phase_FL;
}

float get_phase_FR(void){
	return phase_FR;
}

float get_phase_LR(void){
	return phase_LR;
}

bool get_speed_process_bool(void){
	return SPEED_REGULATOR_PROCESS;
}

bool get_goal_bool(void){
	return GOAL_REACHED;
}

void set_goal_bool(bool reset){
	GOAL_REACHED = reset;
}

void phase_shift(void){

	float phase_L;
	float phase_R;
	float phase_F;

	//resets the max norm values for next data.
	max_rightnorm = MIN_VALUE_THRESHOLD;
	max_rightnorm_index = -1;
	max_frontnorm = MIN_VALUE_THRESHOLD;
	max_frontnorm_index = -1;
	max_leftnorm = MIN_VALUE_THRESHOLD;
	max_leftnorm_index = -1;

	//search for the highest peak
	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
		if(micRight_output[i] > max_rightnorm){
			max_rightnorm = micRight_output[i];
			max_rightnorm_index = i;
		}
		if(micFront_output[i] > max_frontnorm){
			max_frontnorm = micFront_output[i];
			max_frontnorm_index = i;
		}
		if(micLeft_output[i] > max_leftnorm){
			max_leftnorm = micLeft_output[i];
			max_leftnorm_index = i;
		}
	}

	// Condition to check if the max amplitude frequency is similar between front-left and front-right microphones
	if ((max_rightnorm_index > max_frontnorm_index + FREQ_MAXERROR) || (max_rightnorm_index < max_frontnorm_index - FREQ_MAXERROR)){
		SPEED_REGULATOR_PROCESS = false;
	}

	else if ((max_leftnorm_index > max_frontnorm_index + FREQ_MAXERROR) || (max_leftnorm_index < max_frontnorm_index - FREQ_MAXERROR)){
		SPEED_REGULATOR_PROCESS = false;
	}
	else{
		SPEED_REGULATOR_PROCESS = true;
	}

	//phase shift processing

	phase_L = atan2(micLeft_cmplx_input[2*max_leftnorm_index + 1], micLeft_cmplx_input[2*max_leftnorm_index]);		//Left mic phase
	phase_R = atan2(micRight_cmplx_input[2*max_rightnorm_index + 1], micRight_cmplx_input[2*max_rightnorm_index]);	//Right mic phase
	phase_F = atan2(micFront_cmplx_input[2*max_frontnorm_index + 1], micFront_cmplx_input[2*max_frontnorm_index]);	//Front mic phase

	phase_FR = phase_F - phase_R;
	phase_FL = phase_F - phase_L;
	phase_LR = phase_L - phase_R;

	//Condition to let the e-cat only use the frequencies around 250 Hz. (+- 30Hz).
	if(max_frontnorm_index <= FREQ_FORWARD_L || max_frontnorm_index >= FREQ_FORWARD_H){
		SPEED_REGULATOR_PROCESS = false;
	}
	else{
		//Condition to filter out the data with incoherent phases between front-right microphones.
		if ((phase_FR > PHASE_MAXERROR) || (phase_FR < -PHASE_MAXERROR)){
			SPEED_REGULATOR_PROCESS = false;
		}

		//Condition to filter out the data with incoherent phases between front-left microphones.
		else if ((phase_FL > PHASE_MAXERROR) || (phase_FL < -PHASE_MAXERROR)){
			SPEED_REGULATOR_PROCESS = false;
		}

		//Condition to filter out the data with incoherent phases between left-right microphones.
		else if ((phase_LR > PHASE_MAXERROR) || (phase_LR < -PHASE_MAXERROR)){
			SPEED_REGULATOR_PROCESS = false;
		}

		else{
			//Condition to check if we reached a goal with a threshold margin to avoid oscillations around the goal angle.
			if((phase_LR < (GOAL_ANGLE + ERROR_THRESHOLD)) && (phase_LR > (GOAL_ANGLE - ERROR_THRESHOLD))){
				SPEED_REGULATOR_PROCESS = false;
				GOAL_REACHED = true;
			}
			else{
				SPEED_REGULATOR_PROCESS = true;
				GOAL_REACHED = false;
			}
		}
	}

}


/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*/
void processAudioData(int16_t *data, uint16_t num_samples){

	/*
	*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs.
	*
	*/
	static uint16_t nb_samples = 0;
	static uint8_t mustSend = 0;

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4){
		//construct an array of complex numbers. Put 0 to the imaginary part
		micRight_cmplx_input[nb_samples] = (float)data[i + MIC_RIGHT];
		micLeft_cmplx_input[nb_samples] = (float)data[i + MIC_LEFT];
		micBack_cmplx_input[nb_samples] = (float)data[i + MIC_BACK];
		micFront_cmplx_input[nb_samples] = (float)data[i + MIC_FRONT];

		nb_samples++;

		micRight_cmplx_input[nb_samples] = 0;
		micLeft_cmplx_input[nb_samples] = 0;
		micBack_cmplx_input[nb_samples] = 0;
		micFront_cmplx_input[nb_samples] = 0;

		nb_samples++;

		//stop when buffer is full
		if(nb_samples >= (2 * FFT_SIZE)){
			break;
		}
	}


	if(nb_samples >= (2 * FFT_SIZE)){
		/*	FFT proccessing
		*
		*	This FFT function stores the results in the input buffer given.
		*	This is an "In Place" function.
		*/
		doFFT_optimized(FFT_SIZE, micRight_cmplx_input);
		doFFT_optimized(FFT_SIZE, micLeft_cmplx_input);
		doFFT_optimized(FFT_SIZE, micFront_cmplx_input);
		doFFT_optimized(FFT_SIZE, micBack_cmplx_input);
		/*	Magnitude processing
		*
		*	Computes the magnitude of the complex numbers and
		*	stores them in a buffer of FFT_SIZE because it only contains
		*	real numbers.
		*
		*/
		arm_cmplx_mag_f32(micRight_cmplx_input, micRight_output, FFT_SIZE);
		arm_cmplx_mag_f32(micLeft_cmplx_input, micLeft_output, FFT_SIZE);
		arm_cmplx_mag_f32(micFront_cmplx_input, micFront_output, FFT_SIZE);
		arm_cmplx_mag_f32(micBack_cmplx_input, micBack_output, FFT_SIZE);

		//sends only one FFT result over 10 for 1 mic to not flood the computer
		//sends to UART3
		if(mustSend > 8){
			//signals to send the result to the computer
			chBSemSignal(&sendToComputer_sem);
			mustSend = 0;
		}
		nb_samples = 0;
		mustSend++;

		phase_shift();
	}
}

void wait_send_to_computer(void){
	chBSemWait(&sendToComputer_sem);
}

float* get_audio_buffer_ptr(BUFFER_NAME_t name){
	if(name == LEFT_CMPLX_INPUT){
		return micLeft_cmplx_input;
	}
	else if (name == RIGHT_CMPLX_INPUT){
		return micRight_cmplx_input;
	}
	else if (name == FRONT_CMPLX_INPUT){
		return micFront_cmplx_input;
	}
	else if (name == BACK_CMPLX_INPUT){
		return micBack_cmplx_input;
	}
	else if (name == LEFT_OUTPUT){
		return micLeft_output;
	}
	else if (name == RIGHT_OUTPUT){
		return micRight_output;
	}
	else if (name == FRONT_OUTPUT){
		return micFront_output;
	}
	else if (name == BACK_OUTPUT){
		return micBack_output;
	}
	else{
		return NULL;
	}
}
