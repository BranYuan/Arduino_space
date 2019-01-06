#include <ArduinoSTL.h>

/*
 * MODBUS协议解析
 * 创建者：Bran_Yuan
 * 日期：2018-12-16
 * 版本号：V0.1
*/

/*
 * 通信协议定义(MODBUS协议)
 * 字节头高八位    低八位        地址        命令号       寄存器地址高八位  低八位   数据字节数高八位  低八位       数据位1～n  CRC校验低八位  高八位
 * HEAD_H          HEAD_L       DEVICE_ID  CMD_NUM   	  ADDR_H          ADDR_L  DATA_NUM_H        DATA_NUM_L     DATA       CRC_L       CRC_H
 * 例：速度：0xFE OxEF 0x01 0x10 0x44 0x20 0x00 0x02 0x0E Ox38 0xFF 0xFF 
*/

/*
 * vector主要函数,size(),empty(),front(),back();clear()
 * push_back(data),pop_back(),insert(index,data),erase(index),erase(first,last)
 * 
*/

using namespace std;
//参数定义
byte HEAD_H = 0xFE; 
byte HEAD_L = 0xEF;
byte DEVICE_ID = 0x01;
byte CMD_NUM = 0x10;
byte ADDR_H = 0x44;
byte ADDR_L = 0X23;
byte DATA_NUM_H = 0x00;
byte DATA_NUM_L = 0x04;
byte CRC_L = 0xFF;
byte CRC_H = 0xFF;
byte speed_LR[] = {HEAD_H, HEAD_L, DEVICE_ID, 0x10, 0x44, 0x23, 0x00, 0x04, 0x00, 0x00,0x00, 0x00, CRC_L, CRC_H};


void vector_init(std::vector<byte>& data_vector)
{
  if(!data_vector.empty())
  {
    data_vector.clear();
  }
  data_vector.push_back(HEAD_H);
  data_vector.push_back(HEAD_L);
  data_vector.push_back(DEVICE_ID);
}


bool receive_data(std::vector<byte>& data_vector)
{
  byte tmp_data = 0x00;
  int i = 0;
  int count = 0;
  bool received_flag = false;
  vector_init(data_vector);
  while(Serial.available())
  {
    tmp_data = Serial.read();
    if(data_vector.size() != i)
    {
      if(tmp_data != data_vector[i])
      {
        i = 0;
      }
      else
      {
        i++;
      }
    }
    else
    {
      data_vector.push_back(tmp_data);
      if(i == 6)
      {
        count = (tmp_data<<8);
      }
      else if(i == 7)
      {
        count += tmp_data + 2;
      }
      if(i > 7)
      {
        if(--count == 0)
        {
          received_flag = true;
          break;
        }
      }
      i++;  
    }
  }
  return received_flag;
}
