#include <Wire.h>
#include "movements.h"

movement s[16];
Transition t0[2] = {{S0_OPEN, 0.05}, {S0_CLOSE, 0.05}};
Transition t1[4] = {{S1_OPEN, 0.05}, {S1_CLOSE, 0.05}, {S1_OPEN, 0.05}, {S1_CLOSE, 0.05}};
bool done[16];

void setup(){
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  controller.begin();
  controller.setOscillatorFrequency(27000000);
  controller.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  
  for(byte servo=0;servo<6;servo++){
    s[servo] = movement(servo);
    done[servo] = false;
  }
  Serial.println("Setup finished");
  delay(1000);
}


void loop(){
  /*next_stato = getstatus();
  
  //.... s0 ... s15
  if(stato == "happy") {
      if(!executed0 && iteration < 3) {
        if(iteration == 0) executed0 = s0.new_position(current_pos, SO_OPEN, 10);
        else if (iteration == 1) executed0 = s0.new_position(current_pos, SO_CLOSE, 10);
        else  executed0 = s0.new_position(current_pos, SO_OPEN, 10);
      else if(iteration < 3){
        iteration++;
        executed0 = !executed0;
      }
      if(!executed1) ...;
      
      if(iteration0 == 3 && iteration1 == 4 ...) stato = next_stato;
    }
  }*/
  /*if(!s[0].get_executed() && s[0].get_iter() < 2) {
    if(s[0].get_iter() == 0) s[0].set_executed(s[0].new_position(s[0].servo_pos, S0_OPEN, 0.05));
    else if(s[0].get_iter() == 1) s[0].set_executed(s[0].new_position(s[0].servo_pos, S0_CLOSE, 0.05));
  }
  else if(s[0].get_iter() < 2) {
    s[0].set_iter(s[0].get_iter() + 1);
    s[0].set_executed(!s[0].get_executed());
  }
  
  if(s[0].get_iter() == 2) {
    s[0].set_iter(0);
    s[0].set_executed(false);
  }*/

 if(!done[0]) done[0] = s[0].sequence_of_moves(t0, 2);
 if(!done[1]) done[1] = s[1].sequence_of_moves(t1, 4);
 if(done[0] && done[1]) {
   done[0] = false;
   done[1] = false;
   Serial.println("SEQUENCE COMPLETED. RESTARTING");
 }

}




