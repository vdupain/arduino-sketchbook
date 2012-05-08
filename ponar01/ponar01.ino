/*
 ponar01
 Eric Pavey - 2011-08-07
 www.akeric.com
 
 ping))) + servo = ponar
 -------
 
 Hardware:
 Arduino Uno
 http://arduino.cc/en/Main/ArduinoBoardUno
 
 Paralax ping))) :  
 Power requirements:  +5 VDC. 30 mA typ; 35 mA max
 http://www.parallax.com/tabid/768/ProductID/92/Default.aspx
 
 Paralax (Fubata) "standard servo": 
 Power requirements: 4 to 6 VDC*; Maximum current draw is 140 +/- 50 mA at 6 VCDC when 
 operating in no load conditions, 15 mA when in static state. 
 This is found in the "PING))) Mounting Bracket Kit":
 http://www.parallax.com/Store/Robots/RoboticAccessories/tabid/145/CategoryID/22/List/0/SortField/0/Level/a/ProductID/248/Default.aspx
 
 Erector sets to hook it all together.
 ------
 
 Code inspiration:
 Servo code based on the example: http://arduino.cc/en/Tutorial/Sweep
 Ping))) code based on the example: http://www.arduino.cc/en/Tutorial/Ping
 ------
 
 Setup:
 Components +V is hooked into Arduino's 5v pin (via breadboard).
 Components ground is hooked to Arduino's ground pin (via breadboard).
 ping))) signal wire it hooked to Arduino digital pin 7.
 Servo signal wire is hooked to Arduino digital pin 9.
 */

#include <Servo.h> 

//--------------------------------------
// User globals:

// These define the min\max rotation for the servo.  Based on how
// I set it up, 90 deg is 'straight ahead', so 0 deg would be full
// left, and 180 deg is full right.  The servo can go from 0 to 180.
// The default settings have a 90 deg sweet, centered on 'straight ahead'.
int minSweep = 45;
int maxSweep = 135;

int pingPin = 7;  // ping))) signal (digital) pin.
int servoPin = 9; // servo signal (digital) pin.

//--------------------------------------
// System globals:
Servo myservo;  // create servo object to control a servo 
int pos = minSweep;    // variable to store the servo position 
int dir = 1; // positive or negative to define direction

//--------------------------------------
void setup() 
{ 
  myservo.attach(servoPin);
  Serial.begin(9600);
} 

//--------------------------------------
void loop(){
  long cm, duration;

  // Move servo:
  myservo.write(pos); 

  // Define next servo position:
  if (dir == 1){
    if(pos < maxSweep){
      pos += 1;
    }
    else{
      dir = -1;
    }
  }
  else if(dir == -1){
    if(pos > minSweep){
      pos -= 1;
    }
    else{
      dir = 1;
    }    
  }

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);  

  cm = microsecondsToCentimeters(duration);

  // print results to the serial port in the form "# #", or "degrees distance":
  // The Processing sketch is designed to read them in this form.
  Serial.print(pos);
  Serial.print(" ");
  Serial.print(cm);
  Serial.println();

  // give servo a break:
  delay(15);
}

//--------------------------------------
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  //return microseconds / 29 / 2;
  return microseconds / 58;
}
