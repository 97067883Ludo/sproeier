#include "Arduino.h"

class AlarmService {
  public:
    AlarmService(bool active);
    void SetAlarm(uint8_t alarmCode);
    bool AnyAlarms();
    void StartupOk(bool MqttClientConnection);
    void CheckConnections(bool mqttConnection);
  private:
    void _resetAlarm();
    void _setLightStatus();
    void _listenForReset();
    bool _alarmStatusChanged();
    uint8_t _lastAlarmCode;
    uint8_t _alarmCode;
    long _lastChecked;
    int _checkInterval = 1000;
    bool _active;
};

AlarmService::AlarmService(bool active) {
  AlarmService::_active = active;
}

void AlarmService::SetAlarm(uint8_t alarmCode) {
  AlarmService::_alarmCode = alarmCode;
}

bool AlarmService::AnyAlarms() {
  if(AlarmService::_alarmStatusChanged()) {
    AlarmService::_setLightStatus();
    AlarmService::_lastAlarmCode = AlarmService::_alarmCode;
  }

  if(AlarmService::_alarmCode > 0) {
    AlarmService::_listenForReset();
  }

  return AlarmService::_alarmCode > 0;
}

void AlarmService::_setLightStatus() {
  
  if(AlarmService::_alarmCode == 0) {
      digitalWrite(LEDG, HIGH);
  } else {
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDR, HIGH);
  }
  if(AlarmService::_alarmStatusChanged() && AlarmService::_alarmCode == 0) {
    digitalWrite(LEDR, LOW);
  }
}

void AlarmService::_resetAlarm() {
  AlarmService::SetAlarm(0);
}

void AlarmService::StartupOk(bool MqttClientConnection) {
  if(!MqttClientConnection) {
    AlarmService::SetAlarm(1);
  }
  
  AlarmService::_setLightStatus();
}

void AlarmService::_listenForReset() {
  if(digitalRead(BTN_USER) == 0) {
    AlarmService::_resetAlarm();
  }
}

bool AlarmService::_alarmStatusChanged() {
  return AlarmService::_lastAlarmCode != AlarmService::_alarmCode;
}

void AlarmService::CheckConnections(bool mqttConnection) {
  if(millis() - this->_lastChecked > this->_checkInterval) {
    if(!mqttConnection) {
      AlarmService::SetAlarm(1);
    }
  }
}