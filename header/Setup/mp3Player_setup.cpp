#include mp3Player_setup.h

void play_song(int number){
	if (myDFPlayer.available()) {
		myDFPlayer.playFolder(2, number);
	}
}