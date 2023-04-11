#include "ESP8266.h"
#include <EEPROM.h>
//пароль и SSID сети WIFI, номер порта подключения

#define HOST_NAME   "192.168.1.23"
#define CLIENT_MUX 4 
#define TCP_PORT 8090

//назначение ножек

#define MOV_PIN 2
#define REL_PIN 3

//состояния

#define CHIP_OFF 0
#define DOOR_CLOSED 1
#define DOOR_OPEN 2
#define ALARM_ON 3

//отправляемые сообщения


//переменные состояния

int state = CHIP_OFF;
bool alarmSignalSent = false;

//класс WI-FI модуля

ESP8266 wifi(Serial1,115200);

String SSID = "";
String PASSWORD = "";

void setup()
{                                                                              

    Serial.begin(9600);
    pinMode(MOV_PIN, INPUT);
    pinMode(REL_PIN, OUTPUT);
    bool NewWiFi = false;

    ReadAPDataFromEEPROM();

    
    while(!wifi.joinAP(SSID, PASSWORD)) 
    {
        Serial.print("Join AP failure\r\n");
        GetAPData();
        NewWiFi = true;
    }
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");       
    Serial.println(wifi.getLocalIP().c_str());
    if(NewWiFi)
    {
      WriteAPDataToEEPROM();
    }
    
    TCPServerInitialization();

    //Serial.print("setup end\r\n");
}

void TCPServerInitialization()
{
    //new
        
    if(wifi.setOprToStation()) 
    {
        Serial.print("to station ok\r\n");
    } else {
        Serial.print("to station err\r\n");
    }
    
    if (wifi.enableMUX()) {
        Serial.print("multiple ok\r\n");
    } else {
        Serial.print("multiple err\r\n");
    }

    if (wifi.startTCPServer(TCP_PORT)) {
        Serial.print("start tcp server ok\r\n");
    } else {
        Serial.print("start tcp server err\r\n");
    }

    if (wifi.setTCPServerTimeout(10)) { 
        Serial.print("set tcp server timout 10 seconds\r\n");
    } else {
        Serial.print("set tcp server timout err\r\n");
    }
    
}


String AcceptMessage()//можно добавить проверку IP
{
    String Signal;
    uint8_t buffer[128] = {0};
    uint8_t mux_id;
    uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
    if (len > 0) {
        for(uint32_t i = 0; i < len; i++) {
            Signal+=(char)buffer[i];
        }
        if(Signal == "CHECK")
        {
          if(wifi.send(mux_id, buffer, len)) {
              Serial.print("send back ok\r\n");
          } else {
              Serial.print("send back err\r\n");
          }
        }
        
       if (wifi.releaseTCP(mux_id)) {
            Serial.print("release tcp ");
            Serial.print(mux_id);
            Serial.println(" ok");
        } else {
            Serial.print("release tcp");
            Serial.print(mux_id);
            Serial.println(" err");
        }
    }
    return Signal;
}


bool SendMessage(uint8_t* Buffer)
{
    if (wifi.createTCP(CLIENT_MUX, HOST_NAME, TCP_PORT)) 
    {  
      Serial.print("create tcp ok\r\n");
    } else {
      Serial.print("create tcp err\r\n");
      return 0;
    }
    wifi.send(CLIENT_MUX,(const uint8_t*)Buffer, strlen(Buffer));
    if (wifi.releaseTCP(CLIENT_MUX)) {
      Serial.print("release tcp ok\r\n");
    } else {
      Serial.print("release tcp err\r\n");
    }    
    return 1;
}

void ReadAPDataFromEEPROM()
{
  SSID = "";
  PASSWORD = "";
  int address = 0;
  char k;
  do
  {
    k = EEPROM.read(address);
    SSID += k;
    address++;
  }while(k != '\0');
  do
  {
    k = EEPROM.read(address);
    PASSWORD += k;
    address++;
  }while(k != '\0');
  SSID.remove(SSID.length()-1);
  PASSWORD.remove(PASSWORD.length()-1);
}

void WriteAPDataToEEPROM()
{
  int address = 0;
  for(int i = 0; i < SSID.length(); i++)
  {
    EEPROM.write(address, SSID[i]);
    address++;
  }
  EEPROM.write(address, '\0');
  address++;
  for(int i = 0; i < PASSWORD.length(); i++)
  {
    EEPROM.write(address, PASSWORD[i]);
    address++;
  }
  EEPROM.write(address, '\0');
}

void GetAPData()
{
  Serial.println("Enter SSID");
  while(!Serial.available())
  {
    delay(5000);
  }
  SSID = Serial.readString();
  SSID.remove(SSID.length()-2);
  Serial.println("Enter Password");
  while(!Serial.available())
  {
    delay(5000);
  }
  PASSWORD = Serial.readString();
  PASSWORD.remove(PASSWORD.length()-2);
}
                        
void loop()
{
  String Signal = "";
  Signal = AcceptMessage();
  //Прием и обработка сообщения
  if(Signal.length() > 0)
  {
    Serial.println(Signal);
    if(Signal == "ON")
    {
      if(state == CHIP_OFF)
      {
        Serial.println("Chip On");
        state = DOOR_CLOSED;
      }
    }
    if(Signal == "OFF")
    {
      digitalWrite(REL_PIN, LOW);
      Serial.println("Chip Off");
      state = CHIP_OFF;
    }
    if(Signal == "DOOR")
    {
      if(state == DOOR_OPEN)
      {
        Serial.println("Door Closed");
        digitalWrite(REL_PIN, LOW);
        state = DOOR_CLOSED;
      } else if(state == DOOR_CLOSED)
      {
        Serial.println("Door Opened");
        state = DOOR_OPEN;
      }
    }
    if(Signal == "ALARM")
    {
      if(state == ALARM_ON)
      {
        Serial.println("Alarm Off");
        state = DOOR_CLOSED;
        alarmSignalSent = false;
      }
    }      
  }

  if(state != CHIP_OFF)
  {
    if(state == ALARM_ON)
    {
      if(!alarmSignalSent)
      {
        if(SendMessage("ALARMON"))
        {
          alarmSignalSent = true;
        }
      }
      digitalWrite(REL_PIN, HIGH);
      delay(1000);                      
      digitalWrite(REL_PIN, LOW);   
      delay(1000);
    }
    else
    { 
      if (digitalRead(MOV_PIN))//держит логическую единицу долго, поэтому если дверь закрыть во время того, как горит свет, сразу включится сигнализация.
      {
        if(state == DOOR_OPEN)
        {
          digitalWrite(REL_PIN, HIGH);
        }
        if(state == DOOR_CLOSED)
        {
          state = ALARM_ON;
        }
      }
      else 
      {
        digitalWrite(REL_PIN, LOW);
      }
    }
  }
}
