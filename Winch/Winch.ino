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
int WinchMotorPin = 3;    //

void setup() 
{
  Serial.begin(9600);
  while (!Serial) ;            // Wait for serial port to be available
  pinMode(WinchMotorPin, OUTPUT);  // Tell Arduino that redLEDPin is an output pin
  Serial.println("Starting...");
  if (rf95.init()){
    Serial.println("RF95 Init succsess");
  }else{
    Serial.println("RF95 Init FAILED");
  }
}

void loop()
{
 // If We have incoming data
 if (rf95.waitAvailableTimeout(300)){    
  
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)){    // Recieve incoming message
        fail=0;
        int i;            
        float WinchVoltage = 0;
        float WinchByte = 0;
        for (i = 0; i < len; i++)
        {              
          Sensor1CharMsg[i] = char(buf[i]);   // not really needed - Fill Sensor1CharMsg Char array with corresponding chars from buf
        }       
        Serial.println(Sensor1CharMsg);
        //***PROBLEM here winchspeed near top of pot range  sometimes crashes. fix buffer
        Sensor1CharMsg[len] = '\0';           // Null terminate the char array otherwise problems  when  incoming message has less digits than the one before.
        WinchSpeed = atoi(Sensor1CharMsg);    // Cast to int

        //Serial.println(WinchSpeed);
        WinchVoltage = mapfloat(WinchSpeed, 0.00, 100.00, 1.00, 3.8);
        WinchByte = mapfloat(WinchVoltage, 1.00, 3.8, 40, 193);
        Serial.println(WinchVoltage);
        Serial.println(WinchByte);
        analogWrite(WinchMotorPin, WinchByte);
    }else{
      Serial.println("recv failed");
      checkKillWinch();
    }
  }
  else
  {
    // Count check/kill
    Serial.println("No Data");
    checkKillWinch();
  }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void checkKillWinch()    //Stop the winch if 3 messages are missed in a row
{
    fail+= 1;
    if (fail >= 3) {
      WinchSpeed=0;          // maybe halving speed each 3 misses is better here
      Serial.println(WinchSpeed);
    }
}


