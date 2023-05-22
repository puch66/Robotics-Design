#include "movements.h"

Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);

bool done[16];
movement s[16];
int i = 0;
int repeat_actions = 0;

movement::movement() {

}

movement::movement(unsigned char servo, unsigned char delay_t0){
    motion_in_progress = false;
    this->delay_t0 = delay_t0;
    epsilon = 10;
    this->done = false;
    this->servo = servo;
    if (servo == 0) this->servo_pos = S0_CLOSE;
    else if (servo == 1) this->servo_pos = S1_CLOSE;
    else if (servo == 2) this->servo_pos = S2_MID;
    else if (servo == 3) this->servo_pos = S3_MID;
    else if (servo == 4) this->servo_pos = S4_CLOSE;
    else if (servo == 5) this->servo_pos = S5_CLOSE;
    else if (servo == 6) this->servo_pos = S6_CLOSE;
    else if (servo == 7) this->servo_pos = S7_CLOSE;
    else if (servo == 8) this->servo_pos = S8_MID;
    else if (servo == 9) this->servo_pos = S9_CLOSE;
    else if(servo == 10) this->servo_pos = S10_MID;
    else if (servo == 11) this->servo_pos = S11_MID;
    else this->servo_pos = 0;
  
    controller.setPWM(servo, 0, servo_pos);
}
    
void movement::new_position(Transition t){ // min = 0 max = 1
    if(!motion_in_progress){
        fin_pos = t.fin_pos;
        velocity = t.velocity;
        if(velocity<0) velocity = 0;
        else if(velocity>1) velocity = 1;
        if (servo_pos > fin_pos - epsilon && servo_pos < fin_pos + epsilon) {
            done = true;
            return;
        }
        motion_in_progress = true;
        t0 = millis();
        done = false;
    }
    else {
        if(millis()-t0<0) t0 = millis();
        if(millis()-t0>delay_t0){
        servo_pos =  (1-velocity)*servo_pos + velocity*fin_pos;
        controller.setPWM(servo, 0, int(servo_pos));
        }
        if (servo_pos > fin_pos - epsilon && servo_pos < fin_pos + epsilon) {
            servo_pos = fin_pos;
            controller.setPWM(servo, 0, fin_pos);
            motion_in_progress = false;
            done = true;
        }
        else done = false;
    }
}

bool movement::wait(long int delay_t0) {
    if (!motion_in_progress) {
        t0 = millis();
        motion_in_progress = true;
        return false;
    }
    else {
        if (millis() - t0 < 0) t0 = millis();
        if (millis() - t0 > delay_t0) {
            motion_in_progress = false;
            return true;
        }
        else return false;
    }
    return false;
}

//GETTERS AND SETTERS
float movement::get_servo_pos() {
    return servo_pos;
}

void movement::set_servo_pos(float servo_pos) {
    this->servo_pos = servo_pos;
}

bool movement::get_mip(){
  return motion_in_progress;
}

void movement::set_mip(bool mip) {
    motion_in_progress = mip;
}

bool movement::get_done() {
    return done;
}

void movement::set_done(bool done) {
    this->done = done;
}

void setup_servos() {
    Wire.begin(I2C_SDA, I2C_SCL);
    controller.begin();
    controller.setOscillatorFrequency(27000000);
    controller.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
    delay(10);
    Serial.println('\n');

    for (byte servo = 0; servo < 16; servo++) {
        s[servo] = movement(servo, delay_t0[servo]);
        done[servo] = false;
    }
    delay(100);
}

