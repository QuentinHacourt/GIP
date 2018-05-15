#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define ACHTERUIT  200 //de in te geven waarde om de motor achteruit te doen draaien
#define VOORUIT  600 //de in te geven waarde om de motor vooruit te doen draaien
#define STOP 350 //de in te geven waarde om de motor te stoppen
#define trigPin 3
#define echoPin 2
#define tracker1 A1 //rechts vooraan
#define tracker2 A2 //links achteraan
#define tracker3 A3 //rechts achteraan
#define tracker4 A4 //links vooraan



int CheckVast(int tabel[50][2]);
void checkForWalls();
void followRightWall();
void checkRichting();
void updateXandY();
void links();
void rechts();
void rechtDoor();
void motorRelease();
int meetMuur();
void checkDoel();
void randen();

// een array aanmaken die alle nodige informatie over het doolhof bevat.
// de array bestaat uit 5 lagen van een 4x4 tabel
// de laag 0 stelt de vakjes voor die wel of niet bezocht zijn als de zoek o matic start 
// zal in het vakje 0 staan en als de zoek o matic er al is geweest zal daar een 1 komen
// de lagen 1,2,3,4 stellen de 4 mogelijke richtingen voor en of daar een muur staat.
// laag 1 staat voor de rechterkant, laag 2 de onderkant, laag 3 de linker kant en laag 4 de boven kant
// 1 stelt een muur voor en 0 stelt geen muur voor.

int DOOLHOF[3][3][4];

//de coordinaten waarin de zoek o matic zich bevinden dimensioneren

int X, Y;

//de richting waarnaar de zoek o matic kijkt dimensioneren

int RICHTING = 1;

