/*
Easter Egg Hunter for RFduino
Target Device Program
v2.0
By Paul WIlson
19/02/2014

Easter Egg Hunter is a simple demonstration of using the RSSI value
of a wifi singal to give the players hints as to the distance to a
target Host device "Easter Egg".

Each player gets a "Detector" device which will receive a "ping" from the
Easter Eggs which will then send a return signal. The Detector will use the
strength of the return signal to change the status of an RGB LED to indicate
to the player how close they are to the Easter Egg.
*/

/*
Version History

2.0 : Rewrite
Note: Switching the roles of the Detector and Target objects will allow
for multiple Targets while still allowing for multiple players.
* Complete rewrite of the code.
* Detectors are now Hosts
* Target's are now Devices


1.0 : Working prototype
* Send ping to host
* Recieve return ping from host
* change led based on strength of return singal
* Include timeout and change range to maximum on timeout
*/

//-------------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------------
#include <RFduinoGZLL.h>

//-------------------------------------------------------------------
//DEFINES
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//VARIABLES
//-------------------------------------------------------------------
//set device type
//as detectors are pasive recievers they can all be hosts and just recieve
//the transmission from the Target Device.
device_t role = DEVICE0;

//Transmit delay (ms)
int Delay_ms(10);

void setup() {
  // Setup communications
  RFduinoGZLL.begin(role);
}

void loop() {
  delay(Delay_ms);
  RFduinoGZLL.sendToHost(NULL, 0);
}
