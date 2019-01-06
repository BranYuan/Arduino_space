/*
 * DIY手柄
*/


#include <Servo.h>

#define BAUD_RATE     115200
#define SPEED_RANGE   100
#define HANDLE_MIN    0
#define HANDLE_MAX    100
#define SEND_RATE     10      //10HZ

int code_A = 7;
int code_B = 8;

/*
 * 通信协议定义
*/
char DATA_HEAD = '*';
char DATA_END = '#';
char DATA_SEPARATOR = ' ';

void setup() {
  // put your setup code here, to run once:
  pinMode(code_A,INPUT);
  pinMode(code_B,INPUT);
  Serial.begin(BAUD_RATE);
}

void loop() {


  bool code_status_A = digitalRead(code_A);
  bool code_status_B = digitalRead(code_B);
  
  int datas[]={code_status_A,code_status_B};
  int len = sizeof(datas)/sizeof(datas[0]);
  if (code_status_A or code_status_B)
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
