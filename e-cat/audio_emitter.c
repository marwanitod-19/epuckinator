#include <audio_emitter.h>
#include <audio/play_sound_file.h>
#include <audio/audio_thread.h>


void meow(void){
	stopCurrentSoundFile();
	playSoundFile("/sound/meow.wav", SF_FORCE_CHANGE);
	chThdSleep(MS2ST(2500));
	stopCurrentSoundFile();
}


void purr(void){
	stopCurrentSoundFile();
	playSoundFile("/sound/purr.wav", SF_FORCE_CHANGE);
	chThdSleep(MS2ST(2500));
	stopCurrentSoundFile();
}
