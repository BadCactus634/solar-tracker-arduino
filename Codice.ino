#include <Servo.h>
Servo superiore;
Servo inferiore;
int posUp=0, posDwn=0, servoPinUp=5, servoPinDown=6;
float AS, AD, BS, BD;
int wait=50, buzzer=3, lMov=9, lFondo=10, lPWR=11;
float sZmax=1.05, sZmin=0.88, sXmax=1.05, sXmin=0.88;
bool fondo=false, sottosopra=false;

float rDS, rSAB, rDAB, rAB;

void setup() {
 Serial.begin(9600);
 pinMode(lPWR, OUTPUT);
 pinMode(lMov, OUTPUT);
 pinMode(lFondo, OUTPUT);
 pinMode(buzzer, OUTPUT);
 superiore.attach(servoPinUp); // attaches the servo on servoPin to the servo object
 inferiore.attach(servoPinDown);
 // A0 = AS
 // A1 = BS
 // A2 = BD
 // A3 = AD
 avvio();
 superiore.write(90);
 inferiore.write(90);
}
void loop() {
// Serial.println("-- ready --");
  delay(800);
//  Serial.print("AS: ");
//  Serial.println(AS);
//  Serial.print("BS: ");
//  Serial.println(BS);
//  Serial.print("BD: ");
//  Serial.println(BD);
//  Serial.print("AD: ");
//  Serial.println(AD);
  getFotoresistori();
  controllerPosizione();
}

void avvio(){
  digitalWrite(lPWR, HIGH);
  digitalWrite(lMov, HIGH);
  digitalWrite(lFondo, HIGH);
  tone(buzzer, 600, 100);
  delay(130);
  tone(buzzer, 700, 100);
  delay(130);
  tone(buzzer, 800, 100);
  delay(600);
  digitalWrite(lMov, LOW);
  digitalWrite(lFondo, LOW);
  for(int i=superiore.read(); i<=180; i++){
      superiore.write(i);
      delay(6);
  }
  for(int i=inferiore.read(); i<=180; i++){
      inferiore.write(i);
      delay(6);
  }
  for(int i=superiore.read(); i>=0; i--){
      superiore.write(i);
      delay(6);
  }
  for(int i=inferiore.read(); i>=0; i--){
      inferiore.write(i);
      delay(6);
  }

  tone(buzzer, 600, 400);
  delay(440);
  tone(buzzer, 900, 400);
  delay(400);
  Serial.println("-- ready --");
  digitalWrite(lPWR, HIGH);
}

void getFotoresistori(){
  AS = analogRead(A0);
  BS = analogRead(A1);
  BD = analogRead(A2);
  AD = analogRead(A3);

  rDS=(AS+BS)/(AD+BD); // Destra-Sinistra - 1+ SX è più buia di DX
  rSAB=AS/BS; // Alto-Basso lato Sinistro - 1+ A è più buio di B
  rDAB=AD/BD; // Alto-Basso lato Destro - come sopra
  rAB=(BD+BS)/(AD+AS); // Alto-Basso ambo i lati - 1+ B e più buio di A
}

void controllerPosizione(){
  // Rotazione superiore asse Y
  asseY();

  // Rotazione superiore asse Z
  if(rDS>sZmax || rDS<sZmin){ 
    movA();
  }
}

void movA(){ // Movimento 90^ antiorario (Luce a DX)
  // Asse X (inferiore)
  asseX();
  
  // Asse Y (superiore)
  asseY();
}

void asseY(){
  if(rSAB>sZmax || rDAB>sZmax){ // Se A è più buio di B
  Serial.println("Luce sotto");
  Serial.println("* Vai giù");
  digitalWrite(lMov, HIGH);
  tone(buzzer, 600, 50);
  Serial.println("---------------------");
  for(int i=superiore.read(); i<=180; i++){
    if(rSAB>sZmax || rDAB>sZmax){
      superiore.write(i);
      getFotoresistori();
      delay(15);
    }
  }
  digitalWrite(lMov, LOW);
  }else if(rSAB<sZmin || rDAB<sZmin){ // A più in luce di B
    Serial.println("Luce sopra");
    Serial.println("* Vai su");
    tone(buzzer, 600, 50);
    digitalWrite(lMov, HIGH);
    Serial.println("---------------------");
    for(int i=superiore.read(); i>=0; i--){
      if(rSAB<sZmin || rDAB<sZmin){
        superiore.write(i);
        getFotoresistori();
        delay(15);
      }
    }
    digitalWrite(lMov, LOW);
  }
  if(superiore.read()>=178 || superiore.read()<=2){
    Serial.println("FONDOSCALA Y!");
    Serial.println("---------------------");
    digitalWrite(lFondo, HIGH);
    tone(buzzer, 800, 50);
    delay(100);
    tone(buzzer, 800, 50);
  }else
    digitalWrite(lFondo, LOW);
  if(superiore.read()<90)
    sottosopra=true;
  else
    sottosopra=false;
}

void asseX(){
  if((rDS>sXmax && sottosopra==false) || (rDS<sXmin && sottosopra==true)){
    Serial.println("Luce a DX");
    Serial.println("* Ruota in senso antiorario");
    Serial.println("---------------------");
    tone(buzzer, 600, 50);
    digitalWrite(lMov, HIGH);
    for(int i=inferiore.read(); i<=180; i++){
      if(!(rDS>sXmin&&rDS<sXmax)){
        inferiore.write(i);
        getFotoresistori();
        delay(15);
      }
    }
    digitalWrite(lMov, LOW);
  }else if((rDS<sXmin && sottosopra==false) || (rDS>sXmax && sottosopra==true)){
    Serial.println("Luce a SX");
    Serial.println("* Ruota in senso orario");
    Serial.println("---------------------");
    digitalWrite(lMov, HIGH);
    tone(buzzer, 600, 50);
    for(int i=inferiore.read(); i>=0; i--){
      if(!(rDS>sXmin&&rDS<sXmax)){
        inferiore.write(i);
        getFotoresistori();
        delay(15);
      }
    }
    digitalWrite(lMov, LOW);
  }
  if(inferiore.read()>=178 && fondo==false){
    Serial.println("FONDOSCALA X 1!");
    digitalWrite(lFondo, HIGH);
    // Ruota di 180^ l'asse X
    digitalWrite(lMov, HIGH);
    tone(buzzer, 800, 50);
    delay(100);
    tone(buzzer, 800, 50);
    for(int i=inferiore.read(); i>=0; i--){
        inferiore.write(i);
        delay(15);
    }
    digitalWrite(lMov, LOW);
    fondo=true;
  }else if(inferiore.read()<=2 && fondo==false){
    Serial.println("FONDOSCALA X 2!");
    digitalWrite(lFondo, HIGH);
    // Ruota di 180^ l'asse X
    digitalWrite(lMov, HIGH);
    tone(buzzer, 800, 50);
    delay(100);
    tone(buzzer, 800, 50);
    for(int i=inferiore.read(); i<=180; i++){
      inferiore.write(i);
      delay(15);
    }
    digitalWrite(lMov, LOW);
    fondo=true;
  }else{
    fondo=false;
    digitalWrite(lFondo, LOW);
  }
  Serial.println("---------------------");
   asseY();
}