void setup()
{
  Serial.println("Start!");
  pinMode (trigPin, OUTPUT);
  pinMode (echoPin, INPUT);
  pinMode (tracker1, INPUT);
  pinMode (tracker2, INPUT);
  pinMode (tracker3, INPUT);
  pinMode (tracker4, INPUT);
  randen();
  Serial.begin(9600); // set up Serial library at 9600 bps
  
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

void loop()
{
  static int teller, plaatsEnRichting[50][2];

 
 
 
  //nakijken of de zoek o matic al in dit vakje is aangekomen met dezelfde richting
  if(!CheckVast(plaatsEnRichting))
  {
  // de rechter muur volgen
  checkForWalls();
  followRightWall();
  updateXandY();
  }
  else
  {
  while (1)
  {}
  }
  plaatsEnRichting[teller][0] = X;
  plaatsEnRichting[teller][1] = Y;
  plaatsEnRichting[teller][2] = RICHTING;
  teller++;
 
}


//====================================
//        zoeken
//====================================

int CheckVast(int tabel[50][2])
{
  int i;
  for (i = 0; i < 50; i++)
  {
  if ((X ==  tabel[i][0]) && (Y ==  tabel[i][1]) && (RICHTING == tabel[i][2]))
  {
    return(1);
  }
  else
  {
  return(0);
  }
  }
}

void checkForWalls()
{
  int i;
 
  // meten of de zoek o matic zijn doel heeft berijkt,
  // als de zoek o matic zijn doel niet heeft berijkt
  // wordt de coordinaat als "1" gemarkeerd
  checkDoel();
 
  // in de 4 richtingen rond de zoek o matic kijken
  // en eventuele muren noteren
  for(i=0; i<4; i++)
   {
    DOOLHOF[X][Y][RICHTING] = meetMuur();
    rechts();
    motorRelease();
    delay(500);
    RICHTING++;
    checkRichting();
   }
   for(i=1; i<5; i++)
   {
    Serial.println(DOOLHOF[X][Y][i]);
   }
}



void followRightWall()
{
  //een nieuwe variabele aanmaken die de richting voorstelt waar 
  //de zoek-o-matic eventueel naartoe zou rijden
  int R = RICHTING;
  R += 1;
  if (RICHTING > 4)
   {
    RICHTING -= 4;
   }
   else if (RICHTING < 1)
   {
    RICHTING += 4;
   }
 
  if (DOOLHOF[X][Y][R] == 0)
  {
    Serial.println("rechts!");
    rechts();
    rechtDoor();
    motorRelease();
    RICHTING++;
    return;
  }
   
  R = RICHTING;
  if (RICHTING > 4)
   {
    RICHTING -= 4;
   }
   else if (RICHTING < 1)
   {
    RICHTING += 4;
   }
   
   if(DOOLHOF[X][Y][R] == 0)
  {
    Serial.println("rechtdoor");
    rechtDoor();
    motorRelease();
    return;
  }
   
   R = RICHTING;
   R += 3;
   if (RICHTING > 4)
   {
    RICHTING -= 4;
   }
   else if (RICHTING < 1)
   {
    RICHTING += 4;
   }
   
   if(DOOLHOF[X][Y][R] == 0)
  {
    Serial.println("links");
    links();
    rechtDoor();
    motorRelease();
    RICHTING--;
    return;
  }
    
  R = RICHTING;
  R +=2;
  if (R > 4)
   {
    R -= 4;
   }
   else if (R < 1)
   {
    R += 4;
   }
    
  if(DOOLHOF[X][Y][R] == 0)
  {
    Serial.println("achteruit");
    links();
    motorRelease();
    delay(500);
    links();
   
    rechtDoor();
   
    motorRelease();
    RICHTING+=2;
    return;
  }
  checkRichting();
}


void checkRichting()
{
   //richting moet tussen 1 en 4 zijn
   if (RICHTING > 4)
   {
    RICHTING -= 4;
   }
   else if (RICHTING < 1)
   {
    RICHTING += 4;
   }
}

void updateXandY()
{
  switch (RICHTING)
  case 1: 
  {
    X++;
    break;
  
  case 2:  
    Y--;
    break;
  
  case 3:  
    X--;
    break;
  
  case 4:
    Y++;
    break;
  }
  
}

void randen()
{
  //============================================================================================
  //                                   randen invoeren
  //
  // de binnen muren kunnen verplaatst worden maar de buitenste muren zullen er altijd zijn
  //============================================================================================
 
  int i, j;
 
  for (i = 0; i<4; i++)
  {
  for (j=0; j<4; j++)
  {
    //Rechter rand
    if (i==3)
  {
    DOOLHOF[i][j][1] = 1;
  }
  //Onder rand
    if (j==0)
  {
    DOOLHOF[i][j][2] = 1;
  }
  //Linker rand
    if (i==0)
  {
    DOOLHOF[i][j][3] = 1;
  }
  //Boven rand
    if (j==3)
  {
    DOOLHOF[i][j][4] = 1;
  }
  }
  }
  
}


//====================================
//      sensoren & motoren
//====================================

int tracker (int number)
{
  //0 = wit, 1 is zwart
  if (analogRead(number) < 500)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}


void links()
{
 pwm.setPWM(0, 0, ACHTERUIT);
 pwm.setPWM(1, 0, ACHTERUIT);
 pwm.setPWM(2, 0, ACHTERUIT);
 pwm.setPWM(3, 0, ACHTERUIT);
 delay(600);
}

void rechts()
{
 pwm.setPWM(0, 0, VOORUIT);
 pwm.setPWM(1, 0, VOORUIT);
 pwm.setPWM(2, 0, VOORUIT);
 pwm.setPWM(3, 0, VOORUIT);
 delay(600);
}

void motorRelease()
{
 pwm.setPWM(0, 0, STOP);
 pwm.setPWM(1, 0, STOP);
 pwm.setPWM(2, 0, STOP);
 pwm.setPWM(3, 0, STOP);
}

void rechtDoor()
{
 
while (tracker(tracker1) + tracker(tracker4) < 2)
{
  if (tracker(tracker1) + tracker(tracker4) == 0)
  {
    pwm.setPWM(0, 0, ACHTERUIT);
    pwm.setPWM(1, 0, ACHTERUIT);
    pwm.setPWM(2, 0, VOORUIT);
    pwm.setPWM(3, 0, VOORUIT);
  }
  else if (tracker(tracker1))
  {
     pwm.setPWM(0, 0, VOORUIT);
     pwm.setPWM(1, 0, VOORUIT);
     pwm.setPWM(2, 0, VOORUIT);
     pwm.setPWM(3, 0, VOORUIT);
  }
  else if (tracker(tracker4))
  {
    pwm.setPWM(0, 0, ACHTERUIT);
    pwm.setPWM(1, 0, ACHTERUIT);
    pwm.setPWM(2, 0, ACHTERUIT);
    pwm.setPWM(3, 0, ACHTERUIT);
  }
  }
    pwm.setPWM(0, 0, ACHTERUIT);
    pwm.setPWM(1, 0, ACHTERUIT);
    pwm.setPWM(2, 0, VOORUIT);
    pwm.setPWM(3, 0, VOORUIT);
    delay(250);
    motorRelease();
}
  
int meetMuur()
{
  // meten of er recht vooraan een muur is
  // muur = 1, geen muur = 0
  int duration, distance;
  digitalWrite (trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.print(" cm\n");
 if (distance < 25  && distance >= 0)
 {
  return 1 ;
 }
 else
 {
  return 0 ;
 }
}

void checkDoel()
{
  //nakijken of het doel gevonden is
  int gevonden;
  gevonden = tracker(tracker1) + tracker(tracker2) + tracker(tracker3) + tracker(tracker4);
  if (gevonden == 4)
  {
   while (1)
    { }
  }
}
