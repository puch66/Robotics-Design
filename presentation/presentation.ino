#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

#define I2C_SDA 4
#define I2C_SCL 5
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
const int S0_OPEN=100,  S0_CLOSE=380; // min = open, max = close
const int S1_CLOSE=100, S1_OPEN=400; // min = close, max = open
const int S2_DOWN=100,  S2_UP=240;
const int S3_RIGHT=100, S3_LEFT=200, S3_MID = 150; //min = right, max = left, mid = center
const int S4_CLOSE=100, S4_OPEN=350; // min = close, max = open
const int S5_OPEN=100,  S5_CLOSE=410; // min = 
const int S6_MIN=100,   S6_MAX=380;  
byte servo = 0;
int servo_pos[16];
const byte T = 10; //[seconds]
long t0;
byte t;


int state = 0;
byte flag_S0145 = 0;
int flag_S2 = S3_MID;
Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);
void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  controller.begin();
  controller.setOscillatorFrequency(27000000);
  controller.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  servo_pos[0] = S0_CLOSE;
  servo_pos[1] = S1_CLOSE;
  servo_pos[2] = 120;
  servo_pos[3] = S3_MID;
  servo_pos[4] = S4_CLOSE;
  servo_pos[5] = S5_CLOSE;
  for(servo=0;servo<6;servo++) controller.setPWM(servo, 0, servo_pos[servo]);
  
  t0 = millis();
}

void loop() {
  t = (millis()-t0)/(T*1000)*100; // % of completement
  if(state == 0){
    if(flag_S0145 == 0){ 
      servo_pos[0] =  int(0.95*servo_pos[0] + 0.05*S0_OPEN);
      servo_pos[1] =  int(0.95*servo_pos[1] + 0.05*S1_OPEN);
      servo_pos[4] =  int(0.95*servo_pos[4] + 0.05*S4_OPEN);
      servo_pos[5] =  int(0.95*servo_pos[5] + 0.05*S5_OPEN);
      if(servo_pos[0] <= S0_OPEN+20 ){ 
        servo_pos[0] = S0_OPEN;
        flag_S0145 = 1;
        state = 1;
        delay(2000);
        };
    };
    if(flag_S0145 == 1){ 
      servo_pos[0] =  int(0.95*servo_pos[0] + 0.05*S0_CLOSE);
      servo_pos[1] =  int(0.95*servo_pos[1] + 0.05*S1_CLOSE);
      servo_pos[4] =  int(0.95*servo_pos[4] + 0.05*S4_CLOSE);
      servo_pos[5] =  int(0.95*servo_pos[5] + 0.05*S5_CLOSE);
      if(servo_pos[0] >= S0_CLOSE-20){ 
        servo_pos[0] = S0_CLOSE;
        flag_S0145 = 0;
        delay(2000);
        };
    };
  };
  if(state == 1){
    if(flag_S2 == 120){
      servo_pos[2] =  int(0.8*servo_pos[2] + 0.2*S2_DOWN);
      if(servo_pos[2] <= S2_DOWN+20){ 
        servo_pos[2] = S2_DOWN;
        flag_S2 = S2_DOWN;
        delay(1000);
      }
    }
    if(flag_S2 == S2_DOWN){
      servo_pos[2] =  int(0.8*servo_pos[2] + 0.2*120);
      if(servo_pos[2] >= 120-20){ 
        servo_pos[2] = 120;
        flag_S2 = 120;
        state = 0;
      }
    }
  }
  controller.setPWM(0, 0, servo_pos[0]);
  controller.setPWM(1, 0, servo_pos[1]);
  controller.setPWM(4, 0, servo_pos[4]);
  controller.setPWM(5, 0, servo_pos[5]);
  Serial.print(flag_S0145);
  Serial.print(' ');
  Serial.println(servo_pos[0]);
  delay(10);
}
