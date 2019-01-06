/*

*/


//定义基本通信参数
#define BAUD_RATE 9600      //波特率
#define SPEED_RANGE 100     //速度百分比
#define SEND_RATE 2        //循环频率

//引脚定义
#define x_pin  A0
#define y_pin  A1
#define z_pin  10
#define g_led  12

//全局变量定义
int MAX_SPEED = 2500;
int X_VALUE0 = 517-512;
int Y_VALUE0 = 484-512;
bool SPEED_LOCK = false;

/*
 * 通信协议定义(MODBUS协议)
 * 地址        命令号       寄存器地址高八位  低八位   数据字节数高八位  低八位       数据位1～n  CRC校验低八位  高八位
 * DEVICE_ID  CMD_MUN     ADDR_H          ADDR_L  DATA_NUM_H     DATA_NUM_L  DATA       CRC_L        CRC_H
*/
int DEVICE_ID = 0x01;     //设备ID
int CMD_NUN = 0x10;       //命令号：0x10----写入请求, 0x01----读取请求
int ADDR_H = 0x44;        //目标位置地址高八位
int ADDR_L = 0X20;        //目标位置地址低八位 左电机速度写入寄存器地址：0x4420，右电机速度写入寄存器地址：0x4421
int CRC_L = 0xFF;         //CRC校验码低八位
int CRC_H = 0xFF;         //CRC校验码高八位

//目标速度高低八位，单位0.1r/min
int SPEED_H = 0x4E;       
int SPEED_L = 0x20;

void setup() {
  // put your setup code here, to run once:
  pinMode(z_pin, INPUT_PULLUP);
  pinMode(g_led, OUTPUT);

  Serial.begin(BAUD_RATE);
  digitalWrite(g_led, LOW);
}

void loop() {
  int value_x = map(abs(analogRead(x_pin)-X_VALUE0), 0, 1023, SPEED_RANGE, -SPEED_RANGE);
  int value_y = map(abs(analogRead(y_pin)-Y_VALUE0), 0, 1023, -SPEED_RANGE, SPEED_RANGE);
  bool value_z = not digitalRead(z_pin);
  int goal_speed_L = 0;
  int goal_speed_R = 0;
  bool key_flag = true;
  value_y = value_y > 100 ? 100 : value_y;
  value_y = value_y < -100 ? -100 : value_y;
  value_x = value_x > 100 ? 100 : value_x;
  value_x = value_x < -100 ? -100 : value_x;

  //处理摇杆数据
  if (value_x >=0)
  {
    goal_speed_L = MAX_SPEED * value_y; 
    goal_speed_R = goal_speed_L * (1-value_x / 200);
  }
  else
  {
    goal_speed_R = MAX_SPEED * value_y; 
    goal_speed_L = goal_speed_R * (1-value_x / 200);
  }

  //计算并发送左电机速度
  if(!SPEED_LOCK)
  {
    speedCalculate(goal_speed_L);
  }
  int speed_L[] = {DEVICE_ID, 0x10, 0x44, 0x20, 0x00, 0x02, SPEED_H, SPEED_L};
  int len = sizeof(speed_L)/sizeof(speed_L[0]);
  crcCalculate(speed_L, len);
  outPutData(speed_L, len);
  //Serial.println();
  //计算并发送右电机速度
  if(!SPEED_LOCK)
  {
    speedCalculate(goal_speed_L);
  }
  int speed_R[] = {DEVICE_ID, 0x10, 0x44, 0x21, 0x00, 0x02, SPEED_H, SPEED_L};
  len = sizeof(speed_R)/sizeof(speed_R[0]);
  crcCalculate(speed_R, len);
  outPutData(speed_R, len);
  //Serial.println();
  
  //判断Z轴是否被按下，延时5ms排除抖动
  if(digitalRead(z_pin))
  {
    delay(2);
    if(digitalRead(z_pin))
    {
      key_flag = true;
    }
  }
  
  delay(1000 / SEND_RATE);

  if(key_flag && digitalRead(z_pin) == false)
  {
    delay(2);
    if(digitalRead(z_pin) == false)
    {
      SPEED_LOCK = !SPEED_LOCK;
      digitalWrite(g_led,SPEED_LOCK);
      key_flag = false;
    }
  }
  
  
  
}

void outPutData(int* data, unsigned int len)
{
  if(len == 0)
  {
    len = 1;
  }
  while(len--)
  {
    Serial.print(*data,HEX);
    data++;
  }
  Serial.print(CRC_L,HEX);
  Serial.print(CRC_H,HEX);
  /*
  Serial.println("*************");
  Serial.println(value_x, DEC);
  Serial.println(value_y, DEC);
  Serial.println(value_z, DEC);
  speedCalculate(2000);
  Serial.println(SPEED_H,HEX);
  Serial.println(SPEED_L,HEX);
  speedCalculate(-2000);
  Serial.println(SPEED_H,HEX);
  Serial.println(SPEED_L,HEX);
  Serial.println("*************");
  */
}

void crcCalculate(int* data,unsigned int len)
{
  unsigned char i;
  unsigned short crc = 0xFFFF;
  if(len == 0)
  {
    len = 1;
  }
  while(len--)
  {
    crc ^= *data;
    for(i = 0; i < 8; i++)
    {
      if(crc & 1)
      {
        crc >>= 1;
        crc ^= 0xA001;
      }
      else
      {
        crc >>= 1;
      }
    }
    data++;
  }
  CRC_L = crc & 0xff;
  CRC_H = crc >> 8 & 0xff;
  //return(crc);
} 

void speedCalculate(float goal_speed)
{
  int speed_int = (int)goal_speed * 10;
  speed_int = speed_int >= 0?speed_int:0xffff + speed_int + 1;
  SPEED_H = speed_int >> 8 & 0xff;
  SPEED_L = speed_int & 0xff;
}
