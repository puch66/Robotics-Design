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


extern DFRobotDFPlayerMini myDFPlayer;

extern bool init_song;

void setup_mp3();
bool play_song(int number);
#endif