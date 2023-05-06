#ifndef wifi_setup_h
#define wifi_setup_h

#include <WiFi.h>        // Include the Wi-Fi library
#include <WebServer.h>

//WiFi credentials
const char* ssid = "AndroidAPFD5D"; //"Vodafone-33573324";   //"TIM-22838165";   //"HUAWEI nova 5T";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "valerioo";  //"f3545j5a52vxfht";       //"2rihtKdclmlITqJ82BfhZ9xk";         //"portanna";     // The password of the Wi-Fi network

//set variables for client connection
WiFiClient client;
char* server_ip = "192.168.94.177";
int server_port = 80;

//Set web server port number to 80
WebServer server(80);
#endif