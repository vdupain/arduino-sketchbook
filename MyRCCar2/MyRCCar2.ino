#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include <Servo.h> // bibliotheque servo

// variable de l'interruption du bumper
volatile int choc = 0;

// Initialisation du servo
Servo myservo;  // création de l'objet servo
Servo moteur;  // création de l'objet servo
Servo trajectoire;  // création de l'objet servo
const int tempo = 3; // temps d'attente rotation (règle la vitesse de balayage)
const int balayage = 90; // largeur du balayage
const int centre = 90; // point central
int angle_servo = centre;    // variable de position du servo, initialisée droit devant

// initialisation des télémètres
const int IRpinG = A0;   // pin du télémètre IR gauche
const int IRpinC = A1;   // pin du télémètre IR centre
const int IRpinD = A2;   // pin du télémètre IR droite
const int nombre_mesure = 3; // nombre d'iterations pour la moyenne de mesures de distance
int proche[] = {35, 60, 80}; // distances de déclenchement des actions (en cm)

// initialisation des moteurs
const int vitesseArret = 90;
const int vitesseAfond = vitesseArret + 20;
const int vitesseLente = vitesseArret + 10;
const int ralenti = vitesseLente;

const int min_turn = 100; // temps min pour tourner, ne pas mettre à zéro pour que le char ne se bloque pas à coté d'un obstacle tangent
const int max_turn = 1200; // temps max pour tourner
const int centre_turn = 800; // temps de rotation quand capteur central
int temps = 0; // temps de rotation d'une chenille quand on tourne du fait d'un télémètre
int vitesseEncours = vitesseAfond; // stocke la vitesse en cours

// pour la communication avec le téléphone
AndroidAccessory acc("Vince",
         "ArduinoADK",
         "ArduinoADK",
         "1.0",
         "http://www.android.com",
         "0000000012345678");

void setup() 
{ 
  Serial.begin(115200);
  //Serial.print("\r\nStart");
  // servo
  myservo.attach(9);  // servo sur le pin 9
  moteur.attach(10);  // servo sur le pin 10
  trajectoire.attach(11);  // servo sur le pin 11
  
  // interruption par le bumper (choc détecté quand l'interrupt 0 sur le pin 2 passe de LOW à HIGH)  
  //pinMode(2, INPUT);
  //digitalWrite(2, HIGH); // enable pull-up resistor
  attachInterrupt(0, obstacle, RISING); 
  // interrupts();
  
  // initialisation de l'ESC  
  moteur.write(vitesseArret);
  delay(1000);

  acc.powerOn();
  delay(1000);
} 
 
void loop() { 
 if (choc == 1) { // si un choc a été détecté
   bump();
 }  
 for(angle_servo = (centre-balayage); angle_servo < (centre+balayage); angle_servo++)  // balayage servo autour du point central par incréments d'1 degré
  {                                  
    myservo.write(angle_servo);  // met le servo sur la position en cours
    delay(tempo);        // temps d'attente pour que le servo atteigne sa position
    conduit(angle_servo);        // on roule - on transmet la position du servo
  } 
 for(angle_servo = (centre+balayage); angle_servo >= (centre-balayage); angle_servo--)  // idem avec balayage dans l'autre sens
  {                                
    myservo.write(angle_servo);              
    delay(tempo);                       
    conduit(angle_servo);
  }  
} 

/* fonction d'évitement d'obstacle */
/* analyse les données des télémètres et */
/* prend les décisions adéquates en fonction */
/* de la position du servo*/
  
