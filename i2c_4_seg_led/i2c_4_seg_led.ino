/***************************************************
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED Matrix backpacks
  ----> http://www.adafruit.com/products/872
  ----> http://www.adafruit.com/products/871
  ----> http://www.adafruit.com/products/870

  These displays use I2C to communicate, 2 pins are required to
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#ifndef _BV
#define _BV(bit) (1<<(bit))
#endif


Adafruit_7segment matrix = Adafruit_7segment();

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

int counter = 6000;
boolean drawcolon = false;
boolean isStop = true;
boolean isPressing = false;

void setup() {
  Serial.begin(9600);
  Serial.println("HT16K33 test");
  pinMode(buttonPin, INPUT);
  pinMode(13, OUTPUT);
  matrix.begin(0x70);  // pass in the address
}

void loop() {

  if ( digitalRead(buttonPin) == HIGH) {
    delay(10);
    int _int_counter = 0;
    while (digitalRead(buttonPin) == HIGH) //判断按钮状态，如果仍然按下的话，等待松开。防止一直按住导致LED输出端连续反转
    {
      delay(1);
      _int_counter ++;

      if (_int_counter > 1000) {
        matrix.print(6000, DEC);
        matrix.writeDisplay();
        counter = 6000;
        isStop = false;
      }

    }
    buttonState = HIGH;
  } else {
    buttonState = LOW;
  }

  if (buttonState == HIGH) {
    isStop = !isStop;
  }

//  matrix.setBrightness();
  matrix.print(counter, DEC);
 // matrix.drawColon(drawcolon);
  drawcolon = !drawcolon;

  // write the changes we just made to the display
  matrix.writeDisplay();
  delay(100);
  if (!isStop) {
    counter--;
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
}
