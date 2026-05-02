#include "Valve.h"
#include "AlarmService.h"
#include "MqttClient.h"
#include "PressureSensor.h"

IPAddress ip(192, 168, 178, 180);
IPAddress dns(192, 168, 178, 1);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress server(192, 168, 178, 209);

Valve valve1(D0, LED_D0);
Valve valve2(D1, LED_D1);
Valve valve3(D2, LED_D2);
Valve valve4(D3, LED_D3);
AlarmService alarmService(true);
MqttClient mqttClient(ip, dns, gateway, subnet, server);
PressureSensor pressureSensor(I1);

void setup() {
  mqttClient.begin(AcutateValve);
  alarmService.StartupOk(mqttClient.connected());
}

void loop() {
  mqttClient.loop();
  pressureSensor.Loop();
  if(alarmService.AnyAlarms()) {
    valve1.close();
    valve2.close();
    valve3.close();
    valve4.close();
    return;
  }

  alarmService.CheckConnections(mqttClient.connected());

  if(pressureSensor.SouldSendData()) {
    mqttClient.send("huis/sproeisysteem/druk", pressureSensor.SendData(), false);
    Serial.print(pressureSensor.SendData());
  }
}

void AcutateValve(int valve, PinStatus Action) {
  if(!valve4.State() && Action == HIGH) {
    valve4.open();
  }

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

  if(CanStopFrequencyInverter()) {
    Serial.println("Closing 4");
    valve4.close();
  }
}

bool CanStopFrequencyInverter() {

  if(valve1.State()) {
    return false;
  } else if(valve2.State()) {
    return false;
  } else if(valve3.State()) {
    return false;
  }

  return true;
}