//m = 0 -> closed
//m = 1 -> small open
//m = 2 -> normal open
//m = 3 -> totally open
//m = 4 -> sad position - open
//m = 5 -> sad position - close
//m = 6 -> angry
bool set_eyelids(int m, float v = 0.05) {
  if(m == 0) {
    s[0].new_position({S0_CLOSE, v});
    s[1].new_position({S1_CLOSE, v});
    s[4].new_position({S4_CLOSE, v});
    s[5].new_position({S5_CLOSE, v});
  }
  else if(m == 1) {
    s[0].new_position({190, v});
    s[1].new_position({150, v});
    s[4].new_position({240, v});
    s[5].new_position({300, v}); 
  }
  else if(m == 2) {
    s[0].new_position({160, v});
    s[1].new_position({200, v});
    s[4].new_position({280, v});
    s[5].new_position({290, v});
  }
  else if(m == 3) {
    s[0].new_position({S0_OPEN, v});
    s[1].new_position({S1_OPEN, v});
    s[4].new_position({S4_OPEN, v});
    s[5].new_position({S5_OPEN, v}); 
  }
  else if (m == 4) {
      s[0].new_position({ S0_CLOSE, v });
      s[4].new_position({ S4_CLOSE, v });
      s[1].new_position({ S1_OPEN, v });
      s[5].new_position({ S5_OPEN, v });
  }
  else if (m == 5) {
      s[0].new_position({ S0_CLOSE, v });
      s[4].new_position({ S4_CLOSE, v });
      s[1].new_position({ 150, v });
      s[5].new_position({ 300, v });
  }
  else if (m == 6) {
      s[0].new_position({ S0_ANGRY, v });
      s[4].new_position({ S4_ANGRY, v });
      s[1].new_position({ S1_ANGRY, v });
      s[5].new_position({ S5_ANGRY, v });
  }
  else if (m == 7) {
      s[0].new_position({ S0_ANGRY, v });
      s[1].new_position({ S1_ANGRY, v });
      s[4].new_position({ S4_OPEN, v });
      s[5].new_position({ S5_OPEN, v });
  }


  if(s[0].get_done() && s[1].get_done() && s[4].get_done() && s[5].get_done()) {
      s[0].set_done(false); s[1].set_done(false); s[4].set_done(false); s[5].set_done(false);
      return true;
  }
  else return false;
}

// m = 0 -> close
// m = 1 -> small open
// m = 2 -> total open
// m = 3 -> angry
bool set_mouth(int m, float v = 0.01){
  if(m == 0) {
      s[6].new_position({ S6_CLOSE,v });
      s[7].new_position({ S7_CLOSE,v });
  }
  else if(m == 1) {
     s[6].new_position({ S6_MID,v });
     s[7].new_position({ S7_MID,v });
  }
  else if(m == 2) {
    s[6].new_position({ S6_OPEN,v });
    s[7].new_position({ S7_OPEN,v });
  }
  else if (m == 3) {
      s[6].new_position({ S6_ANGRY,v });
      s[7].new_position({ S7_ANGRY,v });
  }

  if (s[6].get_done() && s[7].get_done()) {
      s[6].set_done(false); s[7].set_done(false);
      return true;
  }
  else return false;
}

// m = 0 -> front
// m = 1 -> mid position
// m = 2 -> back
// m = 3 -> sad
// m = 4 -> angry
// m = 5 -> doubtful
// m = 6 -> happy
bool set_chest(int m, float v = 0.01) {
    if (m == 0) {
        s[8].new_position({ S8_FRONT,v });
    }
    else if (m == 1) {
        s[8].new_position({ S8_MID,v });
    }
    else if (m == 2) {
        s[8].new_position({ S8_BACK,v });
    }
    else if (m == 3) {
        s[8].new_position({ S8_SAD,v });
    }
    else if (m == 4) {
        s[8].new_position({ S8_ANGRY,v });
    }
    else if (m == 5) {
        s[8].new_position({ S8_DOUBTFUL,v });
    }
    else if (m == 6) {
        s[8].new_position({ S8_HAPPY,v });
    }

    if (s[8].get_done()) {
        s[8].set_done(false);
        return true;
    }
    else return false;
}

// m = 0 -> left
// m = 1 -> mid position
// m = 2 -> right
// m = 3 -> idle_left
// m = 4 -> idle_right
// m = 5 -> doubtful_left
// m = 6 -> doubtful_right
bool set_body_rotation(int m, float v = 0.05) {
    if (m == 0) {
        s[10].new_position({ S10_LEFT,v });
    }
    else if (m == 1) {
        s[10].new_position({ S10_MID,v });
    }
    else if (m == 2) {
        s[10].new_position({ S10_RIGHT,v });
    }
    else if (m == 3) {
        s[10].new_position({ S10_IDLE_LEFT,v });
    }
    else if (m == 4) {
        s[10].new_position({ S10_IDLE_RIGHT,v });
    }
    else if (m == 5) {
        s[10].new_position({ S10_DOUBTFUL_LEFT,v });
    }
    else if (m == 6) {
        s[10].new_position({ S10_DOUBTFUL_RIGHT,v });
    }

    if (s[10].get_done()) {
        s[10].set_done(false);
        return true;
    }
    else return false;
}

