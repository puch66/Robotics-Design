#ifndef movements_h
#define movements_h

#include <Adafruit_PWMServoDriver.h> // Include library to move motors
#include <Wire.h>

/**************************************************
***************  SERVO CONSTANTS  *****************
***************************************************/

#define I2C_SDA 21
#define I2C_SCL 22
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update
const int S0_OPEN=100,		S0_CLOSE=340, S0_ANGRY = 250; 
const int S1_CLOSE=100,		S1_OPEN=400, S1_ANGRY = 150; 
const int S2_DOWN=100,		S2_UP=300,			S2_MID=200;
const int S3_RIGHT=100,		S3_LEFT=270,		S3_MID=180; 
const int S4_CLOSE=100,		S4_OPEN=340, S4_ANGRY = 200; 
const int S5_OPEN=100,		S5_CLOSE=410, S5_ANGRY = 380; 
const int S6_CLOSE=100,		S6_OPEN=170,		S6_MID=130, S6_ANGRY = 110; 
const int S7_OPEN=128,		S7_CLOSE=198,		S7_MID=168, S7_ANGRY = 188;
const int S8_BACK = 230, S8_FRONT = 320, S8_MID = 280, S8_SAD = 290, S8_ANGRY = 290, S8_DOUBTFUL = 300, S8_HAPPY = 260;
const int S9_OPEN = 150, S9_CLOSE = 330, S9_SMALL_OPEN = 210, S9_SMALL_CLOSE = 270;
const int S10_LEFT = 480, S10_RIGHT = 110, S10_MID = 305, S10_IDLE_LEFT = 331, S10_IDLE_RIGHT = 211, S10_DOUBTFUL_LEFT = 410, S10_DOUBTFUL_RIGHT = 200;
const int S11_DOWN = 190, S11_UP = 390, S11_MID = 320,	S11_SAD = 290,	S11_ANGRY = 330, S11_HAPPY = 340;
									
const int character_pos_LEFT[8] =  {331, 253, 219, 365, 480, 474, 443, 409};
const int character_pos_MID[8] =   {272, 191, 153, 305, 450, 417, 384 ,349};
const int character_pos_RIGHT[8] = {211, 120, 100, 246, 391, 357, 324, 290};

const char delay_t0[16] = { 30, 30, 30, 30, 30, 30, 30, 30, 60, 60, 30, 30, 30, 30, 30, 30 };

/**************************************************
***************  STATE CONSTANTS  *****************
***************************************************/

enum State {
	RESET_POSITION = 0, IDLE = 'A', HAPPY = 'B',
	ANGRY = 'C', SHOCKED = 'D', SAD = 'E',
	RELAXED = 'F', AFRAID = 'G', CAUTIOUS = 'H',
	SURPRISED = 'I', ANNOYED = 'J', EMBARASSED = 'K',
	ANXIOUS = 'L', READING_MESSAGE = 1, LISTENING_MESSAGE = 2, WAIT = 3
};

enum Characters {
	ALL = '0', ROCCO = '1', EVA = '2',
	LELE = '3', CARLOTTA = '4', PEPPE = '5',
	BIANCA = '6', COSIMO = '7'
};

extern Adafruit_PWMServoDriver controller;

/**************************************************
***********  FUNCTIONS INITIALIZATION *************
***************************************************/

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
  void set_mip(bool mip);
  bool get_done();
  void set_done(bool done);
};

extern bool done[16];
extern movement s[16];
extern int i;
extern int repeat_actions;
extern int idle_step;

//servo initialization
void setup_servos();

//body part movement functions
bool set_eyelids(int m, float v);
bool set_mouth(int m, float v);
bool set_chest(int m, float v);
bool set_body_rotation(int m, float v);
bool set_body_rotation(int m, float v, Characters c);
bool set_neck(int m, float v);
bool roll_eyes(int m, float v);
bool move_eyes(int m, float v);

//state functions
bool reset_position();
bool do_idle(Characters c = LELE, long rand = 0);
bool do_happy(Characters c = LELE);
bool do_sad(Characters c = LELE);
bool do_angry(Characters c = LELE);
bool do_shocked(Characters c = LELE);
bool do_doubtful(Characters c = LELE);
bool do_annoyed(Characters c = LELE);
bool read_message(int number);

#endif