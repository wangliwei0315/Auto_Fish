#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET
#include <Blinker.h>
//连接ESP32 G34引脚，用于采集DI反馈
#define motor_di   34
//连接ESP32 G12引脚，用于驱动直流电机
int motor_do = 12;
//用于计算喂鱼开始时间count*100 ms
int count = 0;
//WIFI配置信息
char auth[] = "7c8e9ee2a1c1";
char ssid[] = "Redmi_42E5";
char pswd[] = "gulanjun";
//手动控制喂鱼
BlinkerButton Button1("Auto_fish");
//单次喂鱼结束后，显示喂鱼时间（count的值）
BlinkerNumber Number1("num-abc");
//喂鱼过程中实时显示采集的DI值
BlinkerNumber Number2("num-di");

bool oState = false;
bool motor_state = false;
bool di_state = false;
bool di_state_old = false;
// 按下按键即会执行该函数
void button1_callback(const String & state)
{
    BLINKER_LOG("get button1 state: ", state);
    if(state == "on"){
      digitalWrite(motor_do, HIGH);
      Button1.print("on");
      //delay(3000);
      motor_state = true;
    }
    else if(state == "off"){
      digitalWrite(motor_do, LOW);
      Button1.print("off");
      motor_state = false;
    }
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
//小爱同学控制接入函数
void miotPowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);
    if (state == BLINKER_CMD_ON) {
        //digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(motor_do, HIGH);
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        Button1.print("on");
        motor_state = true;
        oState = true;
    }
    else if (state == BLINKER_CMD_OFF) {
        digitalWrite(motor_do, LOW);
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        Button1.print("off");
        motor_state = false;
        oState = false;
    }
}
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);
    Blinker.vibrate();
    uint32_t BlinkerTime = millis();
    Blinker.print("millis", BlinkerTime);
}

void setup()
{
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    //pinMode(LED_BUILTIN, OUTPUT);
    pinMode(motor_do, OUTPUT);
    pinMode(motor_di,INPUT);
    digitalWrite(motor_do, LOW);
    //digitalWrite(LED_BUILTIN, LOW);
    Blinker.begin(auth, ssid, pswd);
    //Blinker.attachData(dataRead);
    BlinkerMIOT.attachPowerState(miotPowerState);
    //BlinkerMIOT.attachQuery(miotQuery);
    Button1.attach(button1_callback);
  
}

void loop()
{
  Blinker.run();
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

  if((count >148)&&(di_state == true)&&(di_state_old == true)){
    digitalWrite(motor_do, LOW);
    Button1.print("off");
    motor_state = false;
    Number1.print(count);
    count = 0;
    Serial.printf("It is time to stop!!!");
  }
  //Serial.printf("Current Reading on Pin(%d)=%d\n",motor_di,analog_value);
  //如果：正在喂鱼，且开始5S后，且连续2次DI采集为高电平，则停止喂鱼
  if((motor_state == true)&&(count >50)&&(di_state == false)&&(di_state_old == true)){
    //Serial.printf("Current Reading on Pin(%d)=%d\n",motor_di,analog_value);
    digitalWrite(motor_do, LOW);
    Button1.print("off");
    Button1.print("off");
    motor_state = false;
    Serial.printf("the count is %d\n",count);
    Number1.print(count);
    count = 0;
    Serial.printf("normal stop");
  }
  //每200ms（count为奇数）赋值一次
  if((count&1)==1){
    di_state_old = di_state;
    Number2.print(analog_value);
  }
}