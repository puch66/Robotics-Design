#include "movements.h"
#include "wifi_setup.h"
#include "mp3Player_setup.h"

TaskHandle_t Task1;
TaskHandle_t Task2;

enum State {
  RESET_POSITION = 0,   IDLE = 'A',       HAPPY = 'B',
  ANGRY = 'C',          SHOCKED = 'D',    SAD = 'E',
  RELAXED = 'F',        AFRAID = 'G',     DOUBTFUL = 'H',
  SURPRISED = 'I',      ANNOYED = 'J',    EMBARASSED = 'K',
  ANXIOUS = 'L',        READING_MESSAGE = 'Z'
};

enum Characters {
  ALL = '0',              ROCCO = '1',        EVA = '2',
  LELE = '3',             CARLOTTA = '4',     PEPPE = '5',
  BIANCA = '6',           COSIMO = '7'
};

State state = IDLE;
State next_state = IDLE;

//handle emotion exchanging
bool send_emotion = false;
long randNumber;
char towards = ALL;
char next_towards = ALL;

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer

  setup_servos();

  setup_connection();

  //start esp32 server
  server.on("/", handle_root);
  server.on("/controller", handle_controller);
  server.on("/emotion", handle_emotion);

  server.begin();
  Serial.println("HTTP server started");
  delay(100);

  //setup mp3 player
  //setup_mp3();

  //setup dual core tasks
  xTaskCreatePinnedToCore(main_loop, "main", 10000, NULL, 1, &Task1, 0);                 
  delay(500); 

  xTaskCreatePinnedToCore(network_loop, "network", 10000, NULL, 1, &Task2, 1);
  delay(500);

  //set seed for random number generation
  randomSeed(analogRead(0));
}

void main_loop(void * pvParameters) {
  while(true) {
    
    if(state == RESET_POSITION) {
      if(reset_position()) {
        if(next_state!=IDLE) send_emotion = true;
        state = next_state;
        //towards = next_towards;
        next_state =  IDLE;
        //next_towards = ALL;
        check_send_emotion();
      }
    }

    if(state == IDLE) if(do_idle()) state = RESET_POSITION;

    if(state == HAPPY) if(do_happy()) state = RESET_POSITION;

    if(state == SAD) /*if(do_sad())*/ state = RESET_POSITION;

    if(state == ANGRY) if(test()) state = RESET_POSITION;

    if(state == SHOCKED) /*if(test_synchro())*/ state = RESET_POSITION;

    if(state == DOUBTFUL) if(undo_happy()) state = RESET_POSITION; 

    if(state == READING_MESSAGE) {
      int num = (message[1] < 58) ? message[1] - '0' : message[1] - 55; 
      //if(play_song(num)) {
        //TODO: implement logic when after reading a god message arriving to you
        randNumber = random(100);
        Serial.println(randNumber);
        if(randNumber < 50) {
          state= RESET_POSITION;
          next_state = SAD;
          Serial.println("******START SAD*******");
        }
        else {
          state= RESET_POSITION;
          next_state = SHOCKED;
          Serial.println("******START SHOCKED*******");
        }  
      //}
    }

    //play_song(1); 

    delay(10);  
  }
}

void network_loop(void * pvParameters) {
  // Handle connection and receive from WiFi (updating state)
  while(true) {
    server.handleClient();

    if(WiFi.status() != WL_CONNECTED) {
      setup_connection();

      //start esp32 server
      server.on("/", handle_root);
      server.on("/controller", handle_controller);
      server.on("/emotion", handle_emotion);

      server.begin();
      Serial.println("HTTP server started");
      delay(100);

      //let the mock server know your ip  
      //message = "/esp32?init=yes&esp32_ip=" + WiFi.localIP().toString();
      //send_message(server_ip, server_port, message);
    }

    if(Serial.available()) {  //debug
      String send = Serial.readString();
      client.print(send);
    }

    if(client.available())  {
      message = client.readString();
      Serial.println(message);
      
      //message sent by god
      if(message.length() == 2 && message[0] == 'G') {
        int num = (message[1] < 58) ? message[1] - '0' : message[1] - 55; 
        //Serial.println(num);
        if((num%2!=0 && (num+1)/2 == (char)LELE - '0') || (num%2==0 && num/2 == (char)LELE - '0')) {
          //if it's a message directed to us, read it out loud
          next_state = READING_MESSAGE;
        }
        else {
          //turn towards who is speaking
          //next_towards = compute_receiver(message);
        }              
      }

      //emotion sent by someone
      else if(message.length() == 4) {
        //for the moment, discard all messages which are not being directed to us
        //if a message is directed to us, reply with random emotion
        if(message[2] == (char)LELE) {
          randNumber = random(100);
          //Serial.println(randNumber);
          //next_towards = compute_receiver(message);
          if(randNumber < 50) {
            next_state = SAD;
            Serial.println("******START SAD*******");
          }
          else {
            next_state = SHOCKED;
            Serial.println("******START SHOCKED*******");
          }  
        }
      }
    }
    
    delay(10);  
  }
}

