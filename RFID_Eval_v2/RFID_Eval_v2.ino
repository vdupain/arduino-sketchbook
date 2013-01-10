/*

 RFID Evaluation Shield - 13.56MHz avec RFID Module - SM130 Mifare (13.56 MHz)
 
 
 
 Ce sketch ne fait que lire le type de Tag et son numéro d'identité
 
 Pour passer de la version 0023 à la version 0.1
 
 Il faut remplacer :
 
 NewSoftSerial.h --> SoftwareSerial.h
 
 NewSoftSerial rfid(7, 8 ) -->  SoftwareSerial rfid(7, 8);
 
 rfid.print(255, BYTE);  -->  rfid.write(byte(255));
 
 rfid.print(A, BYTE) --> rfid.write(byte(A))
 
 
 
 Note: pour V0023 utilisez la librairie NewSoftSerial
 
 http://arduiniana.org/libraries/newsoftserial/
 
 */


#include <SoftwareSerial.h>
SoftwareSerial rfid(7, 8);


// commande
int Cherche = 130;
int SelecTag = 131;
int Controle=133; // Authentification
int Lire4Block = 134;
int Lire1Block= 135;
int Ecri4Block = 137;
int Ecri1Block=138;
int EcriCle=140;
int AntenneOnOff=144;
int On=1;
int Off=0;
int CleA=170;
int CleB=187;
int Pas2Cle=255;
int Active=147; // Halt
byte Positif=76; //réponse renvoyé en cas de succé


// Variable
int Commande;
int Longueur;
int Code[6];
int Data[16];
int DataE[16];
int SomDataE;
int SomCode;
int CleVerif[6];
int Block;
int BlockS;
int Secteur;
int TypeCle;
int Present;
int a;
float b1;
int b2;
int i;
//_____________________________________________________________
void setup()
{
  Serial.begin(9600);
  Serial.println(F("Demarrage")); // le F( permet de ne pas stocker le contenu de la () dans la RAM mais ne fonctionne qu'avec la version 1.0
  rfid.begin(19200);
  delay(10);
  activation();
}
//_____________________________________________________________
void activation() // active les commande du tag
{
  Commande=Active;
  Longueur=1;
  CleVerif[1]=Longueur+Commande;
  rfid.write(byte(255));
  rfid.write(byte(0));
  rfid.write(byte(Longueur));
  rfid.write(byte(Commande));
  rfid.write(byte(CleVerif[1]));
}

//_____________________________________________________________
void loop()
{
  Cherche_Tag();
  Analyse();
  Control_Presence();
  Imprime_ID_Tag();
  delay(100);
}
//_____________________________________________________________
void Cherche_Tag()   //Cherche la présence d'un tag
{
  Commande=Cherche;
  Longueur=1;
  CleVerif[1]=Longueur+Commande;
  rfid.write(byte(255));
  rfid.write(byte(0));
  rfid.write(byte(Longueur));
  rfid.write(byte(Commande));
  rfid.write(byte(CleVerif[1]));
  delay(50);
}
//_____________________________________________________________
void Analyse()
{
  while(rfid.available()){
    if(rfid.read() == 255){
      for(int i=1;i<11;i++){
        Data[i]= rfid.read();
      }
    }
  }
}
//_____________________________________________________________
void Control_Presence()
{
  if(Data[2] == 6){
    Present++;
  }
  if(Data[2] == 2){
    Present = 0;
  }
}
//_____________________________________________________________
void Imprime_ID_Tag()    //imprime le numéro du tag
{
  if(Present == 1){
    Serial.print(F("Tag Type : "));
    if (Data[4]==1){
      Serial.println(F("Mifare Ultralight "));
    }
    else if (Data[4]==2){
      Serial.println(F("Mifare Standard 1K "));
    }
    else if (Data[4]==3){
      Serial.println(F("Mifare Classic 4K "));
    }
    else {
      Serial.println(F("Non reconnu "));
      for(int i=1;i<5;i++){
        Serial.print(Data[i], HEX);
        Serial.print(F(" "));
      }
    }
    Serial.println();
    Serial.print(F("Tag ID : "));
    for(int i=5;i<9;i++){
      Serial.print(Data[i], HEX);
      Serial.print(F(" "));
    } 
    Serial.println();
  }
  delay(50);
}

