#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

int minute=10;
int second=0;
boolean drawcolon = true;
boolean counting_flag = false;
boolean reading_flag = true;
const int BTN=2;

void setup() {
  Serial.begin(9600);
  Serial.println("HT16K33 test");
  matrix.begin(0x70);  // pass in the address
  setupInterrupt();  
  read_pwm();
  pinMode(13, OUTPUT);
  pinMode(BTN,INPUT);
}

void loop(){
  display_led();

  // if button is clicked;
  int btn_value = read_button();
  if ( btn_value > 0 ) {

      // if button is clicked for 2 seconds or above
      if ( btn_value > 1){
        counting_flag = false;
        second = 0;
        reading_flag = true;
      }

      // click once
      if ( btn_value == 1) {
        show_indicator(LOW);
        counting_flag = ! counting_flag;
        reading_flag = false;
      }
  }

  if ( reading_flag ) {
    show_indicator(HIGH);
    read_pwm();
  }
  delay(500);
}

//*************************
void tick(){
    second--;
    if ( second < 0 ){
      minute--;
      second = 59;
    } 
}

void display_led(){
   int distr = minute*100+second;
   Serial.print("Timer is ");
   Serial.println(distr);
   matrix.print(distr, DEC);
   if ( reading_flag ) {
      matrix.drawColon(true);
   }else{
      matrix.drawColon(drawcolon);
   }
   drawcolon = !drawcolon;
   matrix.writeDisplay();
}

void read_pwm(){
    int n = analogRead(A0);     //读取A0模拟口的数值（0-5V 对应 0-1204取值）
    minute = n/17/5*5;  
}

int read_button(){
    if( digitalRead(BTN)==HIGH ){
        delay(300);
        show_indicator(HIGH);
        if(digitalRead(BTN)==HIGH){
            int hold_time = 0;
            boolean ind_flag = false;
            while(digitalRead(BTN)==HIGH){
              delay(200);
              hold_time++;
              Serial.println(hold_time);
              if(hold_time > 10){
                hold_time = 10;
                show_indicator(ind_flag?HIGH:LOW);
                ind_flag = ! ind_flag;
              }
            }
            Serial.println(hold_time);
            hold_time = hold_time / 10;
            show_indicator(LOW);
            Serial.print("Button value");
            Serial.println(hold_time);
            return hold_time +1;
        }
    }

    return 0;
}

boolean show_indicator(boolean flag){
    digitalWrite(13, flag); 
}
//*************************

byte tcnt2;
unsigned long time = 1000;

// Credits for the interrupt setup routine:
// http://popdevelop.com/2010/04/mastering-timer-interrupts-on-the-arduino/
void setupInterrupt()
{
  /* First disable the timer overflow interrupt while we're configuring */  
  TIMSK2 &= ~(1<<TOIE2);   

  /* Configure timer2 in normal mode (pure counting, no PWM etc.) */  
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));   
  TCCR2B &= ~(1<<WGM22);   
  
  /* Select clock source: internal I/O clock */  
  ASSR &= ~(1<<AS2);
     
  /* Disable Compare Match A interrupt enable (only want overflow) */  
  TIMSK2 &= ~(1<<OCIE2A);   
  
  /* Now configure the prescaler to CPU clock divided by 128 */  
  TCCR2B |= (1<<CS22)  | (1<<CS20); // Set bits   
  TCCR2B &= ~(1<<CS21);             // Clear bit   
  
  /* We need to calculate a proper value to load the timer counter.  
   * The following loads the value 131 into the Timer 2 counter register  
   * The math behind this is:  
   * (CPU frequency) / (prescaler value) = 125000 Hz = 8us.  
   * (desired period) / 8us = 125.  
   * MAX(uint8) + 1 - 125 = 131;  
   */  
  /* Save value globally for later reload in ISR */  
  tcnt2 = 131;    
     
  /* Finally load end enable the timer */  
  TCNT2 = tcnt2;   
  TIMSK2 |= (1<<TOIE2);   
}

/*  
 * Install the Interrupt Service Routine (ISR) for Timer2 overflow.  
 * This is normally done by writing the address of the ISR in the  
 * interrupt vector table but conveniently done by using ISR()  */  
ISR(TIMER2_OVF_vect) {   
  /* Reload the timer */  
  TCNT2 = tcnt2;
  
  if(time > 0)
  {
    if ( ! counting_flag ) return;
    time--;
    if(time==0){
      time=1000;
      tick();
    }
  }
}  

