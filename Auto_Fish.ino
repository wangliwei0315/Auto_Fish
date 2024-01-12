#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Blinker.h>
//连接ESP32 G34引脚，用于采集DI反馈
#define motor_di   34
//连接ESP32 G12引脚，用于驱动直流电机
int motor_do = 12;
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
//手动控制喂鱼
BlinkerButton Button1("Auto_fish");
//单次喂鱼结束后，显示喂鱼时间（count的值）
BlinkerNumber Number1("num-abc");
//喂鱼过程中实时显示采集的DI值
BlinkerNumber Number2("num-di");
//实时显示采集的水温
BlinkerNumber Number3("num-temp");
//手动测温一次
BlinkerButton Button2("auto_temp");
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
void button2_callback(const String & state)
{
    BLINKER_LOG("get button2 state: ", state);
    if(state == "on"){
      Button1.print("on");
      sensors.requestTemperatures();  // 发送命令获取温度
      Number3.print(sensors.getTempCByIndex(0));
      Button1.print("off");
    }
    else if(state == "off"){
      Button1.print("off");
    }
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
    sensors.begin();
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
    Button2.attach(button2_callback);
  
}

void loop()
{
  Blinker.run();
  //采集到的DI电压值（mv）
  int analog_value = 0;
  analog_value = analogRead(motor_di);
  delay(100);
  /*
  if(time3s < 50){
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
  */
  if(analog_value > 4000){
    di_state = true;
  }
  else{
    di_state = false;
  }
  if(motor_state == true){
    count = count + 1;
  }
  if((count >148)&&(motor_state == true)){
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
  di_state_old = di_state;
}