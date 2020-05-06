#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H


#define FFT_SIZE 	1024

typedef enum {
	//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
	LEFT_CMPLX_INPUT = 0,
	RIGHT_CMPLX_INPUT,
	FRONT_CMPLX_INPUT,
	BACK_CMPLX_INPUT,
	//Arrays containing the computed magnitude of the complex numbers
	LEFT_OUTPUT,
	RIGHT_OUTPUT,
	FRONT_OUTPUT,
	BACK_OUTPUT
} BUFFER_NAME_t;

/*
 * 	Returns the phase shift between the Front and Left microphone.
 *
 * 	Output:	Front and Left microphone phase shift.
 */
float get_phase_FL(void);

/*
 * 	Returns the phase shift between the Front and Right microphone.
 *
 * 	Output:	Front and Right microphone phase shift.
 */
float get_phase_FR(void);

/*
 * 	Returns the speed process boolean, used to know if the thread SpeedProcess
 * 	should process the data it received or not.
 *
 * 	Output:	speed process boolean.
 */
bool get_speed_process_bool(void);

/*
 * 	Return true/false if the goal angle is reached or not, used to know if e-cat should go away or not yet.
 *
 * 	Output:	goal reached boolean.
 */
bool get_goal_bool(void);

/*
 * 	Function that computes the phase shifts between the 3 microphones used. Uses the static variables:
 * 					max_rightnorm, max_rightnorm_index, max_leftnorm, max_leftnorm_index, max_frontnorm,
 * 					max_frontnorm_index, phase_FL, phase_FR.
 *
 * 	and arrays : 	micLeft_cmplx_input, micRight_cmplx_input, micFront_cmplx_input, micBack_cmplx_input,
 * 					micLeft_output, micRight_output, micFront_output, micBack_output.
 */

bool set_goal_bool(bool reset);

void phase_shift(void);

/*
 * 	Original function from TP5 used to process the audio data.
 *
 *	Callback called when the demodulation of the four microphones is done.
 *	We get 160 samples per mic every 10ms (16kHz)
 *
 *	params :
 *	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
 *							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
 *	uint16_t num_samples	Tells how many data we get in total (should always be 640)
 */
void processAudioData(int16_t *data, uint16_t num_samples);

/*
*	put the invoking thread into sleep until it can process the audio data
*/
void wait_send_to_computer(void);

/*
*	Returns the pointer to the BUFFER_NAME_t buffer asked
*/
float* get_audio_buffer_ptr(BUFFER_NAME_t name);

#endif /* AUDIO_PROCESSING_H */
