
//#include <SoftwareSerial.h>

//SoftwareSerial GSMSerial (2,3); //GSM
#define PIN_GSM_PWR 9 //PIN GSM POWER

#define GSMBUFFER_LENGTH 32
char GSMbuffer[GSMBUFFER_LENGTH];

void setup () 
{
  pinMode (PIN_GSM_PWR, OUTPUT);
  digitalWrite(PIN_GSM_PWR, LOW);
  Serial1.begin(4800);
  Serial.begin(9600); //monitor
}

void SendQuery (char * str) {
  while (Serial1.available()) Serial1.read(); //clear GSM buffer
  Serial1.println(str);
  //output monitor
  Serial.print ("GSM > ");
  Serial.println (str);
}

int ReadAnswer (unsigned long timeout) {
  int i=0;
  unsigned long starttime = millis();
  char c;
  while (millis() - starttime < timeout) {
    if (Serial1.available()) {
      c=Serial1.read();
      if (c == '\r') continue;
      if (c == '\n') {
        if (i ==0) continue;  //free line
        GSMbuffer[i]=0;
         //output monitor
        Serial.print ("GSM < ");
        Serial.println (GSMbuffer);
        return 1;
      }
      else if (i<GSMBUFFER_LENGTH) GSMbuffer[i++]=c;
    }
  }   
  return 0;
}




void GSMHandler() {
  int gsmon =0;
  int noerror;
  int regok=0;
  int retry;
  char GSMserialnumber[20];  
  
  //Enable GSM module
  Serial.println ("Go GSM handler...");
  SendQuery ("ATE0");  //echo off
  noerror=ReadAnswer (1000);
  if (!noerror) { //time out 
    Serial.println ("GSM OFF");
    Serial.println ("GSM POWER ON...");
    digitalWrite(PIN_GSM_PWR, HIGH);
    delay(1000);
    digitalWrite(PIN_GSM_PWR, LOW); 
    delay(1000);   
    SendQuery ("ATE0");  
    noerror=ReadAnswer (1000);
  }
  retry=10;
  while (retry>0) {
    if (noerror) {
      if (strcmp (GSMbuffer,"OK")==0) {
        Serial.println ("GSM ON");
        gsmon=1;
        break;
      }
    }  
    delay(100); 
    SendQuery ("ATE0");  
    noerror=ReadAnswer (1000);
    retry--;
  }
  if (!gsmon) {
    Serial.println ("GSM disconnect, error");
    return;
  }
  
  //Disable error mode
  SendQuery ("AT+CMEE=0");  
  noerror=ReadAnswer (1000);
  if (noerror) {
    if (strcmp (GSMbuffer,"OK")!=0) {  //query is not OK
      noerror=0;
    }
  }  
  if (!noerror) {
    Serial.println ("GSM error");
    return;
  }
  
  //Serial number
  SendQuery ("AT+GSN");  
  noerror=ReadAnswer (1000);
  if (noerror) {
    strcpy (GSMserialnumber, GSMbuffer);
  }
  noerror=ReadAnswer (1000);
  if (noerror) {
    if (strcmp (GSMbuffer,"OK")!=0) {  //queri is not OK
      noerror=0;
    }
  }  
  if (!noerror) {
    Serial.println ("GSM error");
    return;
  }
  Serial.print ("GSM serial: ");
  Serial.println (GSMserialnumber);
  
  //GSM wait net registration  
  retry=30;  //wait 30 sec
  while (retry>0) {
    SendQuery ("AT+CREG?");  
    noerror=ReadAnswer (1000);
    if (noerror) {
      if (strcmp (GSMbuffer,"+CREG: 0,1")==0) {  
        regok=1;
      }
    }
    noerror=ReadAnswer (1000);
    if (strcmp (GSMbuffer,"OK")==0) {  //query is OK
      if (regok) break;
    }
    delay(1000); 
    retry--;
  }
  if (!regok) {
    Serial.println ("GSM net not registration, error");
    return;
  }
  Serial.print ("GSM net registred");
  
  // TO DO ............ you commands
  
}

void loop() 
{
  char c;
  if (Serial.available()) {
    c=Serial.read();
    switch (c) {
      case 'h':
        GSMHandler();
        break;
    };
  }
} 
