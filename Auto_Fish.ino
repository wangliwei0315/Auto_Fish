#define BLINKER_WIFI
#define BLINKER_MIOT_MULTI_OUTLET
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Blinker.h>
//连接ESP32 G34引脚，用于采集DI反馈
#define motor_di   34
//连接ESP32 G12引脚，用于驱动直流电机
int motor_do = 12;
//连接ESP32 G13引脚，用于驱动LED灯
int led_do = 13;
//连接ESP32 G14引脚，用于驱动氧气泵
int pump_do = 14;
// 定义DS18B20数据口连接ESP32的4号IO上
#define ONE_WIRE_BUS 4
//用于计算喂鱼开始时间count*100 ms
int count = 0;
//用于发送水温
int time10s = 0;
int time3s = 0;
// 初始连接在单总线上的单总线设备
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//WIFI配置信息
char auth[] = "d6bdd6e98371";
char ssid[] = "Redmi_42E5";
char pswd[] = "gulanjun";
//服务器中插座的状态
bool oState[5] = { false };
//手动控制喂鱼
BlinkerButton Button1("Auto_fish");
//单次喂鱼结束后，显示喂鱼时间（count的值）
BlinkerNumber Number1("num-abc");
//喂鱼过程中实时显示采集的DI值
BlinkerNumber Number2("num-di");
//实时显示采集的水温
BlinkerNumber Number3("num-temp");
//手动控制打开鱼缸灯
BlinkerButton Button2("Auto_led");
//手动控制打开氧气泵
BlinkerButton Button3("Auto_pump");
bool motor_state = false;
bool led_state = false;
bool pump_state = false;
bool di_state = false;
bool di_state_old = false;
hw_timer_t* timer1= NULL;  //定义存放定时器的指针
hw_timer_t* timer2= NULL;  //定义存放定时器的指针
int FLAG_timIT1 = 0;
int FLAG_timIT2 = 0;
// 按下自动喂鱼按键即会执行该函数
void button1_callback(const String & state)
{
    BLINKER_LOG("get button1 state: ", state);
    if(state == "on"){
      //digitalWrite(motor_do, HIGH);
      Button1.print("on");
      BlinkerMIOT.powerState("on", 1);
      BlinkerMIOT.print();
      oState[1] = true;
      motor_state = true;
      //Serial.printf("button1 ON");
    }
    else if(state == "off"){
      //digitalWrite(motor_do, LOW);
      Button1.print("off");
      BlinkerMIOT.powerState("off",1);
      BlinkerMIOT.print();
      oState[1] = false;
      motor_state = false;
      //Serial.printf("button1 OFF");
    }
}
void button2_callback(const String & state)
{
    BLINKER_LOG("get button2 state: ", state);
    if(state == "on"){
      digitalWrite(led_do, HIGH);
      Button2.print("on");
      led_state = true;
      BlinkerMIOT.powerState("on",2);
      BlinkerMIOT.print();
      oState[2] = true;
      //Serial.printf("button2 ON");
    }
    else if(state == "off"){
      digitalWrite(led_do, LOW);
      Button2.print("off");
      led_state = false;
      BlinkerMIOT.powerState("off",2);
      BlinkerMIOT.print();
      oState[2] = false;
      //Serial.printf("button2 OFF");
    }
}
void button3_callback(const String & state)
{
    BLINKER_LOG("get button3 state: ", state);
    if(state == "on"){
      digitalWrite(pump_do, HIGH);
      Button3.print("on");
      pump_state = true;
      BlinkerMIOT.powerState("on",3);
      BlinkerMIOT.print();
      oState[3] = true;
      //Serial.printf("button3 ON");
    }
    else if(state == "off"){
      digitalWrite(pump_do, LOW);
      Button3.print("off");
      pump_state = false;
      BlinkerMIOT.powerState("off",3);
      BlinkerMIOT.print();
      oState[3] = false;
      //Serial.printf("button3 OFF");
    }
}
//小爱同学控制接入函数
void miotPowerState(const String & state, uint8_t num)
{
    BLINKER_LOG("need set outlet: ", num, ", power state: ", state);
    if (state == BLINKER_CMD_ON) {
        BlinkerMIOT.powerState("on", num);
        BlinkerMIOT.print();
        oState[num] = true;
        if(num == 1){
          //Serial.printf("button1 ON");
          //digitalWrite(motor_do, HIGH);
          Button1.print("on");
          motor_state = true;
        }
        else if(num == 2){
          //Serial.printf("button2 ON");
          digitalWrite(led_do, HIGH);
          Button2.print("on");
          led_state = true;
        }
        else if(num == 3){
          //Serial.printf("button3 ON");
          digitalWrite(pump_do, HIGH);
          Button3.print("on");
          pump_state = true;
        }
    }
    else if (state == BLINKER_CMD_OFF) {
        //digitalWrite(LED_BUILTIN, LOW);
        BlinkerMIOT.powerState("off", num);
        BlinkerMIOT.print();
        oState[num] = false;
        if(num == 1){
          //Serial.printf("button1 OFF");
          //digitalWrite(motor_do, LOW);
          Button1.print("off");
          motor_state = false;
        }
        else if(num == 2){
          //Serial.printf("button2 OFF");
          digitalWrite(led_do, LOW);
          Button2.print("off");
          led_state = false;
        }
        else if(num == 3){
          //Serial.printf("button1 OFF");
          digitalWrite(pump_do, LOW);
          Button3.print("off");
          pump_state = false;
        }
        if (num == 0)
        {
            for (uint8_t o_num = 0; o_num < 5; o_num++)
            {
                oState[o_num] = false;
            }
        }
    }
}
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);
    Blinker.vibrate();
    uint32_t BlinkerTime = millis();
    Blinker.print("millis", BlinkerTime);
}
void IRAM_ATTR onTimer1()
{
  //Serial.printf("time1!!\n");
  FLAG_timIT1 = 1;
}
/*
void IRAM_ATTR onTimer2()
{
  //Serial.printf("time2!!\n");
  FLAG_timIT2 = 1;
}
*/
void setup()
{
  Serial.begin(115200);
  sensors.begin();
  BLINKER_DEBUG.stream(Serial);
  pinMode(motor_do, OUTPUT);
  pinMode(led_do, OUTPUT);
  pinMode(pump_do, OUTPUT);
  pinMode(motor_di,INPUT);
  digitalWrite(motor_do, LOW);
  digitalWrite(led_do, LOW);
  digitalWrite(pump_do, LOW);
  Blinker.begin(auth, ssid, pswd);
  BlinkerMIOT.attachPowerState(miotPowerState);
  Button1.attach(button1_callback);
  Button2.attach(button2_callback);
  Button3.attach(button3_callback);
  timer1 = timerBegin(0, 80, true);  //设置定时器0，80分频，定时器是否上下计数
  timerAttachInterrupt(timer1, onTimer1, true);  //定时器地址指针，中断函数名称，中断边沿触发类型
  timerAlarmWrite(timer1, 5000000, true);  //操作那个定时器，定时时长单位us，是否自动重装载
  timerAlarmEnable(timer1);  //打开那个定时器
  /*
  delay(3000);
  timer2 = timerBegin(1, 80, true);  //设置定时器1，80分频，定时器是否上下计数
  timerAttachInterrupt(timer2, onTimer2, true);  //定时器地址指针，中断函数名称，中断边沿触发类型
  timerAlarmWrite(timer2, 10000000, true);  //操作那个定时器，定时时长单位us，是否自动重装载
  timerAlarmEnable(timer2);  //打开那个定时器
  */
}
void loop()
{
  Blinker.run();
  //采集到的DI电压值（mv）
  int analog_value = 0;
  analog_value = analogRead(motor_di);
  if(analog_value > 4000){
    di_state = true;
  }
  else{
    di_state = false;
  }
  delay(100);
  if(FLAG_timIT1 == 1){
    FLAG_timIT1 = 0;
    sensors.requestTemperatures();  // 发送命令获取温度
    Number3.print(sensors.getTempCByIndex(0));
    delay(10);
    if(motor_state == true){
      Button1.print("on");
    }
    else{
      Button1.print("off");
    }
    if(led_state == true){
      Button2.print("on");
    }
    else{
      Button2.print("off");
    }
    delay(10);
    if(pump_state == true){
      Button3.print("on");
    }
    else{
      Button3.print("off");
    }
    delay(10);
  }
/*
  if(FLAG_timIT2 == 1){
    FLAG_timIT2 = 0;
    if(motor_state == true){
      Button1.print("on");
    }
    else{
      Button1.print("off");
    }
    if(led_state == true){
      Button2.print("on");
    }
    else{
      Button2.print("off");
    }
    delay(5);
    if(pump_state == true){
      Button3.print("on");
    }
    else{
      Button3.print("off");
    }
  }
*/
  if(motor_state == true){
    digitalWrite(motor_do, HIGH);
    count = count + 1;
  }
  else{
    digitalWrite(motor_do, LOW);
  }
  //如果：正在喂鱼，且开始5S后，且连续1次DI采集为高电平，则停止喂鱼
  if((motor_state == true)&&(count >20)&&(di_state == false)&&(di_state_old == true)){
    //digitalWrite(motor_do, LOW);
    Button1.print("off");
    motor_state = false;
    oState[1] = false;
    Number1.print(count*100);
    Number2.print(analog_value);
    count = 0;
  }
  else if((count >60)&&(motor_state == true)){
    //digitalWrite(motor_do, LOW);
    Button1.print("off");
    oState[1] = false;
    motor_state = false;
    Number1.print(count*100);
    Number2.print(analog_value);
    count = 0;
  }
  di_state_old = di_state;
}