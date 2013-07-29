#include <Usb.h>
#include <adk.h>
#include <Servo.h>

#define  SERVO1         5
#define  SERVO2         6

USB Usb;
ADK adk(&Usb,"Vince", 
"ArduinoADK", 
"ArduinoADK", 
"1.0", 
"http://www.android.com", 
"0000000012345678");

Servo servo1, servo2;

void setup() {
  Serial.begin(115200);
  Serial.print("\r\nStart");

  servo1.attach(SERVO1);
  servo1.write(90);
  servo2.attach(SERVO2);
  servo2.write(90);

  if (Usb.Init() == -1) {
    Serial.print("\r\nOSCOKIRQ failed to assert");
    while(1); //halt
  }
  delay(1000);
}

void loop() {
  Usb.Task();
  if(adk.isReady()) {
    uint8_t msg[3];
    uint16_t len = sizeof(msg);
    uint8_t rcode = adk.RcvData(&len, msg);
    if(rcode && rcode != hrNAK)
      USBTRACE2("Data rcv. :", rcode);
    if (len > 0) {
      // assumes only one command per packet
      if (msg[0] == 0x2) {
        if (msg[1] == 0x1) {
          //Serial.println("Servo1=" + msg[2]);
          servo1.write(constrain(msg[2], 0, 180));
        } 
        else if (msg[1] == 0x2) {
          //Serial.println("Servo2=" + msg[2]);
          servo2.write(constrain(msg[2], 0, 180));
        }
      } 
      else if (msg[0] == 0x3) {
        servo1.write(constrain(msg[1], 0, 180));
        servo2.write(constrain(msg[2], 0, 180));
      }
    }
  } 
  else {
    // reset outputs to default values on disconnect
    servo1.write(90);
    servo2.write(90);
  }
  delay(10);
}




