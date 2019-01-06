/*
 * MODBUS协议无线控制手柄,主要用于行走控制
 * 创建者：Bran_Yuan
 * 日期：2018-12-16
 * 版本号：V0.1
*/


//定义基本通信参数
#define BAUD_RATE 9600      //波特率
#define SPEED_RANGE 10000     //速度千分比
#define MAX_TURN_DIFFERENCE 2    //最大转弯能力，左右轮最大差速比
#define SEND_RATE 10        //循环频率

//引脚定义
#define x_pin  A0
#define y_pin  A1
#define z_pin  10
#define g_led  12

//全局变量定义
int MAX_SPEED = 4200;          //MAX_SPEED = real_speed * 10
int X_VALUE0 = 517-512;
int Y_VALUE0 = 484-512;
bool SPEED_LOCK = false;

/*
 * 通信协议定义(MODBUS协议)
 * 字节头高八位    低八位        地址        命令号       寄存器地址高八位  低八位   数据字节数高八位  低八位       数据位1～n  CRC校验低八位  高八位
 * HEAD_H          HEAD_L       DEVICE_ID  CMD_NUM     ADDR_H          ADDR_L  DATA_NUM_H     DATA_NUM_L  DATA       CRC_L        CRC_H
 * 例：速度：0xFE OxEF 0x01 0x10 0x44 0x20 0x00 0x02 0x0E Ox38 0xFF 0xFF 
*/
byte HEAD_H = 0xFE;
byte HEAD_L = 0xEF;
byte DEVICE_ID = 0x01;     //设备ID
byte CMD_NUN = 0x10;       //命令号：0x10----写入请求, 0x01----读取请求
byte ADDR_H = 0x44;        //目标位置地址高八位
byte ADDR_L = 0X20;        //目标位置地址低八位 左电机速度写入寄存器地址：0x4420，右电机速度写入寄存器地址：0x4421，左右电机速度写入0x4423
byte CRC_L = 0xFF;         //CRC校验码低八位
byte CRC_H = 0xFF;         //CRC校验码高八位

//目标速度高低八位，单位0.1r/min
byte SPEED_HL = 0x4E;       
byte SPEED_LL = 0x20;
byte SPEED_HR = 0x4E;
byte SPEED_LR = 0x20;

void setup() {
  // put your setup code here, to run once:
  pinMode(z_pin, INPUT_PULLUP);
  pinMode(g_led, OUTPUT);
  while(digitalRead(z_pin))
  {
    digitalWrite(g_led,HIGH);
    delay(500);
    digitalWrite(g_led,LOW);
    delay(500);
  }
  Serial.begin(BAUD_RATE);
  digitalWrite(g_led, LOW);
}

void loop() {
  int pre_time = millis();
  int time_change;
  int value_x = map(abs(analogRead(x_pin)-X_VALUE0), 0, 1023, SPEED_RANGE, -SPEED_RANGE);
  int value_y = map(abs(analogRead(y_pin)-Y_VALUE0), 0, 1023, -SPEED_RANGE, SPEED_RANGE);
  bool value_z;
  int goal_speed_L = 0;
  int goal_speed_R = 0;
  int last_debounce_time;
  int debounce_delay = 1000;
  value_y = value_y > SPEED_RANGE ? SPEED_RANGE : value_y;
  value_y = value_y < -SPEED_RANGE ? -SPEED_RANGE : value_y;
  value_x = value_x > SPEED_RANGE ? SPEED_RANGE : value_x;
  value_x = value_x < -SPEED_RANGE ? -SPEED_RANGE : value_x;
  //处理摇杆数据
  if (value_x >=0)
  {
    goal_speed_L = MAX_SPEED * (1.0 * value_y / SPEED_RANGE); 
    goal_speed_R = goal_speed_L * (1-1.0* value_x / SPEED_RANGE / MAX_TURN_DIFFERENCE );
  }
  else
  {
    goal_speed_R = MAX_SPEED * (1.0 * value_y / SPEED_RANGE); 
    goal_speed_L = goal_speed_R * (1-1.0 * value_x /  SPEED_RANGE / MAX_TURN_DIFFERENCE );
  }
  //计算并发送左右电机速度
  if(!SPEED_LOCK)
  {
    speedCalculate(goal_speed_L,goal_speed_R);
  }
  byte speed_LR[] = {HEAD_H, HEAD_L, DEVICE_ID, 0x10, 0x44, 0x23, 0x00, 0x04, SPEED_HL, SPEED_LL,SPEED_HR, SPEED_LR};
  int len = sizeof(speed_LR)/sizeof(speed_LR[0]);
  crcCalculate(speed_LR, len);
  outPutData(speed_LR, len);
  
  //判断Z轴是否被按下，延时50ms排除抖动
  value_z = digitalRead(z_pin);
  if(value_z != digitalRead(z_pin))
  {
    last_debounce_time = millis();
  }
  if(millis() - last_debounce_time > debounce_delay)
  {
    if(!value_z)
    {
      SPEED_LOCK = !SPEED_LOCK;
      digitalWrite(g_led,SPEED_LOCK);
    }
  }
  time_change = 1000 / SEND_RATE + pre_time - millis();
  if(time_change > 0)
  {
    delay(time_change);
  }
}

void outPutData(byte* data, unsigned int len)
{
  if(len == 0)
  {
    len = 1;
  }
  while(len--)
  {
    Serial.write(*data);
    data++;
  }
  Serial.write(CRC_L);
  Serial.write(CRC_H);
}

void crcCalculate(byte* data,unsigned int len)
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

void speedCalculate(int goal_speed_L, int goal_speed_R)
{
  goal_speed_L = goal_speed_L >= 0? goal_speed_L : 0xffff + goal_speed_L + 1;
  SPEED_HL = goal_speed_L >> 8 & 0xff;
  SPEED_LL = goal_speed_L & 0xff;

  goal_speed_R = goal_speed_R >= 0? goal_speed_R : 0xffff + goal_speed_R + 1;
  SPEED_HR = goal_speed_R >> 8 & 0xff;
  SPEED_LR = goal_speed_R & 0xff;
}
