#include "movements.h"
#include "wifi_setup.h"

enum State {
  IDLE = 0,
  HAPPY = 1,
  SAD = 2,
  ANGRY = 3,
  SHOCKED = 4,
  DOUBTFUL = 5
};

State state = IDLE;

//messages and responses
String message, response;
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

  if(state == IDLE);

  if(state == HAPPY) if(do_happy()) state = IDLE;

  if(state == SAD) if(undo_happy()) state = IDLE;

  if(state == ANGRY) if(test()) state = IDLE;

  if(state == SHOCKED) if(test_synchro()) state = IDLE;

  if(state == DOUBTFUL) if(undo_happy()) state = IDLE;  

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

void send_message(char* ip, int port, String message) {
  if (client.connect(ip, port)) {
    Serial.println("connected to " + (String) ip);
    client.println("GET " + message + " HTTP/1.0");
    client.println();
  } else {
    Serial.println("Some error occurred :(");
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
  if(server.arg("emotion").equals("happy")) {
    Serial.println("******START HAPPY*******");
    state = HAPPY;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
  if(server.arg("emotion").equals("sad")) {
    Serial.println("******START SAD*******");
    state = SAD;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
  if(server.arg("emotion").equals("angry")) {
    Serial.println("******START ANGRY*******");
    state = ANGRY;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
    if(server.arg("emotion").equals("shocked")) {
    Serial.println("******START SHOCKED*******");
    state = SHOCKED;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
    if(server.arg("emotion").equals("doubtful")) {
    Serial.println("******START DOUBTFUL*******");
    state = DOUBTFUL;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
}




