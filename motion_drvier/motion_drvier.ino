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
 
byte servo = 0;
int servo_pos[16];

void setup(){
  
}
void loop(){
  
}
class movement{
  private byte servo;
  private int servo_pos;
  private int init_pos,fin_pos;
  byte velocity;
  bool motion_in_progress = false;
  public:
    movement(byte servo){
      this->servo = servo;
    }
    void new_position(int init_pos,int fin_pos, byte velocity){
      if(motion_in_progress == false){
        this->init_pos = init_pos;
        this->fin_pos = fin_pos;
        this->velocity = velocity;
        if(velocity<0) this->velocity = 0;
        else if(velocity>1) this->velocity = 100;
        motion_in_progress == true;
      }
    }
    void make_motion(){
      elseif(vel{
      if(velocity < 0) velocity = 0;
      else if(velocity > 1) velocity = 100;
      servo_pos =  int((1-velocity)*servo_pos + velocity*fin_pos);
    }
};
