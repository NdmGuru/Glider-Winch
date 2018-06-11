// PILOT controller
// Allows a paraglider pilot to remotely operate a pay-in winch that is pulling the pilot into the air.
// server (pilot) sends potentiometer data via RF to client (winch)
// GP Apr 2018 Adapted from Radiohead example sketch showing how to create a simple messageing server
// with the RH_RF95 class.  
//  RH_RF95.cpp  needs to be modified to the relevant frequency of your lora  
//  ie 915khz for the winch project

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;   // Singleton instance of the radio driver

const int numReadings = 3;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int inputPin = A0;


int Sensor1Data;          // pot pin data
char Sensor1CharMsg[4];

void setup() 
{
 for (int thisReading = 0; thisReading < numReadings; thisReading++) {
  readings[thisReading] = 0;
 }
    
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");  
}

void loop()
{
  int i = 0;
  while(i <= numReadings){
    i++;
    // subtract the last reading:
    total = total - readings[readIndex];
    // read from the sensor:
    readings[readIndex] = analogRead(inputPin);
    // add the reading to the total:
    total = total + readings[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;
  
    // if we're at the end of the array...
    if (readIndex >= numReadings) {
      readIndex = 0;
    }
  
    // calculate the average:
    average = total / numReadings;
    delay(10);        // delay in between reads for stability
    Serial.println(average);
  
    Sensor1Data = average;
  }

  // Our ranges are wrong, we need the below:
  // 0-1024 -- 10k POT READ
  // 0-255  -- This relates to the PWM output at the other end. 0 = 0v, 255 = 5v.
  // BUT!!! Our min an MAX on the controller are 1.00 = 0% RPM, 3.8V = 100% RPM
  // To obtain this we will send 0-255 to the winch, and let it figure out voltages. That makes the most sense, and avoids the most converstion.
  
  Sensor1Data = map(Sensor1Data, 0, 1024, 0, 255);
  itoa(Sensor1Data,Sensor1CharMsg,10);      // Convert interger to char*
  
  Serial.print("DEBUG: Throttle: ");
  Serial.println(Sensor1Data);
  
  rf95.send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));
  rf95.waitPacketSent();
}


