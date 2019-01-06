#define PUL_LEFT 4
#define EN_LEFT 2
#define CW_LEFT 3
#define STEPS_LEFT 1600   //360/1.8*4

class step_motor
{
  
  public: 
  unsigned int motor_pin;
  unsigned int motor_en;
  unsigned int motor_cw;
  unsigned int motor_excitation;
  double motor_speed;
  int motor_steps;
  step_motor()
  {
    motor_pin = 2;
    motor_en = 3;
    motor_cw = 4;
    motor_speed = 0;    //rps
    motor_steps = 800; 
    pinMode(motor_pin,OUTPUT);
    pinMode(motor_en,OUTPUT);
    pinMode(motor_cw,OUTPUT);
  }
  void start(int goal_speed,unsigned int motor_runtime)
  {
    int goal_hz = (int)abs(goal_speed * motor_steps);
    digitalWrite(motor_en,HIGH);
    if(goal_speed > 0)
      digitalWrite(motor_cw,HIGH);
    else
      digitalWrite(motor_cw,LOW);
    tone(motor_pin,goal_hz,motor_runtime);
    motor_speed = goal_speed;
  }
  void start(int goal_speed)
  {
    int goal_hz = (int)abs(goal_speed * motor_steps);
    digitalWrite(motor_en,HIGH);
    if(goal_speed > 0)
      digitalWrite(motor_cw,HIGH);
    else
      digitalWrite(motor_cw,LOW);
    tone(motor_pin,goal_hz);
    motor_speed = goal_speed;
  }
  
};

step_motor left_motor;
void setup() {
  // put your setup code here, to run once:
  left_motor.motor_pin = PUL_LEFT;
  left_motor.motor_en = EN_LEFT;
  left_motor.motor_cw = CW_LEFT;
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  left_motor.start(0.5);
  delay(2000);
  left_motor.start(1.5);
   delay(2000);
  left_motor.start(2.5);
  delay(2000);
  left_motor.start(1.5);
  delay(2000);
  left_motor.start(0.5);
  delay(2000);
  left_motor.start(-0.5);
  delay(2000);
  left_motor.start(-1);
  delay(2000);
  left_motor.start(-3);

}
