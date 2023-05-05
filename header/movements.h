#ifndef movements_h
#define movements_h
#include <Adafruit_PWMServoDriver.h> // Include library to move motors

#define I2C_SDA 21
#define I2C_SCL 22
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
const int S0_OPEN=100,  S0_CLOSE=340; 
const int S1_CLOSE=100, S1_OPEN=400; 
const int S2_DOWN=100,  S2_UP=300,    S2_MID = 200;
const int S3_RIGHT=100, S3_LEFT=220,  S3_MID = 180; 
const int S4_CLOSE=100, S4_OPEN=340; 
const int S5_OPEN=100,  S5_CLOSE=410; 
const int S6_CLOSE=100, S6_OPEN=170; 
const int S7_OPEN=130,  S7_CLOSE = 200;

extern Adafruit_PWMServoDriver controller;

struct Transition {
  int fin_pos;
  float velocity; 
};

class movement{
private:
  unsigned char servo;
  int fin_pos;
  float velocity;  // min = 0, max = 1
  bool motion_in_progress;
  long int t0;
  unsigned char delay_t0;  //delay_t0[milliseconds]
  unsigned char epsilon;
  float servo_pos;
  bool executed;
  unsigned char iter;

public:
  movement();
  movement(unsigned char servo);
  bool new_position(Transition t);
  bool sequence_of_moves(Transition transitions[], int i);

  //GETTERS AND SETTERS
  bool get_executed();
  void set_executed(bool executed);
  float get_servo_pos();
  void set_servo_pos(float servo_pos);
  unsigned char get_iter();
  void set_iter(unsigned char iter);
  bool get_mip();
};

extern bool done[16];
extern movement s[16];
extern int i;

bool set_eyelids();
bool set_mouth();
bool blink();
bool do_happy();
bool undo_happy();

#endif