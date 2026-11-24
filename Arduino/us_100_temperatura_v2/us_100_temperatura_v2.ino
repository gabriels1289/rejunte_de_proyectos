unsigned int MSByteDist = 0;
unsigned int LSByteDist = 0;
unsigned int mmDist = 0;
int temp = 0;
 
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}
 
void loop() {
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
      Serial.print("Distancia: "); 
      Serial.print(mmDist, DEC); 
      Serial.println(" mm");
    } 
  }
  Serial1.flush(); // limpia el buffer del puerto serie puerto
  Serial1.write(0x50); // orden de medición de distancia 
  delay(500); 
  if(Serial1.available() >= 1) // comprueba la recepción de 1 byte
  {
    temp = Serial1.read(); // lectura del byte
    if((temp > 1) && (temp < 130)) // comprobación de rango válido
    {
      temp -= 45; // corrige offset de 45º
      Serial.print("Temperatura: ");
      Serial.print(temp, DEC);
      Serial.println(" ºC.");
    }
  }
 
  delay(500);
}
