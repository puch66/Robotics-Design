#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

#define I2C_SDA 4
#define I2C_SCL 5
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
const int S0_OPEN=100,  S0_CLOSE=380; 
const int S1_CLOSE=100, S1_OPEN=400; 
const int S2_DOWN=100,  S2_UP=300,    S2_MID = 200;
const int S3_RIGHT=100, S3_LEFT=220,  S3_MID = 180; 
const int S4_CLOSE=100, S4_OPEN=350; 
const int S5_OPEN=100,  S5_CLOSE=410; 
const int S6_CLOSE=100, S6_OPEN=170; 
const int S7_OPEN=130,  S7_CLOSE = 200;


movement s[16];
bool executed[16];
byte iter[16];
Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);
void setup(){
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  controller.begin();
  controller.setOscillatorFrequency(27000000);
  controller.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  
  for(byte servo=0;servo<6;servo++){
    s[servo] = new movement(servo);
    executed[servo] = false;
    iter[servo] = 0;
  }
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
    }*/
    if(!executed[0] && iter[0] < 2) {
      if(iter[0] == 0) executed[0] = s[0].new_position(s[0].servo_pos, S0_OPEN, 5);
      else if(iter[0] == 1) executed[0] = s[0].new_position(s[0].servo_pos, S0_CLOSE, 5);
    }
    else if(iter[0] < 2) {
      iter[0] ++;
      executed[0] = !executed[0];
    }
    
    if(iter[0] == 2) iter[0] = 0;
    
  }
}
class movement{
  private byte servo;
  public int servo_pos;
  private int fin_pos;
  byte velocity;  // min = 0, max = 100
  bool motion_in_progress = false;
  byte t0,delay_t0 = 10;  //delay_t0[milliseconds]
  byte epsilon = 10;
  public:
    movement(byte servo){
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
    bool new_position(int init_pos,int fin_pos, byte velocity){ // min = 0 max = 100
      if(!motion_in_progress){
        servo_pos = init_pos;
        this->fin_pos = fin_pos;
        this->velocity = velocity;
        if(velocity<0) this->velocity = 0;
        else if(velocity>100) this->velocity = 100;
        motion_in_progress = true;
        t0 = int(millis()/100);
        return false;
      }
      else {
        if(millis()/100-t0<0) t0 = millis()/100;
        if(millis()/100-t0>delta_t0){
          servo_pos =  int((1-velocity/100)*servo_pos + (velocity/100)*fin_pos);
          controller.setPWM(servo, 0, servo_pos);
        }
        if(servo_pos > fin_pos-epsilon && servo_pos < fin_pos+epsilon){
          motion_in_progress = false;
          return true;
        }
        else return false;
    }
};
