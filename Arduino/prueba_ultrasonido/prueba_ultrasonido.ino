// Pins de US-100
const int trigger_us100 = 19;
const int echo_us100 = 18;
 
void setup()
{
  Serial.begin(9600);
 
  // Configuración de los pines
  pinMode(trigger_us100, OUTPUT);
  pinMode(echo_us100, INPUT);
}
 
void loop()
{
  float distancia;
 
  // Medida en el US-100
  distancia = medida(trigger_us100, echo_us100);
  // Devuelve el resultado vía UART
  Serial.print("Dist US-100: ");
  Serial.println(distancia);
 
  delay(500);
}
 
float medida(int trigger, int echo) // Función de ejecución de medida
{
  // Trigger inicial
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
 
  // Comienzo de la medida
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
 
  // Adquisición y conversión a metros
  float distancia = pulseIn(echo, HIGH);
  distancia = distancia * 0.01657;
  return distancia;
}
