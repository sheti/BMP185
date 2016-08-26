#include <Wire.h>
#include <BMP185.h>

BMP185 dps = BMP185();
byte oss = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  delay(1000);
  dps.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  long Pressure = dps.getPressure();
  long Temperature = dps.temperature;
  Serial.print("Pressure(mmHg):");
  Serial.println(Pressure/133.32);   //Выводим давление в мм.рт ст
  Serial.print("Temp:");
  Serial.println(Temperature*0.1);   // Температуру в градусах Цельсия
  Serial.println("--------------");
  delay(5000);
  oss += 1;
  if(oss > 3) oss = 0;
  dps.setOversamplig(oss);
}
