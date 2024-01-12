/* *****************************************************************
 *
 * Download latest Blinker library here:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * 
 * Blinker is a cross-hardware, cross-platform solution for the IoT. 
 * It provides APP, device and server support, 
 * and uses public cloud services for data transmission and storage.
 * It can be used in smart home, data monitoring and other fields 
 * to help users build Internet of Things projects better and faster.
 * 
 * Make sure installed 2.7.4 or later ESP8266/Arduino package,
 * if use ESP8266 with Blinker.
 * https://github.com/esp8266/Arduino/releases
 * 
 * Make sure installed 1.0.5 or later ESP32/Arduino package,
 * if use ESP32 with Blinker.
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * Docs: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************
 * 
 * Blinker 库下载地址:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * Blinker 是一套跨硬件、跨平台的物联网解决方案，提供APP端、设备端、
 * 服务器端支持，使用公有云服务进行数据传输存储。可用于智能家居、
 * 数据监测等领域，可以帮助用户更好更快地搭建物联网项目。
 * 
 * 如果使用 ESP8266 接入 Blinker,
 * 请确保安装了 2.7.4 或更新的 ESP8266/Arduino 支持包。
 * https://github.com/esp8266/Arduino/releases
 * 
 * 如果使用 ESP32 接入 Blinker,
 * 请确保安装了 1.0.5 或更新的 ESP32/Arduino 支持包。
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * 文档: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************/

#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET

#include <Blinker.h>

char auth[] = "7c8e9ee2a1c1";
char ssid[] = "Redmi_42E5";
char pswd[] = "gulanjun";
int motor_do = 12;
int motor_di = 13;


BlinkerButton Button1("Auto_fish");

bool oState = false;
// 按下按键即会执行该函数
void button1_callback(const String & state)
{
    BLINKER_LOG("get button1 state: ", state);
    if(state == "on"){
      digitalWrite(motor_do, HIGH);
      Button1.print("on");
    }
    else if(state == "off"){
      digitalWrite(motor_do, LOW);
      Button1.print("off");
    }
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void miotPowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
        //digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(motor_do, HIGH);
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        Button1.print("on");
        oState = true;
    }
    else if (state == BLINKER_CMD_OFF) {
        digitalWrite(motor_do, LOW);
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        Button1.print("off");
        oState = false;
    }
}

void miotQuery(int32_t queryCode)
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("MIOT Query Power State");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
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
}