void conduit(int pos_servo) {
  byte msg[3];
  // mesure de distance sur les 3 capteurs  
  int distanceGauche = calculDistance(IRpinG); 
  int distanceCentre = calculDistance(IRpinC);
  int distanceDroite = calculDistance(IRpinD);
  
  Serial.print(pos_servo);
  Serial.print(":");
  Serial.print(distanceCentre);
  Serial.println();

  // test
  if (pos_servo >=30 && pos_servo <= 45) {
    distanceCentre = 40;
  }
    
  if (acc.isConnected()) {
    Serial.print("Accessory connected. ");
    msg[0] = 0x6;
    msg[1] = pos_servo;
    msg[2] = distanceCentre;
    acc.write(msg, 3);
  }  
  
  // si rien en vue, on met les gaz  
  if (distanceGauche == 150 && distanceDroite == 150 && distanceCentre == 150 && vitesseEncours != vitesseAfond) {
   roule(vitesseAfond);
  }
  
  // si obstacle à moins du premier seuil de déclenchement, on ralentit  
  if ((distanceGauche < proche[2] || distanceCentre < proche[2] || distanceDroite < proche[2]) && 
       (distanceGauche > proche[1] || distanceCentre > proche[1] || distanceDroite > proche[1]) && vitesseEncours != vitesseLente) {
     roule(vitesseLente);
  }
    
  // si obstacle à moins du deuxième seuil, on ralentit encore plus
  if ((distanceGauche <= proche[1] || distanceCentre <= proche[1] || distanceDroite <= proche[1]) && 
     (distanceGauche > proche[0] || distanceCentre > proche[0] || distanceDroite > proche[0]) && vitesseEncours != ralenti) {
    roule(ralenti);
  }
    
  // on observe si un obstacle est à moins du troisième seuil et quel capteur a l'obstacle le plus proche puis
  // on tourne dans la direction opposée proportionnellement à la direction de l'obstacle. 
  // on relance ensuite les moteurs en vitesse lente  
  if (distanceGauche <= proche[0] || distanceCentre <= proche[0] || distanceDroite <= proche[0]) {
    pos_servo = constrain(pos_servo,(centre-balayage),(centre+balayage)); // on limite la position corrigee du servo aux valeurs valides
    
    if (distanceGauche < distanceCentre && distanceGauche < distanceDroite) {      
        temps = map(pos_servo, (centre+balayage),(centre-balayage),min_turn,max_turn); // on tourne d'autant plus que l'angle véhicule-obstacle est aigu
        tourne(45, temps);
        roule(vitesseLente);
      
    } 
    if (distanceDroite < distanceCentre && distanceDroite < distanceGauche) {
        temps = map(pos_servo,(centre-balayage),(centre+balayage),min_turn,max_turn);
        tourne(135, temps);
        roule(vitesseLente);
      
    }
    if (distanceCentre < distanceGauche && distanceCentre < distanceDroite) { // si obstacle en face on brauqe à fond dans la direction opposée
      if (pos_servo > centre){
      tourne(45, centre_turn);
      }
      if (pos_servo <= centre){
       tourne(135, centre_turn);
      }
      roule(vitesseLente);
    }
  }
}

/* fonctions pour tourner */
/* principe général : on s'arrete (fonction halte()), on tourne, on s'arrete */

void tourne(int angle, int delai) {
   halte();
   trajectoire.write(angle);
   delay(delai);
   halte();
}

void demiTour() {
   halte();
   trajectoire.write(90);
   delay(15);
   trajectoire.write(170);
   delay(15);
   trajectoire.write(0);
   delay(15);
   trajectoire.write(90);
   delay(max_turn);
   halte();
}

void marcheArriere() {
   halte();
   moteur.write(ralenti);
   delay(500);
}

void halte() {
   moteur.write(0); // on arrete
   delay(250);
}

void roule(int vitesse) { // fonction pour avancer tout droit à la vitesse indiquée
   moteur.write(vitesse);
   vitesseEncours = vitesse; // on note la vitesse
}

/* calcul de la distance */

int calculDistance(int telemetre) {
  float distance = 0;
  for (int i = 0; i < nombre_mesure; i++) { // on fait la moyenne de (iteration) mesures pour lisser les pics
   float volts = analogRead(telemetre)*0.0048828125;   // valeur du télémètre * (5/1024) - si alimentaiton en 3.3.volts changer 5 en 3.3
   distance = distance + 65*pow(volts, -1.10);
  }
  distance = distance/nombre_mesure;
  
  if (distance > 150 or distance < 20) { // si hors limites on met à 150
    distance = 150;
  }
  
  return(distance);
}

void obstacle() { // en cas d'interrupt du bumper (choc détecté) on met la variable "choc" à 1
   choc = 1;
}

void bump() {
   marcheArriere();
   demiTour();
   choc = 0;
   roule(vitesseLente);
}
