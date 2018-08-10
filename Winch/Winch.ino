// WINCH controller
// The winch controller receives potentiometer data via RF from the pilot.
// GP April 2018  Adapted from Radiohead example sketch showing how to create a simple messageing client
// with the RH_RF95 class.  
// NDM June 2018  Updated to be abit cleaner, fix our saftey and get a reliable and fast link speed. 
// We had alot of jerking before, not so much now. I think alot that is left is the actual chineese controller :(
// The Arduino seems to be  outputting a voltage update every 10ms or so. 

#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;

// Config Vars
#define PWM_OUTPUT_MIN  55      // PWM - PWM Voltage for 1v is around PWM-55
#define PWM_OUTPUT_MAX  190     // PWM - PWM Voltage for 3.8V is around PWM-190
#define RADIO_TIMEOUT   40      // millis - It Takes the controller about 30ms to read the Pot, and 5ms to transmit.
#define RADIO_FAILCOUNT 10

// Runtime Vars
int WinchSpeed    = 0 ;   // The pot reading from the pilot's controller
int Fail          = 0 ;   // Fail count for saftey cutoff
int WinchMotorPin = 3 ;   // Output pin for controller - This MUST be PWM

void setup() 
{
  Serial.begin(115200);               // Start Serial prompt
  while (!Serial) ;                 // Wait for serial port to be available
  pinMode(WinchMotorPin, OUTPUT);   // Tell Arduino that WinchMotorPin is an output pin
  Serial.println("Starting...");
  if (!rf95.init()){
    Serial.println("RF95 Init FAILED");
  }
}

void loop()
{
  uint8_t Sensor1CharMsg[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(Sensor1CharMsg);
  int i;
  
  if (rf95.waitAvailableTimeout(RADIO_TIMEOUT)){
    if (rf95.recv(Sensor1CharMsg, &len)){    
        Sensor1CharMsg[len] = '\0';          
        WinchSpeed = atoi(Sensor1CharMsg);    
        WinchSpeed = map(WinchSpeed, 0, 255, PWM_OUTPUT_MIN, PWM_OUTPUT_MAX);
        // Reset fail counter, we have a message. 
        Fail=0;
    }  
  }else{
    Fail+= 1;
    if (Fail >= RADIO_FAILCOUNT) {
      // We have failed radio, set winch speed to MIN right away. 
      WinchSpeed=PWM_OUTPUT_MIN ;
      Serial.println("RX FAIL!");
    }
  }
  Serial.print("DEBUG V1.1.0: Throttle: ");
  Serial.println(WinchSpeed);
  analogWrite(WinchMotorPin, WinchSpeed);
}
