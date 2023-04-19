#include "movements.h"

Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);

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
      delay(2000);
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