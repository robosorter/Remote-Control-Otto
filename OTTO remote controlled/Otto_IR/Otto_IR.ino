//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-- Otto DIY APP Firmware version 9 (V09) 
//-- Otto DIY invests time and resources providing open source code and hardware,  please support by purchasing kits from (https://www.ottodiy.com)
//-----------------------------------------------------------------
//-- If you wish to use this software under Open Source Licensing, you must contribute all your source code to the community and all text above must be included in any redistribution
//-- in accordance with the GPL Version 2 when your application is distributed. See http://www.gnu.org/copyleft/gpl.html
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// -- REMOVED serial and app code to allow room for IR remote commands  Jason Snow Nov 2024
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <IRremote.h>
//-- Otto Library version 9
#include <Otto9.h>
Otto9 Otto;  //This is Otto!
//-- IR remote library sensor input pin
int RECV_PIN = 11;           // pin that the IR receiver is connected to
//-- 
int received;               // variable to hold the data received from IR receiver
int REPEAT_received;               // variable to hold the data received from IR receiver
bool active = false;      
int button_1 = 0xFFA25D;      // this is the HEX value for my remote, enter your HEX value,
int button_2 = 0xFF629D;      // this is the HEX value for my remote, enter your HEX value,
int button_3 = 0xFFE21D;      // this is the HEX value for my remote, enter your HEX value,
int button_4 = 0xFF22DD;      // this is the HEX value for my remote, enter your HEX value,
int button_5 = 0xFF02FD;      // this is the HEX value for my remote, enter your HEX value,
int button_6 = 0xFFC23D;      // this is the HEX value for my remote, enter your HEX value,
int button_7 = 0xFFE01F;      // this is the HEX value for my remote, enter your HEX value,
int button_8 = 0xFFA857;      // this is the HEX value for my remote, enter your HEX value,
int button_9 = 0xFF906F;      // this is the HEX value for my remote, enter your HEX value,
int button_0 = 0xFF9867;      // this is the HEX value for my remote, enter your HEX value,
int button_HASH = 0xFFB04F;      // this is the HEX value for my remote, enter your HEX value,
int button_STAR = 0xFF6897;      // this is the HEX value for my remote, enter your HEX value,
int button_OK = 0xFF38C7;      // this is the HEX value for my remote, enter your HEX value,
int button_UP = 0xFF18E7;      // this is the HEX value for my remote, enter your HEX value,
int button_DN = 0xFF4AB5;      // this is the HEX value for my remote, enter your HEX value,
int button_L = 0xFF10EF;      // this is the HEX value for my remote, enter your HEX value,
int button_R = 0xFF5AA5;      // this is the HEX value for my remote, enter your HEX value,
int REPEAT_KEY = 0xFFFFFFFF;
IRrecv irrecv(RECV_PIN);    // set up the IR remote library defining the IR receiver pin
decode_results results;    // variable used by the IR remote library to store received data.

