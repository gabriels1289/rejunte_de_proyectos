//declaracio de variables y constantes
//estructura para el tiempo
typedef struct
{
  byte dias;
  byte horas;
  byte minutos;
  byte segundos; 
} Time;

char msg[120];  //arreglo para el mensaje de texto
volatile Time tiempo;
volatile byte llueve = false; // flag que indica la presencia de lluvia
unsigned int nivel_inicial;
unsigned int nivel_final;
unsigned int precipitacion; // precipitacion en mm
#define sensorLluviaPin 3 // salida TTL del sensor de lluvia
//#define pinLluviaAnalogico A4 salida analogica del sensor de lluvia

#define PIN_GSM_PWR 9 //PIN GSM POWER

// motor 28BYJ-4 módulo ULN2003 frecuencia de operación es 100Hz y la corriente de 92mA
//declaración de las variables para los pines del motor
#define motorPin1 A0  // Azul   - 28BYJ48 pin 1 motor
#define motorPin2 A1  // Rosa   - 28BYJ48 pin 2 motor
#define motorPin3 A2  // Amarillo - 28BYJ48 pin 3 motor
#define motorPin4 A3  // Naranja - 28BYJ48 pin 4 motor
                      // Rojo   - 28BYJ48 pin 5 (VCC) motor

int motorSpeed = 1200;  //variable para fijar la velocidad del motor (el retraso entre cada secuencia)
int count = 0;          // cuenta de los pasos dados
int countsperrev = 512; // número de pasos por vuelta completa 
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};


void setup()
{
  pinMode(sensorLluviaPin, INPUT);
  pinMode(PIN_GSM_PWR, OUTPUT);
  //declaro los pines del motor como salida
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  Serial1.begin(19200); // configura el puerto serie para el modulo GSM
  Serial1.begin(9600);  // configura el puerto serie para el US-100
  
  // inicializo timer1 para que interrumpa cada 1 seg.
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 15625;            // compare match register 16MHz/1024/1Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS10);    // 1024 prescaler 
  TCCR1B |= (1 << CS12);    // 1024 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
 
  attachInterrupt(digitalPinToInterrupt(sensorLluviaPin), sensadoLluvia, CHANGE);
  interrupts();             // enable all interrupts
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  //digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
  ++tiempo.segundos;
  if(tiempo.segundos >= 60)
  {
    tiempo.segundos = 0;
    ++tiempo.minutos;
  }
  if(tiempo.minutos >= 60)
  {
    tiempo.minutos = 0;
    ++tiempo.horas;
  }
  if(tiempo.horas >= 24)
  {
    tiempo.horas = 0;
    ++tiempo.dias;
  }
}

void loop()
{
  if(llueve)
  {
    nivel_inicial = medirNivel();
    abrirTapa();
    inicioTimer();
    while(llueve){};
    paroTimer();
    cerrarTapa();
    nivel_final = medirNivel();
    precipitacion = nivel_inicial - nivel_final;  // nivel_nicial > nivel_final (distancia del sensor al fondo del tacho, ej: 80mm - 60mm)
    powerGSM(); // prendo el modulo GSM
    sendSMS();  // envio el mensaje
    powerGSM(); // apago el modulo GSM
  } 
    //sprintf(msg, "Tiempo transcurrido: %02d:%02d:%02d", tiempo.horas, tiempo.minutos, tiempo.segundos);
    //Serial.println(msg);
}

//declaracion e implementacion de funciones
void clearTiempo()
{
  tiempo.dias = 0;
  tiempo.horas = 0;
  tiempo.minutos = 0;
  tiempo.segundos = 0;
}

/*
 * efectua la medicion de nivel de agua
 * retorna el valor en caso de exito y "0" en caso de fallo
 */
unsigned int medirNivel()
{
  unsigned int MSByteDist = 0;  //variable para el US-100
  unsigned int LSByteDist = 0;  //variable para el US-100
  unsigned int mmDist = 0;  //distancia medida en mm
  
  Serial1.flush(); // limpia el buffer del puerto serie
  Serial1.write(0x55); // orden de medición de distancia
 
  delay(500);
 
  if(Serial1.available() >= 2) // comprueba la recepción de 2 bytes
  {
    MSByteDist = Serial1.read(); // lectura de ambos bytes
    LSByteDist  = Serial1.read();
    mmDist  = MSByteDist * 256 + LSByteDist; // distancia

    if((mmDist > 1) && (mmDist < 10000)) // comprobación de la distancia dentro de rango 
    { 
     /* Serial.print("Distancia: "); 
      Serial.print(mmDist, DEC); 
      Serial.println(" mm");*/
      return mmDist;
    } 
    else
      return 0;
  }
    else return 0;
}

void inicioTimer()
{
  clearTiempo();
  OCR1A = 15625;            // compare match register 16MHz/1024/1Hz
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
}

void paroTimer()
{
  TIMSK1 |= (0 << OCIE1A);  // enable timer compare interrupt
}

void sensadoLluvia()
{
  llueve=!digitalRead(sensorLluviaPin); // lectura digital, niego el valor porque el sensor entrega "1" si no llueve
}

void powerGSM()
{
  digitalWrite(PIN_GSM_PWR, LOW);
  delay(1000);
  digitalWrite(PIN_GSM_PWR, HIGH);
  delay(1000);
  digitalWrite(PIN_GSM_PWR, LOW); 
  delay(1000); 
}

void sendSMS()
{
  Serial.print("AT+CMGF=1\r"); //AT command to send SMS message Because we want to send the SMS in text mode
  delay(100);
  Serial.println("AT + CMGS = \"+5492914313616\""); // recipient's mobile number, in international format
  delay(100);
  sprintf(msg,"Nivel de precipitacion: %dmm. - Duracion: %02d%02d%02d", precipitacion, tiempo.horas, tiempo.minutos, tiempo.segundos);
  Serial.println(msg);  // message to send
  delay(100);
  Serial.println((char)26); // End AT command with a ^Z, ASCII code 26 (0x1A)
  delay(100);
  Serial.println();
  delay(5000);  // give module time to send SMS
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

