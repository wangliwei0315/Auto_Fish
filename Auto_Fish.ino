#define BLINKER_WIFI
//#define BLINKER_MIOT_OUTLET
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
int time3s = 0;
// 初始连接在单总线上的单总线设备
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//WIFI配置信息
char auth[] = "d6bdd6e98371";
char ssid[] = "Redmi_42E5";
char pswd[] = "gulanjun";

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
//bool oState = false;
bool motor_state = false;
bool di_state = false;
bool di_state_old = false;
// 按下自动喂鱼按键即会执行该函数
void button1_callback(const String & state)
{
    BLINKER_LOG("get button1 state: ", state);
    if(state == "on"){
      digitalWrite(motor_do, HIGH);
      Button1.print("on");
      BlinkerMIOT.powerState("on", 1);
      BlinkerMIOT.print();
      oState[1] = true;
      motor_state = true;
      Serial.printf("button1 ON");
    }
    else if(state == "off"){
      digitalWrite(motor_do, LOW);
      Button1.print("off");
      BlinkerMIOT.powerState("off",1);
      BlinkerMIOT.print();
      oState[1] = false;
      motor_state = false;
      Serial.printf("button1 OFF");
    }
}
void button2_callback(const String & state)
{
    BLINKER_LOG("get button2 state: ", state);
    if(state == "on"){
      digitalWrite(led_do, HIGH);
      Button2.print("on");
      BlinkerMIOT.powerState("on",2);
      BlinkerMIOT.print();
      oState[2] = true;
      Serial.printf("button2 ON");
    }
    else if(state == "off"){
      digitalWrite(led_do, LOW);
      Button2.print("off");
      BlinkerMIOT.powerState("off",2);
      BlinkerMIOT.print();
      oState[2] = false;
      Serial.printf("button2 OFF");
    }
}
void button3_callback(const String & state)
{
    BLINKER_LOG("get button3 state: ", state);
    if(state == "on"){
      digitalWrite(pump_do, HIGH);
      Button3.print("on");
      BlinkerMIOT.powerState("on",3);
      BlinkerMIOT.print();
      oState[3] = true;
      Serial.printf("button3 ON");
    }
    else if(state == "off"){
      digitalWrite(pump_do, LOW);
      Button3.print("off");
      BlinkerMIOT.powerState("off",3);
      BlinkerMIOT.print();
      oState[3] = false;
      Serial.printf("button3 OFF");
    }
}
//小爱同学控制接入函数
void miotPowerState(const String & state, uint8_t num)
{
    BLINKER_LOG("need set outlet: ", num, ", power state: ", state);
    if (state == BLINKER_CMD_ON) {
        digitalWrite(LED_BUILTIN, HIGH);
        BlinkerMIOT.powerState("on", num);
        BlinkerMIOT.print();
        oState[num] = true;
        if(num == 1){
          Serial.printf("button1 ON");
          digitalWrite(motor_do, HIGH);
          Button1.print("on");
          motor_state = true;
        }
        else if(num == 2){
          Serial.printf("button2 ON");
          digitalWrite(led_do, HIGH);
          Button2.print("on");
        }
        else if(num == 3){
          Serial.printf("button3 ON");
          digitalWrite(pump_do, HIGH);
          Button3.print("on");
        }
    }
    else if (state == BLINKER_CMD_OFF) {
        digitalWrite(LED_BUILTIN, LOW);
        BlinkerMIOT.powerState("off", num);
        BlinkerMIOT.print();
        oState[num] = false;
        if(num == 1){
          Serial.printf("button1 OFF");
          digitalWrite(motor_do, LOW);
          Button1.print("off");
          motor_state = false;
        }
        else if(num == 2){
          Serial.printf("button2 OFF");
          digitalWrite(led_do, LOW);
          Button2.print("off");
        }
        else if(num == 3){
          Serial.printf("button1 OFF");
          digitalWrite(pump_do, LOW);
          Button3.print("off");
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
    //Blinker.attachData(dataRead);
    BlinkerMIOT.attachPowerState(miotPowerState);
    //BlinkerMIOT.attachQuery(miotQuery);
    Button1.attach(button1_callback);
    Button2.attach(button2_callback);
    Button3.attach(button3_callback);
  
}

void loop()
{
  Blinker.run();
  //采集到的DI电压值（mv）
  int analog_value = 0;
  analog_value = analogRead(motor_di);
  delay(100);
  if(time3s < 5){
    time3s = time3s +1;
  }
  //每3S测温一次
  else{
    sensors.requestTemperatures();  // 发送命令获取温度
    //Serial.print("Temperature for the device 1 (index 0) is: ");
    //Serial.println(sensors.getTempCByIndex(0));
    Number3.print(sensors.getTempCByIndex(0));
    time3s = 0;
  }
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
  if((motor_state == true)&&(count >50)&&(di_state == true)&&(di_state_old == true)){
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