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
int self_lock_pin = 11;
int handle_x_pin = A0;
int handle_y_pin = A1;
int handle_z_pin = 2;
int speed_pin = A2;

int left_servo_pwm_pin = 3;
int right_servo_pwm_pin = 5;
int left_servo_pos_pin = A4;
int right_servo_pos_pin =A5;

/*
 * 通信协议定义
*/
char DATA_HEAD = '*';
char DATA_END = '#';
char DATA_SEPARATOR = ' ';

/*
 * 创建servo实例
*/
Servo left_servo;
Servo right_servo;

void setup() {
  // put your setup code here, to run once:
  pinMode(self_lock_pin,INPUT);
  pinMode(handle_z_pin,INPUT_PULLUP);
  Serial.begin(BAUD_RATE);
  left_servo.attach(3);
  right_servo.attach(5);
}

void loop() {
  int lock_status = digitalRead(self_lock_pin);
  int speed_value = analogRead(speed_pin);
  int speed_percent = map(speed_value,0,1023,0,SPEED_RANGE);

  int handle_x = map(analogRead(handle_x_pin),0,1024,500,2500);   //脉冲量程范围500-2500ms
  int handle_x_deg = map(handle_x,500,2500,0,300);    //脉冲转角度
  int handle_y = map(analogRead(handle_y_pin),0,1024,HANDLE_MAX,HANDLE_MIN);
  int handle_z = digitalRead(handle_z_pin);
  int left_servo_pos = map(analogRead(left_servo_pos_pin),67,943,0,300);  //角度实际位置，0.22v-4.78v
  left_servo.writeMicroseconds(handle_x);
  right_servo.writeMicroseconds(handle_x);
  delay(abs(17*(handle_x_deg-left_servo_pos)));
  //delay(5);
  left_servo_pos = map(analogRead(left_servo_pos_pin),67,943,0,300);
  //left_servo_pos = analogRead(left_servo_pos_pin);

  
  int datas[]={lock_status,speed_percent,handle_x_deg,left_servo_pos,handle_y,handle_z};
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
   //if(len-1!=0)
      Serial.print(datas[len-1]);
   Serial.println(DATA_END);
   delay(1000/SEND_RATE);
   
   
}
