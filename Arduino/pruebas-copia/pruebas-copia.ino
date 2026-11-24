/*
 * ---PINES UTILIZADOS---
 * --MODULO GSM--
 *  pin 9
 *  uart 0 (pin 0-1)
 * --US-100--
 *  uart 3 (pin 14-15)
 * --MOTOR--
 *  A0-A3
 * --SENSOR DE LLUVIA--
 *  A4
 *  pin 3
 * --FIN DE CARRERA--
 *  pin 2
 * --PULSADOR--
 *  pin 4
 * --UART AUXILIAR--
 *  uart2
 */

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
unsigned int Lvinicial = 0;
unsigned int Lvfinal = 0;
unsigned int precipitacion = 0; // precipitacion en mm
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

int motorSpeed = 800;  //variable para fijar la velocidad del motor (el retraso entre cada secuencia)
int count = 0;          // cuenta de los pasos dados
int countsperrev = 512; // número de pasos por vuelta completa 
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

#define fincarreraPin 2 // salida fin de carrera
volatile bool fincarrera = false;
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(sensorLluviaPin, INPUT);
  digitalWrite(PIN_GSM_PWR, LOW);   // pinMode(PIN_GSM_PWR, OUTPUT);**
  //declaro los pines del motor como salida
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  Serial.begin(19200);  // configura el puerto serie para el modulo GSM
  Serial1.begin(9600);  // configura el puerto serie para el US-100
  
  //Serial2.begin(9600);  // configura el puerto serie auxiliar
  
  // inicializo timer1 para que interrumpa cada 1 seg.
  noInterrupts();           // deshabilita todas las interrupciones
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 15625;            // registro de comparación 16MHz/1024/1Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS10);    // 1024 prescaler 
  TCCR1B |= (1 << CS12);    // 1024 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // habilito interrupción del timer por comparación
  attachInterrupt(digitalPinToInterrupt(fincarreraPin), finaldeCarrera, FALLING); // liga la interrupción de final de carrera por flanco descendente del pin fincarreraPin a la
  //función finaldeCarrera
  interrupts();             // habilito todas las interrupciones

  Serial2.println("Iniciando Programa");
  powerGSM(); // prendo el modulo GSM
  sendSMSencendido();  // envio el mensaje
  powerGSM(); // apago el modulo GSM

  medirNivel(); // medición dummy para limpiar los buffers y que no haya error de mediciones luego.
  
  abrirTapa();
  cerrarTapa();

  attachInterrupt(digitalPinToInterrupt(sensorLluviaPin), sensadoLluvia, CHANGE); // liga la interrupción de presencia de lluvia por cambio de flanco
}

ISR(TIMER1_COMPA_vect)  // timer compare interrupt service routine
{
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
    abrirTapa2();
    inicioTimer();
    while(llueve){
      blinkLed(125);
    };
    paroTimer();
    Lvfinal = medirNivel();
    precipitacion = Lvinicial - Lvfinal;  // nivel_nicial > nivel_final (distancia del sensor al fondo del tacho, ej: 80mm - 60mm)
    cerrarTapa();    
    //sprintf(msg,"Precipitacion: %03umm. - Tiempo: %02d:%02d:%02d - Lv. Inicial: %d / LV. Final: %d", precipitacion, tiempo.horas, tiempo.minutos, tiempo.segundos, Lvinicial, Lvfinal);
    //Serial2.println(msg);  // mensaje a enviar
    powerGSM(); // prendo el modulo GSM
    sendSMS();  // envio el mensaje
    powerGSM(); // apago el modulo GSM
  } 
  blinkLed(250);
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
 
  if(Serial1.available() == 2) // comprueba la recepción de 2 bytes
  {
    MSByteDist = Serial1.read(); // lectura de ambos bytes
    LSByteDist  = Serial1.read();
    mmDist  = MSByteDist * 256 + LSByteDist; // distancia

    if((mmDist > 1) && (mmDist < 10000)) // comprobación de la distancia dentro de rango 
    { 
      /*Serial2.print("Distancia: "); 
      Serial2.print(mmDist, DEC); 
      Serial2.println(" mm");*/
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
  OCR1A = 15625;            // registro de comparación 16MHz/1024/1Hz
  TIMSK1 |= (1 << OCIE1A);  // habilita la interrrupción por comparación del timer
}

void paroTimer()
{
  TIMSK1 |= (0 << OCIE1A);  // habilita la interrupción del timer por comparación
}

void sensadoLluvia()
{
  llueve=!digitalRead(sensorLluviaPin); // lectura digital, niego el valor porque el sensor entrega "1" si no llueve
}

void finaldeCarrera()
{
  fincarrera=true;
}

void powerGSM()
{
  digitalWrite(PIN_GSM_PWR, LOW);
  delay(2000);
  digitalWrite(PIN_GSM_PWR, HIGH);
  delay(2000);
  digitalWrite(PIN_GSM_PWR, LOW); 
  delay(2000); 
}

void sendSMS()
{ 
  delay(3000);
  Serial.print("AT+CMGF=1\r"); // comando AT para enviar SMS en modo texto
  delay(1000);
  Serial.println("AT + CMGS = \"+5492915356461\""); // número receptor en formato internacional
  delay(1000);
  sprintf(msg,"Nivel de precipitacion: %03dmm. - Duracion: %02d:%02d:%02d - Niveles: %d - %d", precipitacion, tiempo.horas, tiempo.minutos, tiempo.segundos, Lvinicial, Lvfinal);
  Serial.println(msg);  // mensaje a enviar
  delay(1000);
  Serial.println((char)26); // comando At de fin con ^Z, código ASCII 26 (0x1A)
  delay(1000);
  Serial.println();
  delay(5000);  // le da al módulo tiempo de enviar el SMS
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

void abrirTapa2()
{
  int pasos;
  for(pasos=0; pasos<1792; ++pasos)
  {
    if(pasos == 1386)
      Lvinicial = medirNivel();
    clockwise();
  }
}

void abrirTapa()
{
  while(fincarrera == false)
  {
    clockwise();
  }
}

void cerrarTapa()
{
  int pasos;
  for(pasos=0; pasos<1792; ++pasos)
  {
    anticlockwise();
  }
}

void blinkLed(int t)
{
  digitalWrite(LED_BUILTIN, HIGH);  // enciende el LED
  delay(t);                         // espera por "t" ms
  digitalWrite(LED_BUILTIN, LOW);   // apaga el LED
  delay(t);                         // espera por "t" ms
}

void sendSMSencendido()
{
  delay(3000);
  Serial.print("AT+CMGF=1\r"); // comando AT para enviar SMS en modo texto
  delay(1000);
  Serial.println("AT + CMGS = \"+5492915356461\""); // número receptor en formato internacional
  delay(1000);
  Serial.println("PLUVIOMETRO ENCENDIDO");  // mensaje a enviar
  delay(1000);
  Serial.println((char)26); // comando At de fin con ^Z, código ASCII 26 (0x1A)
  delay(1000);
  Serial.println();
  delay(10000);  // le da al módulo tiempo de enviar el SMS
}
