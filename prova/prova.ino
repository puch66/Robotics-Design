#include <Adafruit_PWMServoDriver.h>

#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define SERVOMINsg90  120 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAXsg90  500 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
int servo = 0;
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
  for(servo=0;servo<16;servo++){ servo_pos[servo] = 130;};
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


//  controller.setPWM(servo_channel, 0, posizione);
//  servo_channel = 0; // Canale utilizzato
//  angolo = 0;
//  posizione = map(angolo, 0, 180, SERVOMINsg90, SERVOMAXsg90);
//  controller.setPWM(servo_channel, 0, posizione);
//  Serial.println(angolo);
//  delay(3000);

// if (Serial.available() > 0) {
//    int digit = Serial.read();
//    if( digit == 100){
//      angolo = 0;
//    }
//    if( digit == 119){
//      angolo = 90;
//    }
//    if( digit == 97){
//      angolo = 180;
//    }
//  }
