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
  
  // Map the data from 0-100
  // NB: Need to save the min and max to eeprom and add a config mode. 
  Sensor1Data = map(Sensor1Data, 0, 1024, 0, 100);
  itoa(Sensor1Data,Sensor1CharMsg,10);      // Convert interger to char*
  
  Serial.print("Sensor1 Integer: ");
  Serial.print(Sensor1Data);
      
      delay(300);
      rf95.send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
}


