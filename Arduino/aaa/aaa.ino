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

// declaracio de variables y constantes

volatile unsigned int nivel_inicial;
volatile unsigned int nivel_final;
unsigned int precipitacion;   // precipitacion en mm

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);  // configura el puerto serie para el modulo GSM
  Serial1.begin(9600);  // configura el puerto serie para el US-100
  
 
 
}

void loop() // bucle principal del programa
{
  Serial.print("Nivel inicial: ");
  nivel_inicial = medirNivel();
  Serial.println(nivel_inicial, DEC);
  delay(5000);
  Serial.print("Nivel final: ");
  nivel_final = medirNivel();
  Serial.println(nivel_final, DEC);
  precipitacion = nivel_inicial - nivel_final;  // nivel_nicial > nivel_final (distancia del sensor al fondo del tacho, ej: 80mm - 60mm)
  Serial.print("Precipitacion total: ");
  Serial.println(precipitacion, DEC);
  Serial.println(" ");
  delay(5000);
}

//declaracion e implementacion de funciones


void blinkLed(int t1)
{
  digitalWrite(LED_BUILTIN, HIGH);  // enciende el led
  delay(t1);                        // espera por t1 ms
  digitalWrite(LED_BUILTIN, LOW);   // apaga el led
  delay(t1);                        // espera por t1 ms
}

/*
 * efectua la medicion de nivel de agua
 * retorna el valor en caso de exito y "0" en caso de fallo
 */
unsigned int medirNivel()
{
  unsigned int MSByteDist = 0;    // variable para el US-100
  unsigned int LSByteDist = 0;    // variable para el US-100
  unsigned int mmDist = 0;        // distancia medida en mm
  
  Serial1.flush();                // limpia el buffer del puerto serie
  Serial1.write(0x55);            // orden de medición de distancia
 
  delay(500);
 
  if(Serial1.available() >= 2)    // comprueba la recepción de 2 bytes
  {
    MSByteDist = Serial1.read();  // lectura de ambos bytes
    LSByteDist  = Serial1.read();
    mmDist  = MSByteDist * 256 + LSByteDist;  // distancia

    if((mmDist > 1) && (mmDist < 10000)) // comprobación de la distancia dentro de rango 
    { 
      return mmDist;
    } 
    else
      return 0;
  }
    else return 0;
}
