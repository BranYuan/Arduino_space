
class step_motor
{
  public: 
  unsigned int motor_pin;
  unsigned int motor_en;
  //unsigned int motor_break;
  unsigned int motor_cw;
  unsigned int motor_ccw;
  unsigned int motor_excitation;
  double motor_speed;
  int motor_steps;
  
  //类初始化函数
  void step_motor()
  {
    motor_pin = 3;
    motor_en = 2;
    motor_cw = 4;
    motor_ccw = 5;
    //motor_break = 6;
    motor_speed = 0;    //rps
    motor_steps = 1600; 
    pinMode(motor_pin,OUTPUT);
    pinMode(motor_en,OUTPUT);
    pinMode(motor_cw,OUTPUT);
    pinMode(motor_ccw,OUTPUT);
  }

  //改变引脚后的重新配置引脚状态函数
  reconfig()
  {
    pinMode(motor_pin,OUTPUT);
    pinMode(motor_en,OUTPUT);
    pinMode(motor_cw,OUTPUT);
    pinMode(motor_ccw,OUTPUT);
  }
  
  //启动电机，传入参数：速度(r/s)goal_speed,持续运动时间(ms)motor_runtime
  void start(float goal_speed)
  {
    int goal_hz = (int)abs(goal_speed * motor_steps);
    int cur_hz = (int)abs(motor_speed * motor_steps);
    //打开使能
    digitalWrite(motor_en,HIGH);
    //digitalWrite(motor_break,LOW);
    //判断正反向并执行
    if(goal_hz != cur_hz)
    {
      if(motor_speed * goal_speed < 0)
      {
        if(motor_speed >= 0)
        {
          digitalWrite(motor_cw,HIGH);
          digitalWrite(motor_ccw,LOW);
        }
        else
        {
          digitalWrite(motor_cw,LOW);
          digitalWrite(motor_ccw,HIGH);
        }
        while(cur_hz != 0)
        {
          cur_hz--;
          tone(motor_pin,cur_hz);
        }
      }
      if(goal_hz != cur_hz)
      {
        int i = (goal_hz-cur_hz)/abs(goal_hz-cur_hz);
        if(goal_speed > 0)
        {
          digitalWrite(motor_cw,HIGH);
          digitalWrite(motor_ccw,LOW);
        }
        else
        {
          digitalWrite(motor_cw,LOW);
          digitalWrite(motor_ccw,HIGH);
        }
        while(goal_hz != cur_hz)
        {
          cur_hz += i;
          tone(motor_pin,cur_hz);
        }
      }
    }
    motor_speed = goal_speed;
  }
  
  //停止
  void motor_stop()
  {
    noTone(motor_pin);
    motor_speed = 0;
  }
  
};
