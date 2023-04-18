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
int servo_angle = 0;
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
  servo = 0; // Canale utilizzato
}
void loop() {
  if (Serial.available() > 0) {
    int digit = Serial.read();
    if(digit >= 48 && digit <= 57){
      if( digit == 48){ servo = 0;}; 
      if( digit == 49){ servo = 1;};
      if( digit == 50){ servo = 2;};
      if( digit == 51){ servo = 3;};
      if( digit == 52){ servo = 4;};
      if( digit == 53){ servo = 5;};
      if( digit == 54){ servo = 6;};
      if( digit == 55){ servo = 7;};
      if( digit == 56){ servo = 8;};
      if( digit == 57){ servo = 9;};
      Serial.print("servo_channel: ");
      Serial.println(servo);
    }
    if( digit == 113){ servo_pos[servo] += 1;};  // q
    if( digit == 119){ servo_pos[servo] += 10;}; // w
    if( digit == 101){ servo_pos[servo] += 50;}; // e
    if( digit == 97){  servo_pos[servo] -= 1;};   // a
    if( digit == 115){ servo_pos[servo] -= 10;}; // s
    if( digit == 100){ servo_pos[servo] -= 50;}; // d
    Serial.print("servo_channel: ");
    Serial.print(servo);
    Serial.print(" bit: ");
    Serial.println(servo_pos[servo]);
  }
  controller.setPWM(servo, 0, servo_pos[servo]);
  delay(100);
}