bool set_body_rotation(int m, float v, Characters character) {
    int index = (char)character - '0';
    int S10_bit_pos = 0;
    if (m == 0) {
        S10_bit_pos = character_pos_LEFT[index];
    }
    else if (m == 1) {
        S10_bit_pos = character_pos_MID[index];
    }
    else if (m == 2) {
        S10_bit_pos = character_pos_RIGHT[index];
    }
    s[10].new_position({ S10_bit_pos,v });
    if (s[10].get_done()) {
        s[10].set_done(false);
        return true;
    }
    else return false;
}

// m = 0 -> up
// m = 1 -> mid position
// m = 2 -> down
// m = 3 -> sad
// m = 4 -> angry
// m = 5 -> happy
bool set_neck(int m, float v = 0.02) {
    if (m == 0) {
        s[11].new_position({ S11_UP,v });
    }
    else if (m == 1) {
        s[11].new_position({ S11_MID,v });
    }
    else if (m == 2) {
        s[11].new_position({ S11_DOWN,v });
    }
    else if (m == 3) {
        s[11].new_position({ S11_SAD,v });
    }
    else if (m == 4) {
        s[11].new_position({ S11_ANGRY,v });
    }
    else if (m == 5) {
        s[11].new_position({ S11_HAPPY,v });
    }

    if (s[11].get_done()) {
        s[11].set_done(false);
        return true;
    }
    else return false;
}

// m = 0 -> up
// m = 1 -> mid position
// m = 2 -> down
bool roll_eyes(int m, float v = 1.0) {
    if (m == 0) {
        s[2].new_position({ S2_UP,v });
    }
    else if (m == 1) {
        s[2].new_position({ S2_MID,v });
    }
    else if (m == 2) {
        s[2].new_position({ S2_DOWN,v });
    }

    if (s[2].get_done()) {
        s[2].set_done(false);
        return true;
    }
    else return false;
}

// m = 0 -> left
// m = 1 -> mid position
// m = 2 -> right
bool move_eyes(int m, float v = 0.2) {
    if (m == 0) {
        s[3].new_position({ S3_LEFT,v });
    }
    else if (m == 1) {
        s[3].new_position({ S3_MID,v });
    }
    else if (m == 2) {
        s[3].new_position({ S3_RIGHT,v });
    }

    if (s[3].get_done()) {
        s[3].set_done(false);
        return true;
    }
    else return false;
}

// m = 0 -> close
// m = 1 -> small close
// m = 2 -> small open
// m = 3 -> open
bool set_tail(int m, float v = 0.5) {
    if (m == 0) {
        s[9].new_position({ S9_CLOSE,v });
    }
    else if (m == 1) {
        s[9].new_position({ S9_SMALL_CLOSE,v });
    }
    else if (m == 2) {
        s[9].new_position({ S9_SMALL_OPEN,v });
    }
    else if (m == 3) {
        s[9].new_position({ S9_OPEN,v });
    }

    if (s[9].get_done()) {
        s[9].set_done(false);
        return true;
    }
    else return false;
}


//state functions
//best velocities for each function:
// set_eyelids = 0.05
// set_mouth = 0.15
// set_chest = 0.1
// set_body_rotation = 0.2
// set_neck = ???
// move_eyes = 0.2
// roll_eyes = 1.0
// set_tail = 0.5
bool reset_position() {
    if (!done[0]) done[0] = set_eyelids(2);
    if (!done[1]) done[1] = set_mouth(0,0.15);
    if (!done[2]) done[2] = set_chest(1, 0.075);
    if (!done[3]) done[3] = set_body_rotation(1, 0.2);
    if (!done[4]) done[4] = set_neck(1, 0.015);
    if (!done[5]) done[5] = move_eyes(1);
    if (!done[6]) done[6] = roll_eyes(1);
    if (!done[7]) done[7] = set_tail(1);

    if (done[0] && done[1] && done[2] && done[3] && done[4] && done[5] && done[6] && done[7]) {
        done[0] = false; done[1] = false; done[2] = false; done[3] = false; done[4] = false; done[5] = false; done[6] = false; done[7] = false;
        return true;
    }
    else return false;
}

