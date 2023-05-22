#ifndef wifi_setup_h
#define wifi_setup_h

#include <WiFi.h>        // Include the Wi-Fi library
#include <WebServer.h>

//WiFi credentials
const char* ssid = "AndroidAPFD5D"; //"Vodafone-33573324";   //"TIM-22838165";   //"HUAWEI nova 5T";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "valerioo";  //"f3545j5a52vxfht";       //"2rihtKdclmlITqJ82BfhZ9xk";         //"portanna";     // The password of the Wi-Fi network
//const char* ssid = "test";
//const char* password = "test1234567!";
//const char* ssid = "Triskarone";
//const char* password = "triskarone";


//set variables for server connection
WiFiClient client;
char* server_ip = "192.168.206.177";
char* god_ip = "192.168.206.177";
int server_port = 80;
int god_port = 8090;

//Set web server port number to 80
WebServer server(80);

String message = "3A00", response, waiting_for;
bool listening_finished = false;

void connect_to_god() {
    // Server connection
    Serial.println();
    Serial.println("Connecting to Server...");
    while (!client.connect(god_ip, god_port)) {
        delay(1000);
        //Serial.print(".");
    }
    Serial.println("Server connected");
    Serial.println();
}

void setup_connection() {
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

    connect_to_god();
}

void send_message(char* ip, int port, String message) {
    if (client.connect(ip, port)) {
        Serial.println("connected to " + (String)ip);
        client.println("GET " + message + " HTTP/1.0");
        client.println();
    }
    else {
        Serial.println("Some error occurred :(");
    }
}

#endif