#include <DallasTemperature.h>
//连接ESP32 G34引脚，用于采集DI反馈
#define motor_di   34
//连接ESP32 G12引脚，用于驱动直流电机
int motor_do = 12;
// 定义DS18B20数据口连接ESP32的4号IO上
#define ONE_WIRE_BUS 4
//用于计算喂鱼开始时间count*100 ms
int count = 0;

bool oState = false;
bool motor_state = false;
bool di_state = false;
bool di_state_old = false;
//用于定时器分钟计数
int min_num = 0;

void IRAM_ATTR onTimer1()
{
  Serial.printf("THE time 60S is ON!!\n");
  min_num = min_num + 1;
  //计时10分钟时间到
  if(min_num >= 720){
    motor_state = true;
    digitalWrite(motor_do, HIGH);
    min_num == 0;
  }
}

void setup()
{
    Serial.begin(115200);
    sensors.begin();
    pinMode(motor_do, OUTPUT);
    pinMode(motor_di,INPUT);
    digitalWrite(motor_do, LOW);
    timer1 = timerBegin(0, 80, true);  //设置定时器0，80分频，定时器是否上下计数
    timerAttachInterrupt(timer1, onTimer1, true);  //定时器地址指针，中断函数名称，中断边沿触发类型
    timerAlarmWrite(timer1, 60000000, true);  //操作那个定时器，定时时长单位us，是否自动重装载
    timerAlarmEnable(timer1);  //打开那个定时器
}

void loop()
{
  //采集到的DI电压值（mv）
  int analog_value = 0;
  analog_value = analogRead(motor_di);
  delay(100);
  if(analog_value > 4000){
    di_state = true;
  }
  else{
    di_state = false;
  }
  if(motor_state == true){
    count = count + 1;
  }
  //喂鱼程序因超时自动停止
  if((count >148)&&(motor_state == true)){
    digitalWrite(motor_do, LOW);
    motor_state = false;
    count = 0;
    Serial.printf("It is time to stop!!!");
  }
  //如果：正在喂鱼，且开始5S后，且连续2次DI采集为高电平，则停止喂鱼
  if((motor_state == true)&&(count >50)&&(di_state == false)&&(di_state_old == true)){
    digitalWrite(motor_do, LOW);
    motor_state = false;
    Serial.printf("the count is %d\n",count);
    count = 0;
    Serial.printf("normal stop");
  }
  di_state_old = di_state;
}