bool do_idle(Characters c) {
    return true;
    //if (i == 0) if (set_body_rotation(1, 0.01, c)) i++;
    if (i == 0) if (set_eyelids(0,0.2)) i++;
    if (i == 1) if (set_eyelids(2,0.2)) i++;
    if (i == 2) if (set_eyelids(0,0.2)) i++;
    if (i == 3) if (set_eyelids(2,0.2)) i++;
    if (i == 4) {
        if (!done[0]) done[0] = set_body_rotation(3, 0.1);
        if (!done[1]) done[1] = set_tail(1);
        if (!done[2]) done[2] = s[14].wait(750);
        if (done[0] && done[1] && done[2]) {
            done[0] = false; done[1] = false; done[2];
            i++;
        }
    }
    if (i == 5) {
        if (!done[0]) done[0] = set_body_rotation(4, 0.1);
        if (!done[1]) done[1] = set_tail(2);
        if (!done[2]) done[2] = s[14].wait(750);
        if (done[0] && done[1] && done[2]) {
            done[0] = false; done[1] = false; done[2];
            i++;
        }
    }
    if (i == 6) {
        i = 0;
        return true;
    }
    else return false;
}

bool do_happy(Characters c) {
  if(i == 0) if (set_body_rotation(1, 0.2, c)) i++;
  if(i == 1) if(set_eyelids(2, 0.02)) i++;
  if (i == 2) if (set_neck(5)) i++;
  if(i == 3) if(roll_eyes(0)) i++;
  if (i == 4) {
      if (!done[0]) done[0] = set_chest(6, 0.1);
      if (!done[1]) done[1] = set_tail(2);
      if (!done[2]) done[2] = set_mouth(1, 0.15);
      if (done[0] && done[1] && done[2]) {
          done[0] = false; done[1] = false; done[2] = false;
          i++;
      }
  }
  if (i == 5) {
      if (!done[0]) done[0] = set_chest(1, 0.1);
      if (!done[1]) done[1] = set_tail(1);
      if (!done[2]) done[2] = set_mouth(0, 0.15);
      if (done[0] && done[1] && done[2]) {
          done[0] = false; done[1] = false; done[2] = false;
          i++;
      }
  }
  if (i == 6 && repeat_actions < 9) {
      repeat_actions++;
      i = 4;
  }
  if(i == 6 && repeat_actions == 9) {
    i = 0;
    repeat_actions = 0;
    return true;
  }
  else return false;  
}

bool do_sad(Characters c) {
    if (i == 0) if(set_body_rotation(1, 0.2, c)) i++;
    if (i == 1) {
        if (!done[0]) done[0] = set_eyelids(1);
        if (!done[1]) done[1] = set_tail(0);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 2) {
        if (!done[0]) done[0] = set_eyelids(4);
        if (!done[1]) done[1] = roll_eyes(2);
        if (!done[2]) done[2] = set_chest(3);
        if (!done[3]) done[3] = set_neck(3,0.01);
        if (done[0] && done[1] && done[2] && done[3] ) {
            done[0] = false; done[1] = false; done[2] = false; done[3] = false;
            i++;
        }
    }
    if (i == 3) {
        if (!done[0]) done[0] = set_eyelids(5);
        if (!done[1]) done[1] = roll_eyes(1);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 4) {
        if (!done[0]) done[0] = set_eyelids(4);
        if (!done[1]) done[1] = roll_eyes(2);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 5) {
        if (!done[0]) done[0] = set_eyelids(5);
        if (!done[1]) done[1] = roll_eyes(1);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 6) {
        if (!done[0]) done[0] = set_eyelids(4);
        if (!done[1]) done[1] = roll_eyes(2);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 7) {
        if (!done[0]) done[0] = set_body_rotation(2, 0.075, c);
        if (!done[1]) done[1] = s[14].wait(2000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }

    if (i == 8) {
        i = 0;
        return true;
    }
    else return false;
}

bool do_angry(Characters c) {
    if (i == 0) if (set_body_rotation(1, 0.2, c)) i++;
    if (i == 1) {
        if (!done[0]) done[0] = set_eyelids(6);
        if (!done[2]) done[2] = set_tail(3);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1] && done[2]) {
            done[0] = false; done[1] = false; done[2] = false;
            i++;
        }
    }
    if (i == 2) {
        if (!done[0]) done[0] = set_eyelids(6);
        if (!done[1]) done[1] = set_mouth(1, 0.1);
        if (!done[2]) done[2] = set_neck(4, 0.1);
        if (!done[3]) done[3] = set_chest(4, 0.1);
        if (done[0] && done[1] && done[2] && done[3]) {
            done[0] = false; done[1] = false; done[2] = false; done[3] = false;
            i++;
        }
    }
    if (i == 3) {
        if (!done[0]) done[0] = set_body_rotation(0, 0.2, c);
        if (!done[1]) done[1] = set_mouth(2, 0.1);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 4) {
        if (!done[0]) done[0] = set_body_rotation(1, 0.2, c);
        if (!done[1]) done[1] = set_mouth(3, 0.1);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 5) {
        if (!done[0]) done[0] = set_body_rotation(2, 0.2, c);
        if (!done[1]) done[1] = set_mouth(2, 0.1);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 6) {
        if (!done[0]) done[0] = set_body_rotation(1, 0.2, c);
        if (!done[1]) done[1] = set_mouth(3, 0.1);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 7 && repeat_actions < 4) {
        repeat_actions++;
        i = 3;
    }
    
    if (i == 7 && repeat_actions == 4) {
        i = 0;
        repeat_actions = 0;
        return true;
    }
    else return false;
}

