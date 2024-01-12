/* *****************************************************************
 *
 * Download latest Blinker library here:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
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