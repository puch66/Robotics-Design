#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
const int S0_OPEN=100,  S0_CLOSE=380; 
const int S1_CLOSE=100, S1_OPEN=400; 
const int S2_DOWN=100,  S2_UP=300,    S2_MID = 200;
const int S3_RIGHT=100, S3_LEFT=220,  S3_MID = 180; 
const int S4_CLOSE=100, S4_OPEN=350; 
const int S5_OPEN=100,  S5_CLOSE=410; 
const int S6_CLOSE=100, S6_OPEN=170; 
const int S7_OPEN=130,  S7_CLOSE = 200;

Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);

class movement{
private:
  byte servo;
  int fin_pos;
  byte velocity;  // min = 0, max = 100
  bool motion_in_progress = false;
  long int t0;
  byte delay_t0 = 10;  //delay_t0[milliseconds]
  byte epsilon = 10;

public:
  float servo_pos;
  bool executed;
  byte iter;

  movement() {

  }

  movement(byte servo){
      this-> iter = 0;
      this -> executed = false;
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
    
  bool new_position(int init_pos,int fin_pos, float velocity){ // min = 0 max = 1
    if(!motion_in_progress){
      servo_pos = init_pos;
      this->fin_pos = fin_pos;
      this->velocity = velocity;
      if(velocity<0) this->velocity = 0;
      else if(velocity>100) this->velocity = 100;
      motion_in_progress = true;
      t0 = millis();
      return false;
    }
    else {
      if(millis()-t0<0) t0 = millis();
      if(millis()-t0>delay_t0){
        servo_pos =  (1-velocity)*servo_pos + velocity*fin_pos;
        controller.setPWM(servo, 0, int(servo_pos));
        delay(10);
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
};

movement s[16];

void setup(){
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  controller.begin();
  controller.setOscillatorFrequency(27000000);
  controller.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  
  for(byte servo=0;servo<6;servo++){
    s[servo] = movement(servo);
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
    if(!s[0].executed && s[0].iter < 2) {
      if(s[0].iter == 0) s[0].executed = s[0].new_position(s[0].servo_pos, S0_OPEN, 0.05);
      else if(s[0].iter == 1) s[0].executed = s[0].new_position(s[0].servo_pos, S0_CLOSE, 0.05);
    }
    else if(s[0].iter < 2) {
      s[0].iter ++;
      s[0].executed = !s[0].executed;
    }
    
    if(s[0].iter == 2) {
      s[0].iter = 0;
      s[0].executed = false;
    }
    
}
