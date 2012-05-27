#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include <Servo.h>

#define  SERVO1         5
#define  SERVO2         6

AndroidAccessory acc("Vince", 
"ArduinoADK", 
"ArduinoADK", 
"1.0", 
"http://www.android.com", 
"0000000012345678");

Servo servo1, servo2;
boolean handshakeOk = false;

void setup() {
  Serial.begin(115200);

  servo1.attach(SERVO1);
  servo1.write(90);
  servo2.attach(SERVO2);
  servo2.write(90);

  acc.powerOn();
  delay(1000);
}

void loop() {
  byte err;
  byte idle;
  static byte count = 0;
  byte msg[3];

  if (acc.isConnected()) {
    if (!handshakeOk) {
      // Do handshake.
      msg[0] = 'X';
      msg[1] = 'X';
      msg[2] = 0x01;
      acc.write(msg, sizeof(msg));
      // Wait for response.
      unsigned long timeout = millis() + 2000;
      boolean timedOut = true;
      int len;
      while (millis () < timeout) {
        len = acc.read(msg, sizeof(msg), 1);
        if (len > 0) {
          timedOut = false;
          break;
        }
      }

      if (timedOut 
        || (len < 3) 
        || !((msg[0] == 'X') && (msg[1] == 'X') && (msg[2] == 0x01))) {
        // No response, short response or bad response received so we
        // assume it's an "old" version.
        // TODO: Print error message to Serial for debugging?
        // Serial.println("Version mismatch.");
        return;
      }
      handshakeOk = true;
    }
    int len = acc.read(msg, sizeof(msg), 1);
    if (len > 0) {
      // assumes only one command per packet
      if (msg[0] == 0x2) {
        if (msg[1] == 0x1) {
          servo1.write(map(msg[2], 0, 255, 0, 180));
        } 
        else if (msg[1] == 0x2) {
          servo2.write(map(msg[2], 0, 255, 0, 180));
        }
      } else if (msg[0] == 0x3) {
          servo1.write(map(msg[1], 0, 255, 0, 180));
          servo2.write(map(msg[2], 0, 255, 0, 180));
      }
    }
    msg[0] = 0x1;
    switch (count++ % 0x10) {
      case 0:
	msg[0] = 0x4;
	msg[1] = servo1.read();
	msg[2] = servo2.read();
	acc.write(msg, 3);
	break;
    }
  } 
  else {
    // reset outputs to default values on disconnect
    servo1.write(90);
    servo2.write(90);
  }
  delay(10);
}

