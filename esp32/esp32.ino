#include <WiFi.h>        // Include the Wi-Fi library
#include <WebServer.h>
#include <Adafruit_PWMServoDriver.h> // Include library to move motors
#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz update

// Define constants for key motor angles
const int S0_OPEN=100,  S0_CLOSE=380; 
const int S1_CLOSE=100, S1_OPEN=400; 
const int S2_DOWN=100,  S2_UP=300,    S2_MID = 200;
const int S3_RIGHT=100, S3_LEFT=220,  S3_MID = 180; 
const int S4_CLOSE=100, S4_OPEN=350; 
const int S5_OPEN=100,  S5_CLOSE=410; 
const int S6_CLOSE=100, S6_OPEN=170; 
const int S7_OPEN=130,  S7_CLOSE = 200;  

int servo = 0;
int servo_pos[16];
Adafruit_PWMServoDriver controller = Adafruit_PWMServoDriver(0x40);

//WiFi credentials
const char* ssid     = "AndroidAPFD5D";   //"TIM-22838165";   //"HUAWEI nova 5T";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "valerioo";        //"2rihtKdclmlITqJ82BfhZ9xk";         //"portanna";     // The password of the Wi-Fi network

//set variables for client connection
WiFiClient client;
char* server_ip = "192.168.142.177";
int server_port = 80;

//Set web server port number to 80
WebServer server(80);

/* NOT USED FOR NOW
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
*/

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
    servo = atoi(server.arg("servo").c_str());
    int up = 1;
    if(server.arg("down").equals("down")) {
      up*= -1;
    }
    if(server.arg("servo_pos").equals("low")) {
      servo_pos[servo] += 1*up;
    }
    else if(server.arg("servo_pos").equals("medium")) {
      servo_pos[servo] += 10*up;
    }
    else if(server.arg("servo_pos").equals("high")) {
      servo_pos[servo] += 50*up;
    }
  
    Serial.print("servo_channel: ");
    Serial.println(servo);
    Serial.print("bit: ");
    Serial.println(servo_pos[servo]);
    controller.setPWM(servo, 0, servo_pos[servo]);
    delay(100);
    server.send(200, "text/html", "<p>Hello world!</p>");
  }
}

void handle_emotion() {

}

