#ifndef movements_h
#define movements_h

#include <Adafruit_PWMServoDriver.h> // Include library to move motors
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
const int S0_OPEN=100,		S0_CLOSE=340; 
const int S1_CLOSE=100,		S1_OPEN=400; 
const int S2_DOWN=100,		S2_UP=300,			S2_MID=200;
const int S3_RIGHT=100,		S3_LEFT=220,		S3_MID=180; 
const int S4_CLOSE=100,		S4_OPEN=340; 
const int S5_OPEN=100,		S5_CLOSE=410; 
const int S6_CLOSE=100,		S6_OPEN=170,		S6_MID=130; 
const int S7_OPEN=130,		S7_CLOSE=200,		S7_MID=170;
const int S8_BACK = 230, S8_FRONT = 320, S8_MID = 280, S8_SAD = 290;
const int S9_BACK = 340, S9_FRONT = 260, S9_MID = 300, S9_SAD = 310;
const int S10_LEFT = 480, S10_RIGHT = 110, S10_MID = 305;
const int S11_DOWN = 190, S11_UP = 390, S11_MID = 300,		S11_SAD = 270;

const char delay_t0[16] = {30, 30, 30, 30, 30, 30, 30, 30, 60, 60, 30, 30, 30, 30, 30, 30};

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
  bool done;

public:
  movement();
  movement(unsigned char servo, unsigned char delay_t0);
  void new_position(Transition t);
  bool wait(long int delay_t0);

  //GETTERS AND SETTERS
  float get_servo_pos();
  void set_servo_pos(float servo_pos);
  bool get_mip();
  bool get_done();
  void set_done(bool done);
};

extern bool done[16];
extern movement s[16];
extern int i;

void setup_servos();

bool set_eyelids(int m, float v);
bool set_mouth(int m, float v);
bool set_chest(int m, float v);
bool set_body_rotation(int m, float v);
bool set_neck(int m, float v);
bool roll_eyes(int m, float v);
bool move_eyes(int m, float v);

//state functions
bool reset_position();
bool do_idle();
bool do_happy();
bool do_sad();
bool do_angry();
bool do_shocked();
bool do_doubtful();

//test functions
bool undo_happy();
bool test();
bool test_synchro();

#endif