/*
RFID Evaluation Shield - 13.56MHz avec RFID Module - SM130 Mifare (13.56 MHz)
 Ce sketch lit le type de Tag, son numéro d'identité, puis vérifie l'absence de clé de contrôle
 et s'il n'y en a pas, lit le block, itération sur les 64 blocks d'un Tag 1k
 */


#include <SoftwareSerial.h>
SoftwareSerial rfid(7, 8);


//Prototypes
void Activation(void);
void Analyse(void);
void Authentication(void);
void Cherche_Tag(void);
//void Cle_Ecrite(void);
void Control_Presence(void);
void Data_Ecrite(void);
//void Ecrire_Block(void);
//void Ecrire_Cle(void);
void Imprime_ID_Tag(void);
void Lecture_Block(void);
void Verification(void);


// commande
int Cherche = 130;
int SelecTag = 131;
int Controle=133; // Authentification
int Lire4Block = 134;
int Lire1Block= 135;
//int Ecri4Block = 137;
//int Ecri1Block=138;
//int EcriCle=140;
//int AntenneOnOff=144;
//int On=1;
//int Off=0;
//int CleA=170;
//int CleB=187;
int Pas2Cle=255;
int Active=147; // Halt
byte Positif=76; //réponse renvoyé en cas de succès


// Variable
int Commande;
int Longueur;
//int Code[6];
int Data[16];
int ID[16];
int CleVerif[3];
int Block;
int Secteur;
int TypeCle;
int Present;
int a;
float b1;
int b2;
int bob;
//_____________________________________________________________
void setup()
{
  Serial.begin(9600);
  Serial.println("Demarrage");
  rfid.begin(19200);
  delay(10);
  Present =0;
  activation();
}
//_____________________________________________________________
void activation() // active les commande du tag
{
  Commande=Active;
  Longueur=1;
  CleVerif[1]=Longueur+Commande;
  rfid.write(byte(255));         // Toujours égale à 255
  rfid.write(byte(0));           // Toujours égale à 0
  rfid.write(byte(Longueur));    // mettre le nombre de valeur envoyé la commande et les données s'il y en a, ici il n'y a qu'1 commande
  rfid.write(byte(Commande));    // C'est la commande
  rfid.write(byte(CleVerif[1])); // Clé de contrôle qui est la somme des valeurs de la longueur, de la commande et des données quand il y en a
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
      for(int i=1;i<17;i++){
        ID[i]= rfid.read();
      }
    }
  }
}
//_____________________________________________________________
void Control_Presence()
{
  if(ID[2] == 6){
    Present++;
  }
  if(ID[2] == 2){
    Present = 0;
  }
}
//_____________________________________________________________
void Imprime_ID_Tag()    // imprime le numéro du tag
{
  if(Present == 1){
    Serial.print("Tag Type : ");
    if (ID[4]==1){
      Serial.println("Mifare Ultralight ");
    }
    else if (ID[4]==2){
      Serial.println("Mifare Standard 1K ");
    }
    else if (ID[4]==3){
      Serial.println("Mifare Classic 4K ");
    }
    else {
      Serial.println("Non reconnu ");
    }
    Serial.print("Tag ID : ");
    for(int i=5;i<9;i++){
      Serial.print(ID[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    Authentication();
  }
  delay(50);
}
//_____________________________________________________________
void Authentication() // Controle l'absence de clé
{
  for (int Block=0;Block<64;Block++){
    Commande = Controle; //Commande d'authentification
    Longueur=3;
    CleVerif[1]=Longueur+Commande+Block+Pas2Cle;


    //Serial.println(Block);


    // commande qui donne accès au block s'il n'y à pas de clé
    rfid.write(byte(255));
    rfid.write(byte(0));
    rfid.write(byte(Longueur));
    rfid.write(byte(Commande));
    rfid.write(byte(Block));        // numéro du block a authentifier
    rfid.write(byte(Pas2Cle));    // pas de clé
    rfid.write(byte(CleVerif[1]));


    //recupère la response
    delay(50);
    while(rfid.available()){
      if(rfid.read() == 255){
        for(int i=1;i<6;i++){
          Data[i]= rfid.read();
        }
      }
    }
    if(Data[4] == Positif){
      Serial.print("OK ");
      bob=Block;
      Lecture_Block();
    }
    else {
      Serial.print("Cle active sur le block : ");
      Serial.println(Block);
    }
    delay(100);
  }
  Present=1;
}
//_____________________________________________________________
void Lecture_Block()
{
  Block=bob;
  Commande=Lire4Block;
  Longueur=2;
  CleVerif[2]=Longueur+Commande+Block;
  rfid.write(byte(255));
  rfid.write(byte(0));
  rfid.write(byte(Longueur));
  rfid.write(byte(Commande)); // Lire 4 block
  rfid.write(byte(Block)); //numéro du premier block
  rfid.write(byte(CleVerif[2]));


  //recupère la response
  delay(50);
  while(rfid.available()){
    if(rfid.read() == 255){
      for(int i=1;i<22;i++){
        Data[i]= rfid.read();
      }
    }
  }
  //afficher le block sous une forme particulière pour plus de clarté
  a=0;
  for(int i=1;i<22;i++){
    Serial.print(Data[i], HEX);
    Serial.print(" ");
    a+=1;
    b1=(float)(Block+1)/4;
    b2=(int)(Block+1)/4;
    if (b1==b2){
      if (a==4){
        Serial.print("- ");
      }
      if (a==10){
        Serial.print("- ");
      }
      if (a==14){
        Serial.print("- ");
      }
      if (a==20){
        Serial.print("- - ");
      }
    }
    else{
      if (a==4){
        Serial.print("- ");
      }
      if (a==8){
        Serial.print("- ");
      }
      if (a==12){
        Serial.print("- ");
      }
      if (a==16){
        Serial.print("- ");
      }
      if (a==20){
        Serial.print("- ");
      }
    }
  }
  Serial.println();
}