bool do_shocked(Characters c) {
    if (i == 0) if (set_body_rotation(1, 0.2, c)) i++;
    if (i == 1) if (set_tail(2)) i++;
    if (i == 2) {
        if (!done[0]) done[0] = set_eyelids(3);
        if (!done[1]) done[1] = set_mouth(2, 0.15);
        if (!done[2]) done[2] = s[14].wait(1500);
        if (done[0] && done[1] && done[2]) {
            done[0] = false; done[1] = false; done[2] = false;
            i++;
        }
    }
    if (i == 3) {
        if (!done[0]) done[0] = set_tail(0);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 4) if (set_neck(4)) i++;
    if (i == 5) if (s[14].wait(500)) i++;
    if (i == 6) if (set_eyelids(0,0.2)) i++;
    if (i == 7) if (set_eyelids(3,0.2)) i++;
    if (i == 8) if (set_eyelids(0,0.2)) i++;
    if (i == 9) if (set_eyelids(3,0.2)) i++;
    if (i == 10) if (s[14].wait(1500)) i++;
    if (i == 11) {
        i = 0;
        return true;
    }
    else return false;
}

bool do_doubtful(Characters c) {
    if (i == 0) if (set_chest(5, 0.1)) i++;
    if (i == 1) {
        if (!done[0]) done[0] = set_body_rotation(6, 0.2);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 2) if (set_body_rotation(1, 0.2)) i++;
    if (i == 3) {
        if (!done[0]) done[0] = set_body_rotation(5, 0.2);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 4) if (set_body_rotation(1, 0.2)) i++;
    if (i == 5) {
        if (!done[0]) done[0] = set_body_rotation(6, 0.2);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 6) if (set_body_rotation(1, 0.2)) i++;
    if (i == 7) if (set_chest(1,0.1)) i++;
    if (i == 8) if (set_eyelids(6)) i++;
    if (i == 9) {
        if (!done[0]) done[0] = move_eyes(2);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 10) {
        if (!done[0]) done[0] = move_eyes(0);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 11 && repeat_actions < 1) {
        repeat_actions++;
        i = 9;
    }
    if (i == 11 && repeat_actions == 1) if (move_eyes(1)) i++;
    if (i == 12) {
        if (set_body_rotation(1, 0.2, c)) i++;
    }
    if (i == 13) {
        if (!done[0]) done[0] = set_eyelids(7);
        if (!done[1]) done[1] = s[14].wait(2000);
        if (!done[2]) done[2] = set_chest(5, 0.1);
        if (done[0] && done[1] && done[2]) {
            done[0] = false; done[1] = false; done[2] = false;
            i++;
        }
    }
    if (i == 14) {
        i = 0;
        repeat_actions = 0;
        return true;
    }
    else return false;
}

bool do_annoyed(Characters c) {
    if (i == 0) if (set_body_rotation(1, 0.2, c)) i++;
    if (i == 1) {
        if (!done[0]) done[0] = set_eyelids(4, 0.02);
        if (!done[1]) done[1] = s[14].wait(1500);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 2) {
        if (!done[0]) done[0] = set_chest(4, 0.1);
        if (!done[1]) done[1] = s[14].wait(1500);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 3) {
        if (!done[0]) done[0] = roll_eyes(2);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 4) {
        if (!done[0]) done[0] = roll_eyes(1);
        if (!done[1]) done[1] = s[14].wait(1500);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 5) {
        if (!done[0]) done[0] = move_eyes(2);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }
    if (i == 6) {
        if (!done[0]) done[0] = set_body_rotation(2, 0.2, c);
        if (!done[1]) done[1] = s[14].wait(1000);
        if (done[0] && done[1]) {
            done[0] = false; done[1] = false;
            i++;
        }
    }

    if (i == 7) {
        i = 0;
        return true;
    }
    else return false;
}
