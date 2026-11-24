#define fincarreraPin 14 // salida fin de carrera

// motor 28BYJ-4 módulo ULN2003 frecuencia de operación es 100Hz y la corriente de 92mA
//declaración de las variables para los pines del motor
#define motorPin1 A0  // Azul   - 28BYJ48 pin 1 motor
#define motorPin2 A1  // Rosa   - 28BYJ48 pin 2 motor
#define motorPin3 A2  // Amarillo - 28BYJ48 pin 3 motor
#define motorPin4 A3  // Naranja - 28BYJ48 pin 4 motor
                      // Rojo   - 28BYJ48 pin 5 (VCC) motor

int motorSpeed = 800;  //variable para fijar la velocidad del motor (el retraso entre cada secuencia)
int count = 0;          // cuenta de los pasos dados
int countsperrev = 512; // número de pasos por vuelta completa 
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

volatile bool fincarrera = false;
volatile unsigned long pasos = 0;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);      // sets the digital pin as output{
  pinMode(fincarreraPin, INPUT);
  //declaro los pines del motor como salida
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  Serial.begin(9600);  // salida a pc

  //fincarrera = digitalRead(fincarreraPin);
  digitalWrite(LED_BUILTIN, LOW);    // sets the LED off
  Serial.println(pasos);
  while(fincarrera == false)
  {
    clockwise();
    pasos++;
    fincarrera = !digitalRead(fincarreraPin);
  } 
  Serial.println(pasos);
  delay(3000);
  for(pasos=0; pasos<1792; ++pasos)
  {
    anticlockwise();
  }
  Serial.println(pasos);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // sets the LED on
  delay(1000);                  // waits for a second
  digitalWrite(LED_BUILTIN, LOW);    // sets the LED off
  delay(1000);                  // waits for a second
  /*if(count < countsperrev )
    clockwise(); //sentido agujas del reloj
  else if (count == countsperrev * 2)
    count = 0;
  else
    anticlockwise(); //sentido antihorario
  count++;*/

  /*if(count < countsperrev )
    anticlockwise(); //sentido agujas del reloj
  count++;*/
}

//creación funciones giro horario y antihorario para el motor paso a paso
void anticlockwise() 
{
  for(int i = 0; i < 8; i++)
  {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

void clockwise()
{
  for(int i = 7; i >= 0; i--)
  {
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

void setOutput(int out) //función secuencia giro
{
  digitalWrite(motorPin1, bitRead(lookup[out], 0));
  digitalWrite(motorPin2, bitRead(lookup[out], 1));
  digitalWrite(motorPin3, bitRead(lookup[out], 2));
  digitalWrite(motorPin4, bitRead(lookup[out], 3));
}