void loop() {

}

// Handle root url (/)
void handle_root() {
    String servo_channel = server.arg("servo_channel");
    String angle = server.arg("angle");
    server.send(200, "text/html", "<p>Hello world!</p>");
    Serial.println("I received something! Servo_channel " + servo_channel + " and angle " + angle);
}

void handle_controller() {
    if (server.arg("servo") && server.arg("servo_pos")) {
        int servo = atoi(server.arg("servo").c_str());
        int up = 1;
        int power = 0;

        if (server.arg("down").equals("down")) {
            up *= -1;
        }
        if (server.arg("servo_pos").equals("low")) {
            s[servo].set_servo_pos(s[servo].get_servo_pos() + 1 * up);
        }
        else if (server.arg("servo_pos").equals("medium")) {
            s[servo].set_servo_pos(s[servo].get_servo_pos() + 10 * up);
        }
        else if (server.arg("servo_pos").equals("high")) {
            s[servo].set_servo_pos(s[servo].get_servo_pos() + 50 * up);
        }
        else if (server.arg("power")) {
            power = atoi(server.arg("power").c_str());
            s[servo].set_servo_pos(s[servo].get_servo_pos() + power * up);
        }

        Serial.print("Moving servo number ");
        Serial.println(servo);
        Serial.print("to bit: ");
        Serial.println(s[servo].get_servo_pos());

        if (servo == 6 || servo == 8) {
            s[servo + 1].set_servo_pos(s[servo + 1].get_servo_pos() - power * up);
            controller.setPWM(servo + 1, 0, s[servo + 1].get_servo_pos());
        }
        else if (servo == 7 || servo == 9) {
            s[servo - 1].set_servo_pos(s[servo - 1].get_servo_pos() - power * up);
            controller.setPWM(servo - 1, 0, s[servo - 1].get_servo_pos());
        }
        controller.setPWM(servo, 0, s[servo].get_servo_pos());
        delay(10);
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
}

void handle_emotion() {
    if (server.arg("emotion").equals("idle")) {
        Serial.println("******START IDLE*******");
        next_state = IDLE;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("happy")) {
        Serial.println("******START HAPPY*******");
        next_state = HAPPY;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("sad")) {
        Serial.println("******START SAD*******");
        next_state = SAD;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("angry")) {
        Serial.println("******START ANGRY*******");
        next_state = ANGRY;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("shocked")) {
        Serial.println("******START SHOCKED*******");
        next_state = SHOCKED;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("doubtful")) {
        Serial.println("******START DOUBTFUL*******");
        next_state = DOUBTFUL;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
}

char compute_receiver(String message) {
  //message sent by god
  if(message.length() == 2 && message[0] == 'G') {
    int num = (message[1] < 58) ? message[1] - '0' : message[1] - 55; 
    if(num == 2*((char)LELE - '0') - 1 || num == 2*((char)LELE - '0')) {
      return ALL;   //MESSAGE TOWARDS ALL
    }
    else {
      return (num%2 == 0 ? num/2 : (num+1)/2);   //MESSAGE TOWARDS WHO THE MESSAGE IS BEING SENT TO
    }
  }
  //emotion sent by someone
  else if(message.length() == 4) {
    return message[0]; //MESSAGE TOWARDS WHO SENT YOU THE EMOTION
  }
}

void check_send_emotion() {
  if(send_emotion) {
    //prepare response
    response[0] = LELE;  //MESSAGE FROM LELE
    response[1] = state; //EMOTION
    response[2] = compute_receiver(message); //RECEIVER OF THE MESSAGE    
    response[3] = '0';   //INTENSITY: NONE

    //Send message WHEN EMOTION IS STARTING
    if(client.available()) client.print(response);
    send_emotion = false;
  }
}
