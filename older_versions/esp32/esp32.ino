#include "movements.h"
#include "wifi_setup.h"
#include "mp3Player_setup.h"

enum State {
  RESET_POSITION = 0,
  IDLE = 1,
  HAPPY = 2,
  SAD = 3,
  ANGRY = 4,
  SHOCKED = 5,
  DOUBTFUL = 6
};

State state = IDLE;
State next_state = IDLE;

//messages and responses
String response;
bool writing = false;

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer

  Wire.begin(I2C_SDA, I2C_SCL);
  controller.begin();
  controller.setOscillatorFrequency(27000000);
  controller.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
  Serial.println('\n');

  for(byte servo=0;servo<16;servo++){
    s[servo] = movement(servo, delay_t0[servo]);
    done[servo] = false;
  }
  delay(100);
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP32 to the computer

  //start esp32 server
  server.on("/", handle_root);
  server.on("/controller", handle_controller);
  server.on("/emotion", handle_emotion);

  server.begin();
  Serial.println("HTTP server started");
  delay(100); 

  //let the mock server know your ip  
  message = "/esp32?init=yes&esp32_ip=" + WiFi.localIP().toString();
  send_message(server_ip, server_port, message);

  //setup mp3 player
  mySoftwareSerial.begin(9600, SERIAL_8N1, 17, 16);  // speed, type, TX, RX

  while(!myDFPlayer.begin(mySoftwareSerial)) {
		Serial.println("Unable to begin DFPlayer");
	}
	Serial.println("DFPlayer Mini online.");

	myDFPlayer.volume(10);
	myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
	myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
}


void loop() {
  server.handleClient();
  if(client.available()) {
    writing = !writing;
    char c = client.read();
    response += c;
  } else if(writing) {
    writing = !writing;
    response = response.substring(response.indexOf("\n\r")+3);
    Serial.print("Received response: ");
    Serial.println(response);
    response = "";
  }

  if(state == RESET_POSITION) {
    if(reset_position()) {
      state = next_state;
      next_state =  IDLE;
    }
  }

  if(state == IDLE) if(do_idle()) state = RESET_POSITION;

  if(state == HAPPY) if(do_happy()) state = RESET_POSITION;

  if(state == SAD) if(do_sad()) state = RESET_POSITION;

  if(state == ANGRY) if(test()) state = RESET_POSITION;

  if(state == SHOCKED) if(test_synchro()) state = RESET_POSITION;

  if(state == DOUBTFUL) if(undo_happy()) state = RESET_POSITION; 

  play_song(1); 

}

// Handle root url (/)
void handle_root() {
  String servo_channel = server.arg("servo_channel");
  String angle = server.arg("angle");
  server.send(200, "text/html", "<p>Hello world!</p>");
  Serial.println("I received something! Servo_channel " + servo_channel + " and angle " + angle);

  if(server.arg("send_message")) {
    //connect esp32 as client to the main server  
    message = "/esp32?esp32_ip=" + WiFi.localIP().toString();
    send_message(server_ip, server_port, message);
  }
}

void handle_controller() {
  if( server.arg("servo") && server.arg("servo_pos")) {
    int servo = atoi(server.arg("servo").c_str());
    int up = 1;
    int power = 0;
    
    if(server.arg("down").equals("down")) {
      up*= -1;
    }
    if(server.arg("servo_pos").equals("low")) {
      s[servo].set_servo_pos(s[servo].get_servo_pos() + 1*up);
    }
    else if(server.arg("servo_pos").equals("medium")) {
      s[servo].set_servo_pos(s[servo].get_servo_pos() + 10*up);
    }
    else if(server.arg("servo_pos").equals("high")) {
      s[servo].set_servo_pos(s[servo].get_servo_pos() + 50*up);
    }
    else if(server.arg("power")) {
      power = atoi(server.arg("power").c_str());
      s[servo].set_servo_pos(s[servo].get_servo_pos() + power*up);
    } 
  
    Serial.print("Moving servo number ");
    Serial.println(servo);
    Serial.print("to bit: ");
    Serial.println(s[servo].get_servo_pos());
    
    if(servo == 6 || servo == 8) {
      s[servo+1].set_servo_pos(s[servo+1].get_servo_pos() - power*up);
      controller.setPWM(servo+1, 0, s[servo+1].get_servo_pos());
    }
    else if(servo == 7 || servo == 9) {
      s[servo-1].set_servo_pos(s[servo-1].get_servo_pos() - power*up);
      controller.setPWM(servo-1, 0, s[servo-1].get_servo_pos());
    }
    controller.setPWM(servo, 0, s[servo].get_servo_pos());
    delay(10);
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
}

void handle_emotion() {
  if(server.arg("emotion").equals("idle")) {
    Serial.println("******START IDLE*******");
    next_state = IDLE;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
  if(server.arg("emotion").equals("happy")) {
    Serial.println("******START HAPPY*******");
    next_state = HAPPY;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
  if(server.arg("emotion").equals("sad")) {
    Serial.println("******START SAD*******");
    next_state = SAD;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
  if(server.arg("emotion").equals("angry")) {
    Serial.println("******START ANGRY*******");
    next_state = ANGRY;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
    if(server.arg("emotion").equals("shocked")) {
    Serial.println("******START SHOCKED*******");
    next_state = SHOCKED;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
    if(server.arg("emotion").equals("doubtful")) {
    Serial.println("******START DOUBTFUL*******");
    next_state = DOUBTFUL;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
}

