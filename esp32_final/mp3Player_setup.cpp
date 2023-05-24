#include "mp3Player_setup.h"

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;

bool init_song = false;

//mp3 initialization
void setup_mp3() {
	mySoftwareSerial.begin(9600, SERIAL_8N1, 17, 16);  // speed, type, TX, RX

	while (!myDFPlayer.begin(mySoftwareSerial)) {
		Serial.println("Unable to begin DFPlayer");
	}
	Serial.println("DFPlayer Mini online.");

	myDFPlayer.volume(30); //volume 20 for presentation, 10 for debug 
	myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
	myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
	delay(10);
}

//plays a song and returns true when it is finished
bool play_song(int number) {
	if (!init_song) {
		myDFPlayer.play(number);
		init_song = true;
	}
	else if (myDFPlayer.available() && myDFPlayer.read() == number) {
		uint8_t type = myDFPlayer.readType();
		if (type == DFPlayerPlayFinished) {
			init_song = false;
			return true;
		}
	}
	return false;
}