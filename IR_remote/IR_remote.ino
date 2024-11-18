/*
 * Sketch to toggle ON/OFF GREEN LED when corect button is pushed on a remote control
 * run this sketch first, use the SERIAL MONITOR within the IDE, set to 9600 baud, press the button on the remote you would like to operate
 * the GREEN LED, write down the HEX value displayed, and place it back into the sketch into the variable button.
 * Download the sketch angain, this time when you push the button on the remote the GREEN LED will turn ON, press again and it will turn OFF
 *
 * LIBRARY USED:
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Library Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 4;           // pin that the IR receiver is connected to
int RELAY_PIN = 9;          // GREEN LED pin 
int received;               // variable to hold the data received from IR receiver
int button_1 = 0xFFA25D;      // this is the HEX value for my remote, enter your HEX value,
int button_2 = 0xFF629D;      // this is the HEX value for my remote, enter your HEX value,
int button_3 = 0xFFE21D;      // this is the HEX value for my remote, enter your HEX value,
int button_4 = 0xFF22DD;      // this is the HEX value for my remote, enter your HEX value,
int button_HASH = 0xFFB04F;      // this is the HEX value for my remote, enter your HEX value,
int button_STAR = 0xFF6897;      // this is the HEX value for my remote, enter your HEX value,
int button_OK = 0xFF38C7;      // this is the HEX value for my remote, enter your HEX value,
int button_UP = 0xFF18E7;      // this is the HEX value for my remote, enter your HEX value,
int button_DN = 0xFF4AB5;      // this is the HEX value for my remote, enter your HEX value,
int button_L = 0xFF10EF;      // this is the HEX value for my remote, enter your HEX value,
int button_R = 0xFF5AA5;      // this is the HEX value for my remote, enter your HEX value,
IRrecv irrecv(RECV_PIN);    // set up the IR remote library defining the IR receiver pin
decode_results results;    // variable used by the IR remote library to store received data.
boolean on = false;        // variable used to control the GREEN LED
void setup()
{
  pinMode(RELAY_PIN, OUTPUT);    // set pin for OUTPUT
  pinMode(13, OUTPUT);            // set ONBOARD LED pin 13 for OUTPUT
  Serial.begin(9600);            // setup serial communications so we can see data on IDE monitor
  irrecv.enableIRIn();           // Start the IR remote Library
  irrecv.blink13(true);        // flash ONBOARD LED PIN 13 everytime we receive IR data - controlled by IR remote library
}


void loop() {
   if(irrecv.decode(&results))              // check to see if we have received any IR remote data, if so then decode the data
  {
   Serial.println(results.value, HEX);     // print on serial monitor the received IR value as a HEX number
  received = results.value;                // store received IR data
  irrecv.resume();                           // get ready to Receive the next IR data value
  }
  if (received == button_1)                   // check if the received data is the same as the data corresponding to our push button
  {
      on = !on;                            // if it is then toggle the variable called on, if its ON turn it OFF, if its OFF turn it ON
      digitalWrite(RELAY_PIN, on);        // turn ON or OFF the GREEN LED using the on variable above
      received = 0;                        // clear our received data variable ready for next time
    }     
     delay(100);                          // small delay to allow things to settle.
  }