//---------------------------------------------------------
//-- First step: Configure the pins where the servos are attached
/*
         ---------------
        |     O   O     |
        |---------------|
YR 3==> |               | <== YL 2
         ---------------
            ||     ||
            ||     ||
RR 5==>   -----   ------  <== RL 4
         |-----   ------|
*/
// SERVO PINs //////////////////////////////////////////////////////////////////////////////
#define PIN_YL 2 //servo[0]  left leg
#define PIN_YR 3 //servo[1]  right leg
#define PIN_RL 4 //servo[2]  left foot
#define PIN_RR 5 //servo[3]  right foot
// ULTRASONIC PINs /////////////////////////////////////////////////////////////////////////
#define PIN_Trigger  8  //TRIGGER pin (8)
#define PIN_Echo     9  //ECHO pin (9)
// BUZZER PIN //////////////////////////////////////////////////////////////////////////////
#define PIN_Buzzer  12 //BUZZER pin (12)
// LED MATRIX PINs //////////////////////////////////////////////////////////////////////////
#define DIN_PIN    A3   //DIN pin (A3)
#define CS_PIN     A2   //CS pin (A2)
#define CLK_PIN    A1   //CLK pin (A1)
#define LED_DIRECTION  1// LED MATRIX CONNECTOR position (orientation) 1 = top 2 = bottom 3 = left 4 = right  DEFAULT = 1
// SERVO ASSEMBLY PIN   /////////////////////////////////////////////////////////////////////
// to help assemble Otto's feet and legs - wire link between pin 7 and GND
#define PIN_ASSEMBLY    7   //ASSEMBLY pin (7) LOW = assembly    HIGH  = normal operation
///////////////////////////////////////////////////////////////////
//-- Global Variables -------------------------------------------//
///////////////////////////////////////////////////////////////////
const char programID[] = "Otto_IR"; //Each program will have a ID
//-- Movement parameters
int T = 1000;            //Initial duration of movement
int moveId = 0;          //Number of movement
int modeId = 0;          //Number of mode
int moveSize = 15;       //Asociated with the height of some movements
unsigned long previousMillis = 0;
bool obstacleDetected = false;
bool AUTO = false;
unsigned long int matrix;
unsigned long timerMillis = 0;
///////////////////////////////////////////////////////////////////
//-- Setup ------------------------------------------------------//
///////////////////////////////////////////////////////////////////
void setup() {
  //Serial communication initialization
  Serial.begin(9600);
  Otto.init(PIN_YL, PIN_YR, PIN_RL, PIN_RR, true, A6, PIN_Buzzer, PIN_Trigger, PIN_Echo); //Set the servo pins and ultrasonic pins
  Otto.initMATRIX( DIN_PIN, CS_PIN, CLK_PIN, LED_DIRECTION);   // set up Matrix display pins = DIN pin,CS pin, CLK pin, MATRIX orientation 
  Otto.matrixIntensity(1);// set up Matrix display intensity
  pinMode(PIN_ASSEMBLY,INPUT_PULLUP); // - Easy assembly pin - LOW is assembly Mode
  //Otto wake up!
  Otto.sing(S_connection);
  Otto.home();
  // Animation Uuuuuh - A little moment of initial surprise
  //-----
  for (int i = 0; i < 2; i++) {
    for (int i = 0; i < 8; i++) {
      Otto.putAnimationMouth(littleUuh, i);
      delay(150);
    }
  }
  //Smile for a happy Otto :)
  Otto.putMouth(smile);
  Otto.sing(S_happy);
  delay(200);
  Otto.putMouth(happyOpen);
  previousMillis = millis();
// if Pin 7 is LOW then place OTTO's servos in home mode to enable easy assembly, 
// when you have finished assembling Otto, remove the link between pin 7 and GND
  while (digitalRead(PIN_ASSEMBLY) == LOW) {
    Otto.home();
    Otto.sing(S_happy_short);   // sing every 5 seconds so we know OTTO is still working
    delay(5000);
  }
irrecv.enableIRIn();           // Start the IR remote Library
//irrecv.blink13(true);        // flash OUR RX LED everytime we receive IR data - controlled by IR remote library
}
///////////////////////////////////////////////////////////////////
//-- Principal Loop ---------------------------------------------//
///////////////////////////////////////////////////////////////////
void loop() {
  
    // check remote control for data  
   if(irrecv.decode(&results))              // check to see if we have received any IR remote data, if so then decode the data
  {
   //Serial.println(results.value, HEX);     // print on serial monitor the received IR value as a HEX number
    received = results.value;                // store received IR data
    irrecv.resume();                           // get ready to Receive the next IR data value
    previousMillis = millis(); // ensure we don't trigger the sleeping Otto mode if we are receiving IR instructions
  }
  
  if (received == REPEAT_KEY)                   // check if the received data is the same as the data corresponding to our push button
    {
      // remote key continuosly pressed (REPEAT)so keep the same movement so OTTO continues to walk
      delay(50);
       received = REPEAT_received;      // if we have a repeat command use the prevoius command   
                    
       //Serial.println(received);    //debugging serial output
       //Serial.println("repeat");    //debugging serial output
    }
    if (received == button_UP)                   // check if the received data is the same as the data corresponding to our push button
    {
      //OTTO walk
      Otto.putMouth(happyOpen);
      Otto.walk(1, T, 1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
    if (received == button_DN)                   // check if the received data is the same as the data corresponding to our push button
    {
      //OTTO walk backward
      Otto.putMouth(happyOpen);
      Otto.walk(1, T, -1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
      
    }
    if (received == button_L)                   // check if the received data is the same as the data corresponding to our push button
    {
      //OTTO turn left
      Otto.putMouth(happyClosed );
      Otto.turn(1, T, 1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
      
    }
    if (received == button_R)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO turn right
       Otto.putMouth(happyClosed );
      Otto.turn(1, T, -1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
      
    }
// other OTTO commands
      if (received == button_1)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO flapping left
      Otto.putMouth(tongueOut);                  
      Otto.flapping(1, T, moveSize, 1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
      if (received == button_2)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO flapping right
      Otto.putMouth(okMouth); 
      Otto.flapping(1, T, moveSize, -1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
       if (received == button_3)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO tiptoe swing
      Otto.tiptoeSwing(1, T, moveSize);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
       if (received == button_4)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO crusaito left
      Otto.crusaito(1, T, moveSize, 1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
       if (received == button_5)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO crusaito right
      Otto.crusaito(1, T, moveSize, -1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
        if (received == button_6)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO shake leg left
      Otto.putMouth(smile);
      Otto.shakeLeg(1, T, 1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
         if (received == button_7)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO shake leg right
      Otto.putMouth(smallSurprise);    
      Otto.shakeLeg(1, T, -1);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
         if (received == button_8)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO jitter
      Otto.jitter(1, T, moveSize);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
        if (received == button_9)                   // check if the received data is the same as the data corresponding to our push button
    {
       //OTTO jump
      Otto.putMouth(heart); 
      Otto.jump(1, T);
      REPEAT_received = received;           // store command incase we have a repeat command
      received = 0;                        // clear our received data variable ready for next time
    }
       if (received == button_STAR)                   // check if the received data is the same as the data corresponding to our push button
    {
       AUTO = true; 
      received = 0;                        // clear our received data variable ready for next time
    }
           if (received == button_HASH)                   // check if the received data is the same as the data corresponding to our push button
    {
       AUTO = false; 
      received = 0;                        // clear our received data variable ready for next time
    }

  if (AUTO == true) {
    if (obstacleDetected) {  //  we have detected an object in the way so we take action
        Otto.putMouth(bigSurprise);
        Otto.sing(S_surprise);
        Otto.jump(5, 500);
        Otto.putMouth(confused);
        Otto.sing(S_cuddly);
        //Otto takes two steps back
        for (int i = 0; i < 3; i++) Otto.walk(1, 1300, -1);
        previousMillis = millis(); // ensure we don't trigger the sleeping Otto mode
        delay(100);
        //Otto shows a smile
         Otto.putMouth(smile);
        delay(500);
        //Otto turns left
        for (int i = 0; i < 3; i++) {
         Otto.turn(1, 1000, 1);
         delay(200);
          }
        }
        //If there are no obstacles  Otto is happy to walk straight
         Otto.putMouth(happyOpen);
        delay(200);
        previousMillis = millis(); // ensure we don't trigger the sleeping Otto mode
        Otto.walk(1, T, 1); //Otto walk straight
        // whilst he is walking some IR remote signals are ignored due to time spent walking
        obstacleDetector(); // check to see if an obstacle is in the way
        
      }
      //if we are not in auto or have had no IR commands for a period of time 80 seconds
      //we place Otto in sleeping mode
      if (millis() - previousMillis >= 80000) {
        OttoSleeping_withInterrupts(); //ZZzzzzz...
        previousMillis = millis();
        Otto.home();
      }
      if (AUTO == false) //if we are not in auto or have had no IR commands for a period of time 10 seconds
      //we place Otto in home mode to save power
         {
            if (millis() - previousMillis >= 10000) {
                previousMillis = millis();
                 Otto.home();
              }
        }
      
      
}
    

///////////////////////////////////////////////////////////////////
//-- Functions --------------------------------------------------//
///////////////////////////////////////////////////////////////////

//-- Function to read distance sensor & to actualize obstacleDetected variable
void obstacleDetector() {
  int distance = Otto.getDistance();
  if (distance < 15) obstacleDetected = true;
  else obstacleDetected = false;
  //Serial.print("distance = ");    //debugging serial output
  //Serial.println(distance);    //debugging serial output
}

//-- Functions with animatics
//--------------------------------------------------------
void OttoSleeping_withInterrupts() {
  int bedPos_0[4] = {100, 80, 60, 120};
  Otto._moveServos(700, bedPos_0);
  for (int i = 0; i < 4; i++) {
    Otto.putAnimationMouth(dreamMouth, 0);
    Otto.bendTones (100, 200, 1.04, 10, 10);
    Otto.putAnimationMouth(dreamMouth, 1);
    Otto.bendTones (200, 300, 1.04, 10, 10);
    Otto.putAnimationMouth(dreamMouth, 2);
    Otto.bendTones (300, 500, 1.04, 10, 10);
    delay(500);
    Otto.putAnimationMouth(dreamMouth, 1);
    Otto.bendTones (400, 250, 1.04, 10, 1);
    Otto.putAnimationMouth(dreamMouth, 0);
    Otto.bendTones (250, 100, 1.04, 10, 1);
    delay(500);
  }
  Otto.putMouth(lineMouth);
  Otto.sing(S_cuddly);
  Otto.home();
  Otto.putMouth(happyOpen);

}


