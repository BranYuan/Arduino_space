/*
 * DIY手柄
*/


#include <Servo.h>

#define BAUD_RATE     9600
#define SPEED_RANGE   100
#define HANDLE_MIN    0
#define HANDLE_MAX    100
#define SEND_RATE     10      //10HZ

/*
 * 定义引脚
*/
int handle_x_pin = A0;


/*
 * 通信协议定义
*/
char DATA_HEAD = '*';
char DATA_END = '#';
char DATA_SEPARATOR = ' ';

/*
 * 创建servo实例
*/

void setup() {
  // put your setup code here, to run once:
  pinMode(3,OUTPUT);
  Serial.begin(BAUD_RATE);
}

void loop() {
  int s = 0;
  int handle_x = map(analogRead(handle_x_pin),0,1024,0,255);   //脉冲量程范围500-2500ms 
    //analogWrite(3,handle_x);
    digitalWrite(3,HIGH);
    delay(1+handle_x);
    digitalWrite(3,LOW);
    delay(255-handle_x);
    int datas[]={handle_x};
    int len = sizeof(datas)/sizeof(datas[0]);
    outPutData(datas,len); 


}

void outPutData(int datas[],int len)
{
  Serial.print(DATA_HEAD);
  for(int i=0;i<len;i++)
  {
    Serial.print(datas[i]);
    Serial.print(DATA_SEPARATOR);
     
   }
   if(len-1!=1)
    Serial.print(datas[len-1]);
   Serial.println(DATA_END);
   delay(1000/SEND_RATE);
   
   
}
