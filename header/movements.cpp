#include "movements.h"

Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);

bool done[16];
movement s[16];
int i;

movement::movement() {

}

movement::movement(unsigned char servo){
    motion_in_progress = false;
    delay_t0 = 10;
    epsilon = 10;
    iter = 0;
    executed = false;
    this->servo = servo;
    if(servo == 0) this->servo_pos = S0_CLOSE;
    else if(servo == 1) this->servo_pos = S1_CLOSE;
    else if(servo == 2) this->servo_pos = S2_MID;
    else if(servo == 3) this->servo_pos = S3_MID;
    else if(servo == 4) this->servo_pos = S4_CLOSE;
    else if(servo == 5) this->servo_pos = S5_CLOSE;
    else if(servo == 6) this->servo_pos = S6_CLOSE;
    else if(servo == 7) this->servo_pos = S7_CLOSE;
  
    controller.setPWM(servo, 0, servo_pos);
}
    
bool movement::new_position(Transition t){ // min = 0 max = 1
    if(!motion_in_progress){
        fin_pos = t.fin_pos;
        velocity = t.velocity;
        if(velocity<0) velocity = 0;
        else if(velocity>1) velocity = 1;
        motion_in_progress = true;
        t0 = millis();
        return false;
    }
    else {
        if(millis()-t0<0) t0 = millis();
        if(millis()-t0>delay_t0){
        servo_pos =  (1-velocity)*servo_pos + velocity*fin_pos;
        controller.setPWM(servo, 0, int(servo_pos));
        }
        if(servo_pos > fin_pos-epsilon && servo_pos < fin_pos+epsilon){
        servo_pos = fin_pos;
        controller.setPWM(servo, 0, fin_pos);
        motion_in_progress = false;
        return true;
        }
        else return false;
    }
}

bool movement::sequence_of_moves(Transition transitions[], int i) {
    if(!executed && iter < i) {
      executed = new_position(transitions[iter]);
      return false;
    }
    else if(iter < i) {
      iter++;
      executed = !executed;
      Serial.print("Transition ");
      Serial.print(iter);
      Serial.print(" completed, servo ");
      Serial.print(servo);
      Serial.print(" at pos: ");
      Serial.println(servo_pos);
      //delay(2000);
      return false;
    }
    else {
      iter = 0;
      executed = false;
      return true;
    }
}

//GETTERS AND SETTERS
bool movement::get_executed() {
    return executed;
}

void movement::set_executed(bool executed) {
    this->executed = executed;
}

float movement::get_servo_pos() {
    return servo_pos;
}

void movement::set_servo_pos(float servo_pos) {
    this->servo_pos = servo_pos;
}

unsigned char movement::get_iter() {
    return iter;
}

void movement::set_iter(unsigned char iter) {
    this->iter = iter;
}

bool movement::get_mip(){
  return motion_in_progress;
}

bool blink() {
  Transition t[5];
  float v = 0.025;

  //blink left eye
  if(!done[0]) {
    t[0] = {S0_OPEN, v}; t[1] = {S0_CLOSE, v}; t[2] = {S0_OPEN, v}; t[3] = {S0_CLOSE, v}; t[4] = {S0_OPEN, v};
    done[0] = s[0].sequence_of_moves(t, 5);
  } 
  if(!done[1]) {
    t[0] = {S1_OPEN, v}; t[1] = {S1_CLOSE, v}; t[2] = {S1_OPEN, v}; t[3] = {S1_CLOSE, v}; t[4] = {S1_OPEN, v};
    done[1] = s[1].sequence_of_moves(t, 5);    
  } 

  //blink right eye
  if(!done[4]) {
    t[0] = {S4_OPEN, v}; t[1] = {S4_CLOSE, v}; t[2] = {S4_OPEN, v}; t[3] = {S4_CLOSE, v}; t[4] = {S4_OPEN, v};
    done[4] = s[4].sequence_of_moves(t, 5);    
  }
  if(!done[5]) {
    t[0] = {S5_OPEN, v}; t[1] = {S5_CLOSE, v}; t[2] = {S5_OPEN, v}; t[3] = {S5_CLOSE, v}; t[4] = {S5_OPEN, v};
    done[5] = s[5].sequence_of_moves(t, 5);    
  }

  if(done[0] && done[1] && done[4] && done[5]) {
    done[0] = false; done[1] = false; done[4] = false; done[5] = false;
    return true;
  }
  else return false;
}

//m = 0 -> closed
//m = 1 -> small open
//m = 2 -> normal open
//m = 3 -> totally open
bool set_eyelids(int m, float v) {
  if(m == 0) {
    if(!done[0]) done[0] = s[0].new_position({S0_CLOSE, v});
    if(!done[1]) done[1] = s[1].new_position({S1_CLOSE, v});
    if(!done[4]) done[4] = s[4].new_position({S4_CLOSE, v});
    if(!done[5]) done[5] = s[5].new_position({S5_CLOSE, v});
  }
  else if(m == 1) {
    if(!done[0]) done[0] = s[0].new_position({190, v});
    if(!done[1]) done[1] = s[1].new_position({150, v});
    if(!done[4]) done[4] = s[4].new_position({240, v});
    if(!done[5]) done[5] = s[5].new_position({300, v}); 
  }
  else if(m == 2) {
    if(!done[0]) done[0] = s[0].new_position({160, v});
    if(!done[1]) done[1] = s[1].new_position({200, v});
    if(!done[4]) done[4] = s[4].new_position({320, v});
    if(!done[5]) done[5] = s[5].new_position({290, v});
    //Serial.println(done[5]);
  }
  else if(m == 3) {
    if(!done[0]) done[0] = s[0].new_position({S0_OPEN, v});
    if(!done[1]) done[1] = s[1].new_position({S1_OPEN, v});
    if(!done[4]) done[4] = s[4].new_position({S4_OPEN, v});
    if(!done[5]) done[5] = s[5].new_position({S5_OPEN, v}); 
  }

  if(done[0] && done[1] && done[4] && done[5]) {
    done[0] = false; done[1] = false; done[4] = false; done[5] = false;
    return true;
  }
  else return false;
}

// m = 0 -> close
// m = 1 -> small open
// m = 2 -> total open
bool set_mouth(int m, float v){

  if(m == 0) {
    if(!done[6]) done[6] = s[6].new_position({S6_CLOSE,v});
    if(!done[7]) done[7] = s[7].new_position({S7_CLOSE,v});
  }
  else if(m == 1) {
    if(!done[6]) done[6] = s[6].new_position({130,v});
    if(!done[7]) done[7] = s[7].new_position({170,v});
  }
  else if(m == 2) {
    if(!done[6]) done[6] = s[6].new_position({S6_OPEN,v});
    if(!done[7]) done[7] = s[7].new_position({S7_OPEN,v});
  }

  if(done[6] && done[7]) {
    done[6] = false; done[7] = false;
    return true;
  }
  else return false;
}

bool do_happy() {
  if(i == 0) if(set_eyelids(2, 0.02)) i++;
  if(i == 1) if(set_mouth(1, 0.02)) i++;
  //Serial.println(i);
  if(i == 2) {
    i = 0;
    return true;
  }
  else return false;  
}

bool undo_happy() {
  if(i == 0) if(set_eyelids(1, 0.01)) i++;
  if(i == 1) if(set_mouth(0, 0.01)) i++;
  //Serial.println(i);
  if(i == 2) {
    i = 0;
    return true;
  }
  else return false;
}
