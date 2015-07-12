#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();
 
void setup()
{
  Serial.begin(9600); 
  Serial.println("HT16K33 test");
  matrix.begin(0x70);  // pass in the address  z
}
 
void loop()
{
  int n = analogRead(A0);     //读取A0模拟口的数值（0-5V 对应 0-1204取值）
  Serial.println(n);                   //串口输出温度数据
  delay(2000);  

   matrix.print(n, DEC);
   matrix.writeDisplay(); 
}

