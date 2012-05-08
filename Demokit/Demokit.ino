#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include <Servo.h>

#define  LED1_RED       3
#define  LED1_GREEN     5
#define  LED1_BLUE      6
#define  SERVO1         9
#define  LIGHT_SENSOR   A2
#define  TEMP_SENSOR    A3
#define  BUTTON1        A4

AndroidAccessory acc("Google, Inc.",
         "DemoKit",
         "DemoKit Arduino Board",
         "1.0",
         "http://www.android.com",
         "0000000012345678");
         
Servo servo;

void init_buttons() {
	pinMode(BUTTON1, INPUT);
	// enable the internal pullups
	digitalWrite(BUTTON1, HIGH);
}

void init_leds() {
	pinMode(LED1_RED, OUTPUT);
	pinMode(LED1_GREEN, OUTPUT);
	pinMode(LED1_BLUE, OUTPUT);

	analogWrite(LED1_RED, 255);
	analogWrite(LED1_GREEN, 255);
	analogWrite(LED1_BLUE, 255);
}

byte b1;

void setup() {
	Serial.begin(115200);
	//Serial.print("\r\nStart");

	init_leds();
	init_buttons();
	servo.attach(SERVO1);
	servo.write(90);
	b1 = digitalRead(BUTTON1);

	acc.powerOn();
        delay(1000);
}

void loop() {
	byte err;
	byte idle;
	static byte count = 0;
	byte msg[3];

	if (acc.isConnected()) {
                //Serial.println("Accessory connected. ");

		int len = acc.read(msg, sizeof(msg), 1);
		int i;
		byte b;
		uint16_t val;
		int x, y;
		char c0;

		if (len > 0) {
			// assumes only one command per packet
			if (msg[0] == 0x2) {
				if (msg[1] == 0x0)
					analogWrite(LED1_RED, 255 - msg[2]);
				else if (msg[1] == 0x1)
					analogWrite(LED1_GREEN, 255 - msg[2]);
				else if (msg[1] == 0x2)
					analogWrite(LED1_BLUE, 255 - msg[2]);
				else if (msg[1] == 0x10)
					servo.write(map(msg[2], 0, 255, 0, 180));
			}
		}

		msg[0] = 0x1;

		b = digitalRead(BUTTON1);
		if (b != b1) {
			msg[1] = 0;
			msg[2] = b ? 0 : 1;
			acc.write(msg, 3);
			b1 = b;
		}

		switch (count++ % 0x10) {
		case 0:
			val = analogRead(TEMP_SENSOR);
			msg[0] = 0x4;
			msg[1] = val >> 8;
			msg[2] = val & 0xff;
			acc.write(msg, 3);
			break;

		case 0x4:
			val = analogRead(LIGHT_SENSOR);
			msg[0] = 0x5;
			msg[1] = val >> 8;
			msg[2] = val & 0xff;
			acc.write(msg, 3);
			break;
		}
	} else {
		// reset outputs to default values on disconnect
		analogWrite(LED1_RED, 255);
		analogWrite(LED1_GREEN, 255);
		analogWrite(LED1_BLUE, 255);
		servo.write(90);
	}
	delay(10);
}

