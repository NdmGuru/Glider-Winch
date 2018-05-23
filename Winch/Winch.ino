// WINCH controller
// The winch controller receives potentiometer data via RF from the pilot.
// GP April 2018  Adapted from Radiohead example sketch showing how to create a simple messageing client
// with the RH_RF95 class.  


#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;             // Singleton instance of the radio driver
int WinchSpeed;           //   the pot reading from the pilot's controller
char Sensor1CharMsg[4];   // winchspeed data message
int fail =0;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) ;            // Wait for serial port to be available
  
  if (!rf95.init())
    Serial.println("init failed");

}

void loop()
{
  uint8_t data[] = "Hi";               // not really needed anymore
  rf95.send(data, sizeof(data));
  
  rf95.waitPacketSent();                 // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000))
  {    
    if (rf95.recv(buf, &len))    // Server (pilot end) starts sending winchspeed once it has been pinged with a "hi"
   {
      // debug  // Serial.print("winch got a speed update: ");
        fail=0;
        int i;            
        for (i = 0; i < len; i++)
        {              
          Sensor1CharMsg[i] = char(buf[i]);   // not really needed - Fill Sensor1CharMsg Char array with corresponding chars from buf
        }       
                     //***PROBLEM here winchspeed near top of pot range  sometimes crashes. fix buffer
        Sensor1CharMsg[len] = '\0';           // Null terminate the char array otherwise problems  when  incoming message has less digits than the one before.
        WinchSpeed = atoi(Sensor1CharMsg);    // Cast to int
        Serial.println(WinchSpeed);
    
    }
    else
    {
      Serial.println("recv failed");
      checkKillWinch();
    }
  }
  else
  {
    //  debug only:  Serial.println("No reply, is rf95_server running?");
    checkKillWinch();
  }
  delay(400);
}

void checkKillWinch()    //Stop the winch if 3 messages are missed in a row
{
    fail+= 1;
    if (fail >= 3) {
      WinchSpeed=0;          // maybe halving speed each 3 misses is better here
      Serial.println(WinchSpeed);
    }
}


