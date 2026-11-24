void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);      // sets the digital pin as output
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);   // sets the LED on
  delay(1000);                  // waits for a second
  digitalWrite(LED_BUILTIN, LOW);    // sets the LED off
  delay(1000);                  // waits for a second
}
