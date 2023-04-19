#include <WiFi.h>        // Include the Wi-Fi library
#include <WebServer.h>
#include <Adafruit_PWMServoDriver.h> // Include library to move motors
#include <Wire.h>
#include "movements.h"

#define IDLE 0;
#define HAPPY 1;
#define SAD 2;

unsigned char state = IDLE;
bool blinking;

//WiFi credentials
const char* ssid     = "Vodafone-33573324"; //"AndroidAPFD5D";   //"TIM-22838165";   //"HUAWEI nova 5T";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "f3545j5a52vxfht"; //"valerioo";        //"2rihtKdclmlITqJ82BfhZ9xk";         //"portanna";     // The password of the Wi-Fi network

//set variables for client connection
WiFiClient client;
char* server_ip = "192.168.1.2";
int server_port = 80;

//Set web server port number to 80
WebServer server(80);

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

  for(byte servo=0;servo<6;servo++){
    s[servo] = movement(servo);
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

  if(state == 1) if(blink()) state = IDLE;

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
    int fin_pos;
    if(server.arg("down").equals("down")) {
      up*= -1;
    }
    if(server.arg("servo_pos").equals("low")) {
       fin_pos = s[servo].get_servo_pos() + 1*up;
    }
    else if(server.arg("servo_pos").equals("medium")) {
      fin_pos = s[servo].get_servo_pos() + 10*up;
    }
    else if(server.arg("servo_pos").equals("high")) {
      fin_pos = s[servo].get_servo_pos() + 50*up;
    }
  
    Serial.print("Moving servo number ");
    Serial.println(servo);
    Serial.print("to bit: ");
    Serial.println(fin_pos);
    Transition t = {fin_pos, 0.05};
    s[servo].new_position(t);
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
}

void handle_emotion() {
  if(server.arg("emotion").equals("happy")) {
    Serial.println("******START BLINKING*******");
    state = HAPPY;
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
}

