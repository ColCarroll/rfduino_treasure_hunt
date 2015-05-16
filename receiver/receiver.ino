/*
Easter Egg Hunter for RFduino
Detector Device Program
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

3.0 : Added in dimming (using analogWrite) and blinking, both controlled by the signal strength.
Note: There is an issue with the default analogWrite that conflicts with the GZLL library.  You need to edit the c-code
as described in another post (http://forum.rfduino.com/index.php?topic=517.msg1748#msg1748)

2.0 : Rewrite
Note: Switching the roles of the Detector and Target objects will allow
for multiple Targets while still allowing for multiple players.
* Complete rewrite of the code.
* Detectors are now Hosts
* Target's are now Devices


1.0 : Working prototype
* Send ping to host
* receive return ping from host
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
//set number of targets
#define TARGETS 8

//-------------------------------------------------------------------
//VARIABLES
//-------------------------------------------------------------------
//set device type
//as detectors are passive receivers they can all be hosts and just receive
//the transmission from the Target Device.
device_t role = HOST;

int Signal_Total[TARGETS];
int Signal_Count[TARGETS];
device_t Signal_ID[TARGETS];
device_t Closest_Device; //intended for future expansions

//Time to sample signals
int Sample_Time = 50;

//pin assignements for the RGB LED
int led_blue_pin = 4;
int led_green_pin = 3;
int led_red_pin = 2;

//range incements (minimum range)
//Note: Does not conespond to real world distances
int Very_Close = 190;
int Close = 150;
int Near = 110;
int Far = 90;
int Very_Far = 60;
int Extreme_Far = 30;

//Activate/deactivate communication flag
int Ready_To_Receive = 0;

//-------------------------------------------------------------------
//FUNCTIONS
//-------------------------------------------------------------------
void SetLEDs(int Signal_Strength)
{
  //convert the signal strength into range
  int Range;

  //Range = map(Signal_Strength, -128, -40, 0, 255);
  Range = map(Signal_Strength, -128, -35, 0, 255);
  Range = constrain(Range, 0, 255);
  
  //set the LED to indicate range to host device
  if (Range > Very_Close)
  {
    //red
    analogWrite(led_red_pin, Range);
    analogWrite(led_green_pin, 0);
    analogWrite(led_blue_pin, 0);
  }
  else if (Range > Close)
  {
    //red + green (yellow)
    analogWrite(led_red_pin, Range/2);
    analogWrite(led_green_pin, Range/2);
    analogWrite(led_blue_pin, 0);
  }
  else if (Range > Near)
  {
    //green
    analogWrite(led_red_pin, 0);
    analogWrite(led_green_pin, Range);
    analogWrite(led_blue_pin, 0);
  }
  else if (Range > Far)
  {
    //green + blue (cyan)
    analogWrite(led_red_pin, 0);
    analogWrite(led_green_pin, Range/2);
    analogWrite(led_blue_pin, Range/2);
  }
  else if (Range > Very_Far)
  {
    //blue
    analogWrite(led_red_pin, 0);
    analogWrite(led_green_pin, 0);
    analogWrite(led_blue_pin, Range);
  }
  else if (Range > Extreme_Far)
  {
    //blue + red (purple)
    analogWrite(led_red_pin, Range/2);
    analogWrite(led_green_pin, 0);
    analogWrite(led_blue_pin, Range/2);
  }
  else
  {
    //white
    analogWrite(led_red_pin, Range/3);
    analogWrite(led_green_pin, Range/3);
    analogWrite(led_blue_pin, Range/3);
  }

  // program a variable delay based on signal strength
  delay(4*(255 - Range)); 
  
  //all off - to create a blink
  analogWrite(led_red_pin, 0);
  analogWrite(led_green_pin, 0);
  analogWrite(led_blue_pin, 0); 
  
  // program a variable delay based on signal strength
  delay(4*(255 - Range)); 

}

//-------------------------------------------------------------------
//MAIN PROGRAM
//-------------------------------------------------------------------
void setup() {
  RFduinoGZLL.txPowerLevel = +4; //Sets the transmit power to max +4dBm
  
  // initialize the serial communication
  Serial.begin(9600);
  
  // Setup communications
  RFduinoGZLL.begin(role);
  
  //setup pins for LED
  pinMode(led_red_pin, OUTPUT);
  pinMode(led_green_pin, OUTPUT);
  pinMode(led_blue_pin, OUTPUT);
}

void loop()
{  
  int i; //loop counter
  int Average_Signal[TARGETS];
  
  //reset signal strength tracking
  for (i = 0; i < TARGETS; i++)
  {
    Signal_Total[i] = 0;
    Signal_Count[i] = 0;
    Average_Signal[i]= -128;
  }
  
  //receive signals samples
  Ready_To_Receive = 1;
  delay (Sample_Time);
  Ready_To_Receive = 0;
     
  //calc average signal strength for each Target Device
  for (i = 0; i < TARGETS; i++)
  {
    if (Signal_Count[i] > 0)
    {
      Average_Signal[i] = Signal_Total[i] / Signal_Count[i];
    }
  }
  
  // find the device with the maximum RSSI value
  int closest = 0;
  for (i = 1; i < TARGETS; i++)
  {
    if (Average_Signal[i] > Average_Signal[closest])
    {
      closest = i;
    }
  }
    
  Closest_Device = Signal_ID[closest];

  //set the led status  
  SetLEDs(Average_Signal[closest]);
  
  // send the value to the serial port
  //while (Serial.available() <= 0) {
  //  Serial.println(Average_Signal[closest]);
  //  delay(2000);
  //}
}

//-------------------------------------------------------------------
//INTERRUPTS
//-------------------------------------------------------------------
void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
  if (Ready_To_Receive)
  {
    if (device < TARGETS)
    {
      Signal_Total[device] += rssi;
      Signal_Count[device]++;
      Signal_ID[device] = device;
    }
  }
}
