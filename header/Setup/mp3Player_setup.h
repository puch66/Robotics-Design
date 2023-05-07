#ifndef mp3Player_setup_h
#define mp3Player_setup_h

// RXD2 pin = GPIO 16
// TXD2 pin = GPIO 17

// wirings:
// https://github.com/NicolasBrondin/flower-player/raw/master/schema.jpg
// tx pin mp3 -> gpio 17 esp32
// rx pin mp3 -> gpio 16 esp32
// from left to right, with mp3 player sd card side on the left, on the top:
// spk2 mp3 -> speaker negative
// gnd mp3 -> gnd
// spk1 mp3 -> speaker positive
// skip x2 pins
// tx mp3
// rx mp3
// vcc mp3

#include <DFRobotDFPlayerMini.h>
#include <HardwareSerial.h>

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;

void setup_mp3() {
	mySoftwareSerial.begin(9600, SERIAL_8N1, 17, 16);  // speed, type, TX, RX

	while (!myDFPlayer.begin(mySoftwareSerial)) {
		Serial.println("Unable to begin DFPlayer");
	}
	Serial.println("DFPlayer Mini online.");

	myDFPlayer.volume(10);
	myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
	myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
	delay(100);
}

bool play_song(int number) {
	myDFPlayer.playFolder(2, number);
	if (myDFPlayer.available()) {
		uint8_t type = myDFPlayer.readType();
		if (type == DFPlayerPlayFinished) {
			return true;
		}
	}
	return false;
}
#endif