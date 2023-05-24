#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>

#define I2C_SDA 21 //esp32
#define I2C_SCL 22 //esp32
//#define I2C_SDA 4 //esp8266
//#define I2C_SCL 5 //esp8266
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
const byte T = 10; //[seconds]
long t0;
byte t;


int state = 0;
byte flag = 0;
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
  servo_pos[2] = S2_MID;
  servo_pos[3] = S3_MID;
  servo_pos[4] = S4_CLOSE;
  servo_pos[5] = S5_CLOSE;
  servo_pos[6] = S6_CLOSE;
  servo_pos[7] = S7_CLOSE;
  for(servo=0;servo<6;servo++) controller.setPWM(servo, 0, servo_pos[servo]);
  delay(1000);
  t0 = millis();
}

void loop() {
  t = (millis()-t0)/(T*1000)*100; // % of completement
  if(state == 0){
    servo_pos[0] =  int(0.95*servo_pos[0] + 0.05*S0_OPEN);
    servo_pos[1] =  int(0.95*servo_pos[1] + 0.05*S1_OPEN);
    servo_pos[4] =  int(0.95*servo_pos[4] + 0.05*S4_OPEN);
    servo_pos[5] =  int(0.95*servo_pos[5] + 0.05*S5_OPEN);
    if(servo_pos[0] <= S0_OPEN+20 ){ 
      servo_pos[0] = S0_OPEN;
      state = 1;
      delay(2000);
      }
    }
  if(state == 1){
    servo_pos[2] = S2_DOWN;
    controller.setPWM(2, 0, servo_pos[2]);
    delay(1000);
    state = 2;
  }
  if(state == 2){
    servo_pos[2] =  S2_MID;
    controller.setPWM(2, 0, servo_pos[2]);
    delay(1000);
    state = 3;
  }
  if(state == 3){
    servo_pos[3] =  S3_MID + 50;
    controller.setPWM(3, 0, servo_pos[3]);
    delay(2000);
    servo_pos[3] =  S3_MID - 100;
    controller.setPWM(3, 0, servo_pos[3]);
    delay(2000);
    servo_pos[3] =  S3_MID;
    controller.setPWM(3, 0, servo_pos[3]);
    delay(2000);
    state = 4;
  }
  if(state == 4){
    servo_pos[6] =  int(0.95*servo_pos[6] + 0.05*S6_OPEN);
    servo_pos[7] =  int(0.95*servo_pos[7] + 0.05*S7_OPEN);
    if(servo_pos[6] >= S6_OPEN-20){ 
      servo_pos[6] = S6_OPEN;
      servo_pos[7] = S7_OPEN;
      state = 5;
      delay(2000);
    }
  }
  if(state == 5){
    servo_pos[6] =  int(0.95*servo_pos[6] + 0.05*S6_CLOSE);
    servo_pos[7] =  int(0.95*servo_pos[7] + 0.05*S7_CLOSE);
    if(servo_pos[6] <= S6_CLOSE+20){ 
      servo_pos[6] = S6_CLOSE;
      servo_pos[7] = S7_CLOSE;
      state = 6;
      delay(2000);
    }
  }
  if(state == 6){ 
    servo_pos[0] =  int(0.95*servo_pos[0] + 0.05*S0_CLOSE);
    servo_pos[1] =  int(0.95*servo_pos[1] + 0.05*S1_CLOSE);
    servo_pos[4] =  int(0.95*servo_pos[4] + 0.05*S4_CLOSE);
    servo_pos[5] =  int(0.95*servo_pos[5] + 0.05*S5_CLOSE);
    if(servo_pos[0] >= S0_CLOSE-20){ 
      servo_pos[0] = S0_CLOSE;
      state = 0;
      delay(2000);
    }
  }
  controller.setPWM(0, 0, servo_pos[0]);
  controller.setPWM(1, 0, servo_pos[1]);
  controller.setPWM(4, 0, servo_pos[4]);
  controller.setPWM(5, 0, servo_pos[5]);

  controller.setPWM(6, 0, servo_pos[6]);
  controller.setPWM(7, 0, servo_pos[7]);
  Serial.println(state);
  delay(10);
}
