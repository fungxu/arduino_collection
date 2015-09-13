//红外感应
//信号接 2 端口
int ledpin = 2;
 
void setup()
{
  pinMode(ledpin, INPUT);
  Serial.begin(9600);  // 打开串口，设置波特率为9600 bps
}
 
void loop()
{
  int in = digitalRead(ledpin); 
  Serial.println(in); //有人的时候输出高电平1 无人0
  delay(2000);    
}
