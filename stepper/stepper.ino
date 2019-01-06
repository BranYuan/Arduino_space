#include "StepMotor.h"
#include "ModbusSerial.h"

#define PUL_LEFT 3
#define EN_LEFT 2
#define CW_LEFT 4
#define CCW_LEFT 5
#define STEPS_LEFT 1600   //360/1.8*8




step_motor left_motor;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial.println("start");
}

void loop() {
  std::vector<byte> data_vector;
  double speed_L;
  double speed_R;
  bool receive_flag = false;
  int data_size = 0;
  
  
  if(Serial.available())
  {
    receive_flag = receive_data(data_vector);
    /*
    Serial.println("start to receive data");
    Serial.println(receive_flag);
    data_size = data_vector.size();
    for(int i=0; i<data_size; i++)
    {
      Serial.println(data_vector[i],HEX);
    }
    */
  }
  
  if(receive_flag)
  {
    Serial.println(data_vector[8],HEX);
    Serial.println(data_vector[9],HEX);
    Serial.println(data_vector[10],HEX);
    Serial.println(data_vector[11],HEX);
    speed_L = ((data_vector[8]<<8) + data_vector[9])/60.0/10;
    speed_R = ((data_vector[10]<<8) + data_vector[11])/60.0/10;
    Serial.println(speed_L,HEX);
  }
  if(abs(speed_L) >=0.1)
    left_motor.start(speed_L);
  else
    left_motor.motor_stop();
  delay(60);
}
