#include <Servo.h> 
 
Servo servo_direction;  // create servo object to control a servo 
Servo servo_moteur;  // create servo object to control a servo 
 
// Value to change servo stopping point pulse
int servo_direction_stop = 89;  // set the Neutral position for Direction Servo - change as needed
int servo_moteur_stop = 89;  // set the Neutral position for Engine Servo  - change as needed

int servo_direction_droite = servo_direction_stop + 40;
int servo_direction_gauche = servo_direction_stop - 40;
int servo_moteur_avance = servo_moteur_stop + 8;
int servo_moteur_recule = servo_moteur_stop - 8;

// integers to use for updating Servo motors
// change these values to change the various motor actions
int stop_time = 1000;  // stop for 1000 millliseconds = 1 second
int backup_time = 300; // backup for 300 milliseconds = .3 seconds
int turn_time = 300;   // turn (either direction) for 300 milliseconds = .3 seconds

int antennae_L = 3; // connect left antennae sensor to digital pin 3
int antennae_R = 2; // connect right antennae sensor to digital pin 2

// value names used to hold timing variables.
unsigned long timer_startTick;
// value names used to hold antennae states
int antennae_R_val;
int antennae_L_val;

void setup() 
{ 
  Serial.begin(9600);
  servo_direction.attach(12);  // attaches the servo on pin 9 to the servo object
  servo_moteur.attach(10);  // attaches the servo on pin 10 to the servo object
  pinMode(antennae_R, INPUT);  // declare input
  digitalWrite(antennae_R, HIGH); // enable pull-up resistor
  pinMode(antennae_L, INPUT);  // declare input
  digitalWrite(antennae_L, HIGH); // enable pull-up resistor
  arm();
}

void arm()  //intializes the ESC
{
  Serial.println("Arming ESC...");
  servo_moteur.write(servo_moteur_stop);
  delay(1000);
}

void loop() 
{ 
  antennae_R_val = digitalRead(antennae_R); // read Right antennae
  antennae_L_val = digitalRead(antennae_L); // read Left antennae
  
  // Use Antennae sensors
  // check to see if either antennae sensor is equal to GND (it is being touched).
  if (antennae_R_val == 0 || antennae_L_val == 0) {
    // now check to see if only the Left antennae was touched
    if (antennae_R_val == 0 && antennae_L_val == 1) {
      Serial.println("Left");
      timer_startTick = millis();
      stop_motors();
      backup_motors();
      turn_right();
    }
    // otherwise, if the Right sensor was touched and the Left was not
    else if (antennae_R_val == 1 && antennae_L_val == 0) {
      Serial.println("Right");
      timer_startTick = millis();
      stop_motors();
      backup_motors();
      turn_left();
    }
    else {
      // otherwise, both antennae sensors were touched
      Serial.println("Both");
      // turn either direction
      timer_startTick = millis();
      stop_motors();
      backup_motors();
      turn_left();
    }
  }
  else {
    // otherwise no sensors were touched, so go Forward!
    Serial.println("Forward motor");
    forward_motors();
  }
  // print the states of each antennae
  Serial.print("Right sensor ");
  Serial.print(antennae_R_val);
  Serial.print("    ");
  Serial.print("Left sensor ");
  Serial.print(antennae_L_val);
  Serial.print("    ");        
  Serial.print("Moteur ");
  Serial.print(servo_moteur.read());
  Serial.println("    ");        
}
///////////////////// End Loop /////////////////////

// Beginning motor control functions

void stop_motors() {
  // stop motors for the amount of time defined in the "stop_time" variable
  while(millis() < timer_startTick + stop_time){
    servo_moteur.write(servo_moteur_stop);
  }
  timer_startTick = millis();  // reset timer variable
}

void backup_motors() {
  // backup for the amount of time defined in the "backup_time" variable
  while(millis() < timer_startTick + backup_time){
    servo_moteur.write(servo_moteur_recule);
  }
  timer_startTick = millis();  // reset timer variable
}

void forward_motors() {
  // go forward indefinitely
  servo_direction.write(servo_direction_stop);
  servo_moteur.write(servo_moteur_avance);
}

void turn_right() {
  // turn right for the amount of time defined in the "turn_time" variable
  while(millis() < timer_startTick + turn_time) {
    servo_direction.write(servo_direction_droite);
  }
}
void turn_left() {
  // turn left for the amount of time defined in the "turn_time" variable
  while(millis() < timer_startTick + turn_time) {
    servo_direction.write(servo_direction_gauche);
  }
}

