#include <Adafruit_PWMServoDriver.h>

#include <Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define SERVOMINsg90  120 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAXsg90  500 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
int servo_channel = 0;
int posizione = 0;
int angolo = 0;
Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);
void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  controller.begin();
  controller.setOscillatorFrequency(27000000);
  controller.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  posizione = 100;
  servo_channel = 1; // Canale utilizzato
}
void loop() {
  if (Serial.available() > 0) {
    int digit = Serial.read();
    if( digit == 113){ posizione = posizione +1;};
    if( digit == 119){ posizione = posizione +10;};
    if( digit == 101){ posizione = posizione +50;};
    if( digit == 97){ posizione = posizione -1;};
    if( digit == 115){ posizione = posizione -10;};
    if( digit == 100){ posizione = posizione -50;};
    Serial.print("bit: ");
    Serial.println(posizione);
  }
  controller.setPWM(0, 0, posizione);
  controller.setPWM(1, 0, posizione);
  
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
