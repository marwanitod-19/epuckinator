#include <audio_emitter.h>
#include <audio/play_sound_file.h>
#include <audio/audio_thread.h>


void meow(){
	//dac_power_speaker(1);

	stopCurrentSoundFile();
	//waitSoundFileHasFinished();

	//dac_start();
	//setSoundFileVolume(20);
	//dac_play(500);
	playSoundFile("/sound/meow.wav", SF_FORCE_CHANGE);
	chThdSleep(MS2ST(2500));
	stopCurrentSoundFile();
	//playWAVFile("/sound/meow.wav");

	//dac_stop();

}


void purr(){
	stopCurrentSoundFile();
	//waitSoundFileHasFinished();

	//dac_start();
	//setSoundFileVolume(20);
	//dac_play(500);
	playSoundFile("/sound/purr.wav", SF_FORCE_CHANGE);
	chThdSleep(MS2ST(2500));
	stopCurrentSoundFile();
}
