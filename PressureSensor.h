#include "Arduino.h"

class PressureSensor {
  public:
    PressureSensor(uint8_t pinSensor);
    void Loop();
    bool SouldSendData();
    float SendData();
  private:
    int _pinSensor;
    float _pressure;
    float _lastSendData;
    const float sensorVoltageMax = 10.0;
    const float pressureRangeMax = 5.0;
    long _lastTimeSendData = 0;
    const int _timeInBetweenPublishment = 60000;
    const float publishThreshold = 0.06;
};

PressureSensor::PressureSensor(uint8_t pinSensor) {
  this->_pinSensor = pinSensor;
}

void PressureSensor::Loop() {
  int sensorValue = analogRead(this->_pinSensor);
  float outputVoltage = (sensorValue / 918.0) * this->sensorVoltageMax;

  this->_pressure = 0.5 * outputVoltage;
}

float PressureSensor::SendData() {
  this->_lastTimeSendData = millis();
  this->_lastSendData = this->_pressure;
  return this->_pressure;
}

bool PressureSensor::SouldSendData() {
  if (this->_lastSendData - this->_pressure > this->publishThreshold) {
    Serial.println(analogRead(this->_pinSensor));
    return true;
  }

  if (this->_pressure - this->_lastSendData > this->publishThreshold) {
    Serial.println(analogRead(this->_pinSensor));
    return true;
  }

  if(millis() - this->_lastTimeSendData > this->_timeInBetweenPublishment) {
    this->_lastTimeSendData = millis();
    return true;
  }

  return false;
}
