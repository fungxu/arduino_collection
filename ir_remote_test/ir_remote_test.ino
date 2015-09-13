#include <IRremote.h>

// 从 https://github.com/shirriff/Arduino-IRremote
// 下载新的库

// 将 RobotIRremote库删除
 
 
int RECV_PIN = 11;//定义红外接收器的引脚为11
 
IRrecv irrecv(RECV_PIN);
 
decode_results results;
 
void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // 初始化红外接收器
}

// 显示红外线协定种类
void showIRProtocol(decode_results *results) 
{
  Serial.print("Protocol: ");
  
  // 判断红外线协定种类
  switch(results->decode_type) {
   case NEC:
     Serial.print("NEC");
     break;
   case SONY:
     Serial.print("SONY");
     break;
   case RC5:
     Serial.print("RC5");
     break;
   case RC6:
     Serial.print("RC6");
     break;

   case PANASONIC:
     Serial.print("PANASONIC");
     break;

   case JVC:
     Serial.print("JVC");
     break;

   case SAMSUNG:
     Serial.print("SAMSUNG");
     break;

   case AIWA_RC_T501:
     Serial.print("SAMSUNG");
     break;

   case LG:
     Serial.print("LG");
     break;

    case SANYO:
     Serial.print("SANYO");
     break;  
     
    case MITSUBISHI:
     Serial.print("MITSUBISHI");
     break;  

    case DISH:
     Serial.print("DISH");
     break;  

         case SHARP:
     Serial.print("SHARP");
     break;  


         case DENON:
     Serial.print("DENON");
     break;  

         case PRONTO:
     Serial.print("PRONTO");
     break;  
     
   default:
     Serial.print("Unknown encoding");  
  }  

  // 把红外线编码印到 Serial port
  Serial.print(", irCode: ");            
  Serial.print(results->value, HEX);    // 红外线编码
  Serial.print(",  bits: ");           
  Serial.println(results->bits);        // 红外线编码位元数    
}
 
void loop() {
  if (irrecv.decode(&results)) {
  // Serial.println(results.value, HEX);//以16进制换行输出接收代码
   // Serial.println();//为了便于观看输出结果增加一个空行


   showIRProtocol( &results);
   irrecv.resume(); // 接收下一个值
  }
}

