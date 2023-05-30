# Robotics-Design
*This repository provides the esp32 code for CamaLele robot, developed for the Robotics and Design course at Politecnico di Milano. There are also included some architectures to manage the communication between our robot and the other ones developed in the course. Finally, a manual controller to move the robot via wifi is provided for presentation and debug purposes*

## Arduino code for the esp32
### Installation and run
- download and install Arduino IDE on your laptop [Arduino IDE download](https://www.arduino.cc/en/software)
- in esp32_final/wifi_setup.h change lines 11-12 with your wifi ID and password
- open esp32_final.ino file with Arduino and upload it on the esp_32

## God server
- download and install python on your laptop [Python Windows download](https://www.python.org/downloads/windows/)
- open a terminal in the *server* folder and run 
  ``` bash
   python server.py
  ```

## Manual controller
### Installation and run
 - download node.js and install on your laptop: [Node.js Windows download](https://nodejs.org/en/download)
 - open a terminal in the *server* folder and run 
   ``` bash
   node server.js 
   ```
 - the server will be now active at an **ip address printed in the terminal**
 - the server will be accessible from any device connected to its same network
 - to access the server, just type its ip address on a web browser
 - on the server, you can send commands to the robot to rotate any motor by any degree
 
 ## ESP32 client-server communication
 ### Client
  - client side is implemented using the Arduino WiFiClient library
  - to send a message, just use the function send_message(), specifying the ip_address and port of the receiver and the content of the message
  - the first time you connect to the mock server, he will not know your ip_address, so a message is sent to it with content
    ``` c++
    /?esp32_ip=" + WiFi.localIP().toString();
    ```
  **IMPORTANT:** ESP32 does not know the ip address of the server initially, so you have to set it manually changing the server_ip value in esp32.ino script
  
  ### Server
   - server side is implemented using the Arduino WebServer library
   - server is always listening at the url http://*esp32_ip*/ where *esp32_ip* is the local ip address of the esp32, that is printed in Arduino terminal when the ESP32 first connects to the network
   - thus, to send a message to ESP32, just a HTTP GET request has to be performed
  
