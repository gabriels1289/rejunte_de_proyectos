volatile byte llueve = false; // flag que indica la presencia de lluvia
#define sensorLluviaPin 18 // salida TTL del sensor de lluvia

void setup() {
 pinMode(sensorLluviaPin, INPUT);
 attachInterrupt(digitalPinToInterrupt(sensorLluviaPin), sensadoLluvia, CHANGE);
 pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if(llueve)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // sets the LED on
    delay(1000);                  // waits for a second
    digitalWrite(LED_BUILTIN, LOW);    // sets the LED off
    delay(1000);                  // waits for a second
  }
}

void sensadoLluvia()
{
  llueve=!digitalRead(sensorLluviaPin); // lectura digital, niego el valor porque el sensor entrega "1" si no llueve
}
