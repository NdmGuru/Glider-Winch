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
 
int led = 8;
int potpin=0;             // initialize analog pin 0
int Sensor1Data;          // pot pin data
char Sensor1CharMsg[4];

void setup() 
{
  pinMode(led, OUTPUT);     
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
//  driver.setTxPower(23, false);
  
}

void loop()
{
  Sensor1Data = analogRead(potpin);         // read from potentiometer
  itoa(Sensor1Data,Sensor1CharMsg,10);      // convert to char data
  Serial.print("Sensor1 Integer: ");
  Serial.print(Sensor1Data);
  Serial.print(" Sensor1 CharMsg: ");
  Serial.print(Sensor1CharMsg);
  Serial.println(" ");
  delay(1000);
  
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      //      RH_RF95::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);

      // Send a reply
//    uint8_t data[]= "And hello back to you";
      rf95.send((uint8_t *)Sensor1CharMsg, strlen(Sensor1CharMsg));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}


