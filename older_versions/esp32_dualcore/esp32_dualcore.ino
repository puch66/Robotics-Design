#include "movements.h"
#include "wifi_setup.h"
#include "mp3Player_setup.h"

TaskHandle_t Task1;
TaskHandle_t Task2;

State state = IDLE;
State next_state = IDLE;

//handle emotion exchanging
long randNumber;
Characters towards = ALL;
Characters next_towards = ALL;

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
  setup_mp3();

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
        state = next_state;
        towards = next_towards;
        next_state =  IDLE;
        next_towards = ALL;
        if(state!=IDLE && state!=READING_MESSAGE && state != LISTENING_MESSAGE && state!=WAIT) send_emotion(state, towards);
      }
    }

    if(state == IDLE) if(do_idle(towards)) state = RESET_POSITION;

    if(state == HAPPY) if(do_happy(towards)) state = RESET_POSITION;

    if(state == SAD) if(do_sad(towards)) state = RESET_POSITION;

    if(state == ANGRY) if(do_angry(towards)) state = RESET_POSITION;

    if(state == SHOCKED) if(do_shocked(towards)) state = RESET_POSITION;

    if(state == CAUTIOUS) if(do_doubtful(towards)) state = RESET_POSITION; 

    if(state == ANNOYED) if(do_annoyed(towards)) state = RESET_POSITION; 

    if(state == READING_MESSAGE) {
      //num variable is 1 if G5 is sent from god, 2 if G6 is sent
      int num = ((message[1] < 58) ? message[1] - '0' : message[1] - 55) -4;
      if(play_song(num)) {
        client.print("GG");
        
        //logic when after reading a god message arriving to you
        if(num == 1) {
          state= RESET_POSITION;
          next_state = ANGRY;
          next_towards = ALL;
        }
        else {
          state= RESET_POSITION;
          next_state = SHOCKED;
          next_towards = ALL;
        }  
      }
    }

    if(state == LISTENING_MESSAGE) {
      int num = (message[1] < 58) ? message[1] - '0' : message[1] - 55;
      num = num%2 == 0 ? num/2 : (num+1)/2;
      //Serial.println(num);
      towards = (Characters)(num+'0');
      if(set_body_rotation(1,0.2 , towards)) {
        waiting_for = message;
        state = WAIT;
      } 
    }

    if(state == WAIT) {
      if(listening_finished) {
        state = RESET_POSITION;
        listening_finished = false;
      }
    }

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

    if(!client.connected()) {
      connect_to_god();
    }

    if(Serial.available()) {  //debug
      String send = Serial.readString();
      client.print(send);
    }

    if(client.available())  {
      message = client.readString();
      Serial.println(message);
      
      //message sent by god
      if(state == WAIT && message.length() == 2 && message[0] == 'G' && message[1] == 'G') {
        //another character just finished reading the message, decide how to react
        next_towards = (Characters)compute_receiver(waiting_for);
        next_state = compute_state_for_GX(waiting_for);
        listening_finished = true;
      }
      else if(message.length() == 2 && message[0] == 'G' && message[1] != 'G') {
        hard_reset();
        int num = (message[1] < 58) ? message[1] - '0' : message[1] - 55;
        if(message[1] == 'F') {
          //GF signal from god means return to idle
          state = RESET_POSITION;
          next_state = IDLE;
        }
        else if((num%2!=0 && (num+1)/2 == (char)LELE - '0') || (num%2==0 && num/2 == (char)LELE - '0')) {
          //if it's a message directed to us, read it out loud
          state = RESET_POSITION;
          next_state = READING_MESSAGE;
        }
        else {
          //turn towards who is speaking
          state = RESET_POSITION;
          next_state = LISTENING_MESSAGE;
        }              
      }

      //emotion sent by someone
      else if(message.length() == 4) {
        if(is_interesting_to_us(message)) {
          next_towards = (Characters)compute_receiver(message);
          next_state = compute_state_for_emotion(message);          
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

        if (servo == 6 /*|| servo == 8*/) {
            s[servo + 1].set_servo_pos(s[servo + 1].get_servo_pos() - power * up);
            controller.setPWM(servo + 1, 0, s[servo + 1].get_servo_pos());
        }
        else if (servo == 7 /*|| servo == 9*/) {
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
        next_towards = LELE;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("happy")) {
        Serial.println("******START HAPPY*******");
        next_state = HAPPY;
        next_towards = LELE;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("sad")) {
        Serial.println("******START SAD*******");
        next_state = SAD;
        next_towards = LELE;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("angry")) {
        Serial.println("******START ANGRY*******");
        next_state = ANGRY;
        next_towards = LELE;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("shocked")) {
        Serial.println("******START SHOCKED*******");
        next_state = SHOCKED;
        next_towards = LELE;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("doubtful")) {
        Serial.println("******START DOUBTFUL*******");
        next_state = CAUTIOUS;
        next_towards = BIANCA;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
    if (server.arg("emotion").equals("annoyed")) {
        Serial.println("******START ANNOYED*******");
        next_state = ANNOYED;
        next_towards = LELE;
        server.send(200, "text/html", "<p>Hello world!</p>");
    }
}

bool is_interesting_to_us(String message) {
  if(message[2] == (char) LELE) return true;
  else if(message[2] == (char) ALL) return true;
  else { 
    if(message[1] == (char) ANGRY) {
      if(message[2] == (char) PEPPE) return true;
      else if(message[2] == (char) CARLOTTA) return true;
    }
    else if(message[1] == (char) SHOCKED) {
      if(message[2] == (char) CARLOTTA) return true;
    }
    else if(message[1] == (char) CAUTIOUS) {
      if(message[2] == (char) CARLOTTA) return true;
    }
    else if(message[1] == (char) ANNOYED) {
      if(message[2] == (char) PEPPE) return true;
    }
  }
  return false;
}

State compute_state_for_GX(String message) {
  if(message[1] == '1') return HAPPY;
  else if(message[1] == '2') return SHOCKED;
  else if(message[1] == '3') return SHOCKED;
  else if(message[1] == '4') return CAUTIOUS;
  else if(message[1] == '5') return ANGRY;
  else if(message[1] == '6') return SHOCKED;
  else if(message[1] == '7') return SAD;
  else if(message[1] == '8') return ANGRY;
  else if(message[1] == '9') return HAPPY;
  else if(message[1] == 'A') return HAPPY;
  else if(message[1] == 'B') return SHOCKED;
  else if(message[1] == 'C') return ANNOYED;
  else if(message[1] == 'D') return CAUTIOUS;
  else if(message[1] == 'E') return SHOCKED;
}

State compute_state_for_emotion(String message) {
  randNumber = random(100);
  //message is sent to you
  if(message[2] == (char) LELE) {
    if(message[1] == (char) HAPPY) return HAPPY;
    else if(message[1] == (char) ANGRY) {
      if(message[0] == (char) CARLOTTA) {
        if(randNumber < 33) return ANNOYED;
        else if(randNumber < 67) return ANGRY;
        else return SAD;
      }
      else if(message[0] == (char) PEPPE) return SAD;
      else return ANGRY;
    }
    else if(message[1] == (char) SHOCKED) {
      if(message[0] == (char) CARLOTTA) {
        if(randNumber < 50) return SAD;
        else return CAUTIOUS;
      }
      else if(message[0] == (char) COSIMO) return ANGRY;
      else return SHOCKED;
    }
    else if(message[1] == (char) SAD) {
      if(message[0] == (char) CARLOTTA) {
        if(randNumber < 50) return SAD;
        else return ANGRY;
      }
      else if(message[0] == (char) COSIMO) return ANGRY;
      else return SAD;      
    }
    else if(message[1] == (char) RELAXED) return HAPPY;
    else if(message[1] == (char) AFRAID) return SAD;
    else if(message[1] == (char) CAUTIOUS) {
      if(randNumber < 50) return ANNOYED;
      else return CAUTIOUS;
    }
    else if(message[1] == (char) SURPRISED) return ANNOYED;
    else if(message[1] == (char) ANNOYED) return ANGRY;
    else if(message[1] == (char) EMBARASSED) {
      if(randNumber < 33) return ANNOYED;
      else if(randNumber < 67) return ANGRY;
      else return SAD;
    }
    else if(message[1] == (char) ANXIOUS) {
      if(randNumber < 50) return ANGRY;
      else return CAUTIOUS;
    }
  }
  //message is sent to everyone
  else if(message[2] == (char) ALL) {
    if(message[1] == (char) HAPPY) return HAPPY;
    else if(message[1] == (char) ANGRY) {
      if(message[0] == (char) PEPPE) return ANGRY;
      else {
        if(randNumber < 50) return ANNOYED;
        else return ANGRY;
      }
    }
    else if(message[1] == (char) SHOCKED) {
      if(message[0] == (char) CARLOTTA) return ANGRY;
      else if(message[0] == (char) COSIMO) return ANGRY;
      else return SAD;
    }
    else if(message[1] == (char) SAD) {
      if(message[0] == (char) CARLOTTA) {
        if(randNumber < 50) return SAD;
        else return CAUTIOUS;
      }
      else if(message[0] == (char) PEPPE) return ANGRY;
      else  {
        if(randNumber < 50) return ANNOYED;
        else return CAUTIOUS;
      }
    }
    else if(message[1] == (char) RELAXED) return HAPPY;
    else if(message[1] == (char) AFRAID) {
      if(randNumber < 50) return SAD;
      else return CAUTIOUS;
    }
    else if(message[1] == (char) CAUTIOUS) {
      if(randNumber < 50) return ANNOYED;
      else return CAUTIOUS;
    }
    else if(message[1] == (char) SURPRISED) return HAPPY;
    else if(message[1] == (char) ANNOYED) return ANNOYED;
    else if(message[1] == (char) EMBARASSED) return HAPPY;
    else if(message[1] == (char) ANXIOUS) return CAUTIOUS;
  }
  //message is sent to someone else
  else { 
    if(message[1] == (char) ANGRY) {
      if(message[2] == (char) PEPPE) return ANGRY;
      else if(message[2] == (char) CARLOTTA) return ANGRY;
    }
    else if(message[1] == (char) SHOCKED) {
      if(message[2] == (char) CARLOTTA) return CAUTIOUS;
    }
    else if(message[1] == (char) CAUTIOUS) {
      if(message[2] == (char) CARLOTTA) return CAUTIOUS;
    }
    else if(message[1] == (char) ANNOYED) {
      if(message[2] == (char) PEPPE) return ANGRY;
    }
  }

  return IDLE;
}

char compute_receiver(String message) {
  //message sent by god
  if(message.length() == 2 && message[0] == 'G') {
    int num = (message[1] < 58) ? message[1] - '0' : message[1] - 55; 
    if(num == 2*((char)LELE - '0') - 1 || num == 2*((char)LELE - '0')) {
      return ALL;   //MESSAGE TOWARDS ALL
    }
    else {
      return (num%2 == 0 ? num/2 : (num+1)/2)+'0';   //MESSAGE TOWARDS WHO THE MESSAGE IS BEING SENT TO
    }
  }
  //emotion sent by someone
  else if(message.length() == 4) {
    //message is sent to you
    if(message[2] == (char) LELE) {
      return message[0]; //MESSAGE TOWARDS WHO SENT YOU THE EMOTION
    }
    //message is sent to everyone
    else if(message[2] == (char) ALL) {
      if(message[1] == (char) HAPPY) return message[0];
      else if(message[1] == (char) ANGRY) {
        if(message[0] == (char) PEPPE) return ALL;
        else return message[0];
      }
      else if(message[1] == (char) SHOCKED) return message[0];
      else if(message[1] == (char) SAD) {
        if(message[0] == (char) PEPPE) return ALL;
        else return message[0];
      }
      else if(message[1] == (char) RELAXED) return ALL;
      else if(message[1] == (char) AFRAID) return message[0];
      else if(message[1] == (char) CAUTIOUS) return message[0];
      else if(message[1] == (char) SURPRISED) return ALL;
      else if(message[1] == (char) ANNOYED) return message[0];
      else if(message[1] == (char) EMBARASSED) return ALL;
      else if(message[1] == (char) ANXIOUS) return message[0];
    }
    //message is sent to someone else
    else {
      if(message[1] == (char) ANGRY) {
        if(message[2] == (char) PEPPE) return message[0];
        else if(message[2] == (char) CARLOTTA) return message[0];
      }
      else if(message[1] == (char) SHOCKED) {
        if(message[2] == (char) CARLOTTA) return CARLOTTA;
      }
      else if(message[1] == (char) CAUTIOUS) {
        if(message[2] == (char) CARLOTTA) return CARLOTTA;
      }
      else if(message[1] == (char) ANNOYED) {
        if(message[2] == (char) PEPPE) return message[0];
      }
    }
    
  }
  else return ALL;
}

void send_emotion(State s, Characters t) {
    //prepare response
    response = "";
    response += (char)LELE;  //MESSAGE FROM LELE
    response += (char)s; //EMOTION
    response += (char)t; //RECEIVER OF THE MESSAGE    
    response += '0';   //INTENSITY: NONE

    //Send message WHEN EMOTION IS STARTING
    client.print(response);
}

void hard_reset() {
  for(int i=0; i<16; i++) {
    s[i].set_done(false);
    s[i].set_mip(false);
    done[i] = false;    
  }
  i = 0;
  repeat_actions = 0;
  init_song = false;
  listening_finished = false;
  myDFPlayer.pause();  //pause the mp3
}
