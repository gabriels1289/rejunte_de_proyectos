#define PIN_GSM_PWR 9 //PIN GSM POWER

void setup()
{
  pinMode (PIN_GSM_PWR, OUTPUT);
  digitalWrite(PIN_GSM_PWR, LOW);
  Serial.begin(19200);
  delay(1000);

  digitalWrite(PIN_GSM_PWR, HIGH);
  delay(1000);
  digitalWrite(PIN_GSM_PWR, LOW); 
  delay(1000);   

  //Serial.print("AT+CMGF=0");
  delay(7000);

  //Serial.println("ATD + +5492914313616;");
  Serial.println("ATD + +5492915356461;");
  delay(12000);

  Serial.println("ATH");
  
  sendSMS();
}
void loop()
{

}
/*
void callSomeone()
{
  Serial.println("ATD + +542914313616;"); // dial UK phone
  delay(100);
  Serial.println();
  delay(10000);            // wait for 5 seconds...
  Serial.println("ATH");   // hang up
}
*/
void sendSMS()
{
  Serial.print("AT+CMGF=1\r");
  // AT command to send SMS message
  delay(100);
  //Serial.println("AT + CMGS = \"+5492914313616\"");
  Serial.println("AT + CMGS = \"+5492915356461\""); 
  // recipient's mobile number, in international format
  delay(100);
  Serial.println("Hello, world. This is a text message from an Arduino Uno.");  
  // message to send
  delay(100);
  Serial.println((char)26); // End AT command with a ^Z, ASCII code 26 (0x1A)
  delay(100);
  Serial.println();
  delay(5000);       // give module time to send SMS
  //SIM900power();     // turn off module
}
