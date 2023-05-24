#include "movements.h"
#include "mp3Player_setup.h"

TaskHandle_t Task1;
TaskHandle_t Task2;

State state = IDLE;
State next_state = IDLE;

//handle emotion exchanging
long randNumber;
Characters towards = ALL;
Characters next_towards = ALL;

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer

  setup_servos();

  delay(100);

  //setup mp3 player
  setup_mp3();

  //setup dual core tasks
  xTaskCreatePinnedToCore(main_loop, "main", 10000, NULL, 1, &Task1, 0);                 
  delay(500); 

  xTaskCreatePinnedToCore(network_loop, "network", 10000, NULL, 1, &Task2, 1);
  delay(500);

  //set seed for random number generation
  randomSeed(analogRead(0));
}

void main_loop(void * pvParameters) {
  while(true) {
    
    if(state == IDLE) if(reset_position()) if(do_idle(towards)) state = HAPPY;

    if(state == HAPPY) if(reset_position()) if(do_happy(towards)) state = SAD;

    if(state == SAD) if(reset_position()) if(do_sad(towards)) state = ANGRY;

    if(state == ANGRY) if(reset_position()) if(do_angry(towards)) state = SHOCKED;

    if(state == SHOCKED) if(reset_position()) if(do_shocked(towards)) state = CAUTIOUS;

    if(state == CAUTIOUS) if(reset_position()) if(do_doubtful(towards)) state = ANNOYED; 

    if(state == ANNOYED) if(reset_position()) if(do_annoyed(towards)) state = IDLE;

    delay(10);  
  }
}

void network_loop(void * pvParameters) {
  // Handle connection and receive from WiFi (updating state)
  while(true) {

    if(Serial.available()) {  //debug
      String send = Serial.readString();
      Serial.println(send);
    }
    
    delay(10);  
  }
}

void loop() {

}

void hard_reset() {
  for(int i=0; i<16; i++) {
    s[i].set_done(false);
    s[i].set_mip(false);
    done[i] = false;    
  }
  i = 0;
  repeat_actions = 0;
  init_song = false;
  myDFPlayer.pause();  //pause the mp3
}
