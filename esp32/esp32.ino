#include <WiFi.h>        // Include the Wi-Fi library
#include <WebServer.h>

//WiFi credentials
const char* ssid     = "TIM-22838165";//"AndroidAPFD5D";   //"HUAWEI nova 5T";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "2rihtKdclmlITqJ82BfhZ9xk"; //"valerioo";        //"portanna";     // The password of the Wi-Fi network

//set variables for client connection
WiFiClient client;
char* server_ip = "192.168.1.101";
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
  delay(10);
  Serial.println('\n');
  
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

  server.begin();
  Serial.println("HTTP server started");
  delay(100); 
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

  //connect esp32 as client to the main server  
  message = "/esp32?esp32_ip=" + WiFi.localIP().toString();
  send_message(server_ip, server_port, message);
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

