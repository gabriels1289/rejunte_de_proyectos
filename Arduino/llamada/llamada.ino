#define PIN_GSM_PWR 9 //PIN GSM POWER

void setup()
{
  pinMode (PIN_GSM_PWR, OUTPUT);
  digitalWrite(PIN_GSM_PWR, LOW);
  Serial.begin(9600);
  Serial1.begin(19200);
  delay(1000);

  Serial.println ("GSM OFF");
  Serial.println ("GSM POWER ON...");
  digitalWrite(PIN_GSM_PWR, HIGH);
  delay(1000);
  digitalWrite(PIN_GSM_PWR, LOW); 
  delay(1000);   

  //Serial.println("set up comms");
  //Serial1.print("AT+CMGF=0");
  delay(7000);

  Serial.println("dialling...");
  Serial1.println("ATD + +5492914313616;");
  delay(12000);

  Serial1.println("ATH");
  Serial.println("Hung up.");
  
  //sendSMS();
}
void loop()
{

}

void callSomeone()
{
  Serial1.println("ATD + +542914313616;"); // dial UK phone
  Serial.println("ATD + +542914313616;");
  delay(100);
  Serial1.println();
  delay(10000);            // wait for 5 seconds...
  Serial1.println("ATH");   // hang up
  Serial.println("ATH - HANG UP");
}

void sendSMS()
{
  Serial1.print("AT+CMGF=1\r");
  // AT command to send SMS message
  delay(100);
  Serial1.println("AT + CMGS = \"+5492914313616\""); 
  // recipient's mobile number, in international format
  delay(100);
  Serial1.println("Hello, world. This is a text message from an Arduino Uno.");  
  // message to send
  delay(100);
  Serial1.println((char)26); // End AT command with a ^Z, ASCII code 26 (0x1A)
  delay(100);
  Serial1.println();
  delay(5000);       // give module time to send SMS
  //SIM900power();     // turn off module
}
