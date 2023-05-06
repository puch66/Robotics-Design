#include "movements.h"

Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);

bool done[16];
movement s[16];
int i = 0;

movement::movement() {

}

movement::movement(unsigned char servo, unsigned char delay_t0){
    motion_in_progress = false;
    this->delay_t0 = delay_t0;
    epsilon = 10;
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
    else if (servo == 9) this->servo_pos = S9_MID;
    else if(servo == 10) this->servo_pos = S10_MID;
    else if (servo == 11) this->servo_pos = S11_MID;
    else this->servo_pos = 0;
  
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
    if(!done[6]) done[6] = s[6].new_position({S6_MID,v});
    if(!done[7]) done[7] = s[7].new_position({S7_MID,v});
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

// m = 0 -> front
// m = 1 -> mid position
// m = 2 -> back
bool set_chest(int m, float v) {
    if (m == 0) {
        if (!done[8]) done[8] = s[8].new_position({ S8_FRONT,v });
        if (!done[9]) done[9] = s[9].new_position({ S9_FRONT,v });
    }
    else if (m == 1) {
        if (!done[8]) done[8] = s[8].new_position({ S8_MID,v });
        if (!done[9]) done[9] = s[9].new_position({ S9_MID,v });
    }
    else if (m == 2) {
        if (!done[8]) done[8] = s[8].new_position({ S8_BACK,v });
        if (!done[9]) done[9] = s[9].new_position({ S9_BACK,v });
    }

    if (done[8] && done[9]) {
        done[8] = false; done[9] = false;
        return true;
    }
    else return false;
}

// m = 0 -> left
// m = 1 -> mid position
// m = 2 -> right
bool set_body_rotation(int m, float v) {
    if (m == 0) {
        if (!done[10]) done[10] = s[10].new_position({ S10_LEFT,v });
    }
    else if (m == 1) {
        if (!done[10]) done[10] = s[10].new_position({ S10_MID,v });
    }
    else if (m == 2) {
        if (!done[10]) done[10] = s[10].new_position({ S10_RIGHT,v });
    }

    if (done[10]) {
        done[10] = false;
        return true;
    }
    else return false;
}

// m = 0 -> up
// m = 1 -> mid position
// m = 2 -> down
bool set_neck(int m, float v) {
    if (m == 0) {
        if (!done[11]) done[11] = s[11].new_position({ S11_UP,v });
    }
    else if (m == 1) {
        if (!done[11]) done[11] = s[11].new_position({ S11_MID,v });
    }
    else if (m == 2) {
        if (!done[11]) done[11] = s[11].new_position({ S11_DOWN,v });
    }

    if (done[11]) {
        done[11] = false;
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

bool test() {
    if (i == 0) if (set_chest(0, 0.015)) i++;
    if (i == 1) if (set_chest(2, 0.015)) i++;
    if (i == 2) if (set_chest(1, 0.015)) i++;
    if (i == 3) if (set_body_rotation(0, 0.01)) i++;
    if (i == 4) if (set_body_rotation(2, 0.01)) i++;
    if (i == 5) if (set_body_rotation(1, 0.01)) i++;
    if (i == 6) if (set_neck(0, 0.01)) i++;
    if (i == 7) if (set_neck(2, 0.01)) i++;
    if (i == 8) if (set_neck(1, 0.01)) i++;

    //Serial.println(i);
    Serial.println(done[10]);
    if (i == 9) {
        i = 0;
        return true;
    }
    else return false;
}

bool test_synchro() {
    if (i == 0) {
        if (!done[14]) done[14] = set_chest(0, 0.02);
        if (!done[15]) done[15] = set_neck(0, 0.02);

        if (done[14] && done[15]) {
            done[14] = false; done[15] = false;
            i++;
        }
    }

    //Serial.println(i);
    if (i == 1) {
        i = 0;
        return true;
    }
    else return false;

}
