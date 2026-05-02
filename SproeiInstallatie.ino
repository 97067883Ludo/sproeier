#include "Valve.h"
#include "AlarmService.h"
#include "MqttClient.h"
#include "PressureSensor.h"
#include "TankFiller.h"

IPAddress ip(192, 168, 178, 180);
IPAddress dns(192, 168, 178, 1);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress server(192, 168, 178, 209);

Valve valve1(D0, LED_D0);
Valve valve2(D1, LED_D1);
Valve valve3(D2, LED_D2);
AlarmService alarmService(true);
TankFiller tankFiller(I2, I3, I4, D3, alarmService);
MqttClient mqttClient(ip, dns, gateway, subnet, server);
PressureSensor pressureSensor(I1);

void setup() {
  mqttClient.begin(AcutateValve);
  alarmService.StartupOk(mqttClient.connected());
  tankFiller.begin();
}

void loop() {
  mqttClient.loop();
  pressureSensor.Loop();
  tankFiller.loop();

  if(alarmService.AnyAlarms()) {
    valve1.close();
    valve2.close();
    valve3.close();
    tankFiller.close();
    return;
  }

  alarmService.CheckConnections(mqttClient.connected());
  

  if(pressureSensor.SouldSendData()) {
    mqttClient.send("huis/sproeisysteem/druk", pressureSensor.SendData(), false);
    Serial.print(pressureSensor.SendData());
  }
}

void AcutateValve(int valve, PinStatus Action) {
  switch(valve) {
    case 1:
      if(Action == HIGH) {
        valve1.open();
      } else {
        valve1.close();
      }
    break;
    case 2:
      if(Action == HIGH) {
        valve2.open();
      } else {
        valve2.close();
      }
    break;
    case 3:
      if(Action == HIGH) {
        valve3.open();
      } else {
        valve3.close();
      }
    break;
  }
}
