#include <MFRC522.h>
#include <EEPROM.h>
#include <SPI.h>

MFRC522 okuyucu(10,9);

String master = "146,237,34,28", hunkarimiz="91,248,36,10";
int masterSayac = 0, kartAdedi=0;
String kart=" ",tutucu=" ";
int m1 = 6, m2 = 7, me = 5;
int switch1 = 2, switch2 = 3; 

void setup(){
  Serial.begin(9600);
  SPI.begin();
  okuyucu.PCD_Init();
  pinMode(A4,OUTPUT); // led
  pinMode(A5,OUTPUT); // led
  pinMode(A3,OUTPUT); // buzzer
  pinMode(A0,INPUT); // optik sensör
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(me, OUTPUT);
  digitalWrite(me,LOW);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(4, INPUT); // PIR sensör

   
  for(int i = 0; i < 1024; i+=4){
   if(EEPROM.read(i)== 0){
    break;
   }
   kartAdedi++;
  }
}

void loop(){
  
  if ( ! okuyucu.PICC_IsNewCardPresent()){
    return;
  }
  if ( ! okuyucu.PICC_ReadCardSerial()){
    return;
  }

  kart = String(okuyucu.uid.uidByte[0])+","+String(okuyucu.uid.uidByte[1])+","+String(okuyucu.uid.uidByte[2])+","+String(okuyucu.uid.uidByte[3]);

  if(kart == hunkarimiz){
    girisBasarili();
    return;
  }
  
  Serial.println(kart);
  
  if (master == kart){
    masterSayac++;
    sariYan();
    delay(100);
    son();
    return;
  }
  if(masterSayac<10 && masterSayac>0){
    kartKaydet();
    masterSayac=0;
    return;
  }
  if(masterSayac>10){
     kartSil();
    masterSayac=0;
    return;
  }

  for(int i = 0; i < kartAdedi*4; i+=4){ 
    
    tutucu = (String)EEPROM.read(i)+","+(String)EEPROM.read(i+1)+","+(String)EEPROM.read(i+2)+","+(String)EEPROM.read(i+3);
    if(tutucu == kart){
      girisBasarili();
      return;
    }
  }
  girisBasarisiz();
  delay(1000);
  
}



void kartKaydet(){
  
  for(int i = 0; i < kartAdedi*4; i+=4){
    tutucu = (String)EEPROM.read(i)+","+(String)EEPROM.read(i+1)+","+(String)EEPROM.read(i+2)+","+(String)EEPROM.read(i+3); 
    if(tutucu == kart){
      kartKaydetF();
      delay(1000);
      return;
    }
  }
  for(int i = 0; i<4; i++){
    EEPROM.write(((kartAdedi*4)+i),okuyucu.uid.uidByte[i]);
  }
  kartKaydetP();
  delay(1000);
}



void kartSil(){
  int sayac = 1;
  for(int i = 0; i < kartAdedi*4; i+=4){
    tutucu = (String)EEPROM.read(i)+","+(String)EEPROM.read(i+1)+","+(String)EEPROM.read(i+2)+","+(String)EEPROM.read(i+3); 
    if(tutucu == kart){
      
     for(int k = i; k < (i+((kartAdedi-sayac)*4)); k++){
       EEPROM.write(k , EEPROM.read(k+4));
     }

     for(int t = ((kartAdedi-1)*4); t < (((kartAdedi-1)*4)+4); t++){
       EEPROM.write(t ,0);
     }
     kartSilP();
     delay(1000);
     return;
    }

    sayac++;
  } 
  kartSilF();
  delay(1000);
}




void kirmiziYan(){
  digitalWrite(A4, LOW);
  digitalWrite(A5, HIGH);
}
void sariYan(){
  digitalWrite(A4, HIGH);
  digitalWrite(A5, LOW);
}
void son(){
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
}


void bagir(){
  digitalWrite(A3,HIGH);
}
void sus(){
  digitalWrite(A3, LOW);
}


void kartKaydetP(){
  kartAdedi++;
  sariYan();
  bagir();
  delay(100);
  son();
  sus();
  delay(100);
  sariYan();
  bagir();
  delay(100);
  son();
  sus();
  delay(100);
  sariYan();
  bagir();
  delay(100);
  son();
  sus();
}
void kartKaydetF(){
  kirmiziYan();
  bagir();
  delay(50);
  son();
  sus();
  delay(50);
  kirmiziYan();
  bagir();
  delay(50);
  son();
  sus();
  delay(50);
  kirmiziYan();
  bagir();
  delay(50);
  son();
  sus();
}


void girisBasarili(){
  
  sariYan();
  bagir();
  delay(200);
  sus();
  delay(300);
  son();
  motorAc();
  delay(4000);
  int x = 0;
  while(x != 4000){
   delay(1);
   if(optikSensorOkuma()){
    x=0;
   }
   x++;
  }  
  motorKapat();
}

void girisBasarisiz(){
  kartKaydetF();
}


void kartSilP(){
  kartAdedi--;
  kartKaydetF();
  kartKaydetF();
}
void kartSilF(){
  kartKaydetF();
  kartKaydetF();
}

bool optikSensorOkuma(){
    if(digitalRead(A0) == LOW){
    bagir();
    delay(100);
    sus();
    delay(100);
    bagir();
    delay(100);
    sus();
    delay(100);
    bagir();
    delay(100);
    sus();
    delay(3000);
    return true;
  }
  return false;
}

void motorAc(){

  if(digitalRead(4) == HIGH){
    bagir();
    delay(200);
    sus();
    delay(200);
    bagir();
    delay(200);
    sus();
    delay(200);
    bagir();
    delay(200);
    sus();
    delay(1000);
    motorAc();
    return;
  }

  while(digitalRead(4) == HIGH){
    bagir();
    delay(200);
    sus();
    delay(200);
    bagir();
    delay(200);
    sus();
    delay(200);
    bagir();
    delay(200);
    sus();
    delay(1000);
  }
  
  Serial.println("MOTOR ACILDI");
  digitalWrite(m1, HIGH);
  digitalWrite(m2, LOW);
  digitalWrite(me,HIGH);
  
  delay(100);
  
  while(digitalRead(switch1) == LOW ){
    Serial.println("MOTORdevam"); 
    delay(100); 
  }
  
  digitalWrite(me,LOW);
  Serial.println("MOTORDURDU");
  }


void motorKapat(){
  
  digitalWrite(m1, LOW);
  digitalWrite(m2, HIGH);
  digitalWrite(me,HIGH);

  delay(100);

  while(digitalRead(switch2 == LOW)){
    
    delay(1);
    
    if(digitalRead(A0) == LOW){
      motorAc();
      delay(3000);
    }
  }

  digitalWrite(me,LOW);
}
