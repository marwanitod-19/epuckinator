#include <audio_emitter.h>
#include <audio/play_sound_file.h>
#include <audio/audio_thread.h>


void meow(){
	//dac_power_speaker(1);
	//waitSoundFileHasFinished();
	stopCurrentSoundFile();
	//dac_start();
	setSoundFileVolume(20);
	//dac_play(500);
	playSoundFile("/sound/meow.wav", 0);
	//chThdSleep(MS2ST(3000));
	//playWAVFile("/sound/meow.wav");
	//waitSoundFileHasFinished();

	//dac_stop();

}


void purr(){


}
