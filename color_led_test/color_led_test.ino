const uint8_t red = 3;
const uint8_t green = 5;
const uint8_t blue = 6;

void setup()
{
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(green,OUTPUT);
}


int brightness = 0;
int fadeAmount = 5;
void loop(){
  //digitalWrite(green, 1);
  //digitalWrite(red, 0);
  //digitalWrite(blue,0);

  analogWrite(green,brightness);
  analogWrite(red,brightness);
  analogWrite(blue,brightness);

  brightness += fadeAmount;

  if(brightness == 0 || brightness == 255){
    fadeAmount = -fadeAmount;
  }
  delay(30);
}

