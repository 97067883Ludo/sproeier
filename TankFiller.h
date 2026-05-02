#include "Arduino.h"
#include "AlarmService.h"

// Alarm code raised when the safety sensor triggers during filling.
// Alarm code 1 is reserved for MQTT disconnect (see AlarmService).
static const uint8_t ALARM_TANK_FAULT = 2;

class TankFiller {
  public:
    TankFiller(pin_size_t safetyPin, pin_size_t lowerPin, pin_size_t upperPin,
               pin_size_t valvePin, AlarmService& alarmService);
    void begin();
    void loop();
    void close();

  private:
    enum State { IDLE, FILLING, FULL, ALARM };

    void _openValve();
    void _closeValve();

    pin_size_t _safetyPin; // I2 — HIGH = fault condition
    pin_size_t _lowerPin;  // I3 — LOW  = level below lower sensor, needs fill
    pin_size_t _upperPin;  // I4 — HIGH = tank full
    pin_size_t _valvePin;  // D3 — HIGH = fill valve open

    AlarmService& _alarmService;
    State _state;
};

TankFiller::TankFiller(pin_size_t safetyPin, pin_size_t lowerPin, pin_size_t upperPin,
                       pin_size_t valvePin, AlarmService& alarmService)
  : _safetyPin(safetyPin),
    _lowerPin(lowerPin),
    _upperPin(upperPin),
    _valvePin(valvePin),
    _alarmService(alarmService),
    _state(IDLE)
{}

void TankFiller::begin() {
  pinMode(_safetyPin, INPUT);
  pinMode(_lowerPin,  INPUT);
  pinMode(_upperPin,  INPUT);
  pinMode(_valvePin,  OUTPUT);
  _closeValve(); // always start safe
}

void TankFiller::loop() {
  switch (_state) {

    case IDLE:
      // Start filling when the water level drops below the lower sensor.
      if (digitalRead(_lowerPin) == LOW) {
        _openValve();
        _state = FILLING;
      }
      break;

    case FILLING:
      // Safety sensor triggered — close valve immediately and raise alarm.
      if (digitalRead(_safetyPin) == HIGH) {
        _closeValve();
        _alarmService.SetAlarm(ALARM_TANK_FAULT);
        _state = ALARM;
        break;
      }
      // Upper sensor reached — tank is full, close valve.
      if (digitalRead(_upperPin) == HIGH) {
        _closeValve();
        _state = FULL;
      }
      break;

    case FULL:
      // Water has been consumed enough to drop below the lower sensor again.
      if (digitalRead(_lowerPin) == LOW) {
        _openValve();
        _state = FILLING;
      }
      break;

    case ALARM:
      // Only return to IDLE once AlarmService confirms the alarm has been
      // cleared (BTN_USER pressed). Safe to call loop() at any time.
      if (!_alarmService.AnyAlarms()) {
        _state = IDLE;
      }
      break;
  }
}

void TankFiller::close() {
  _closeValve();
  _state = ALARM;
}

void TankFiller::_openValve() {
  digitalWrite(_valvePin, HIGH);
}

void TankFiller::_closeValve() {
  digitalWrite(_valvePin, LOW);
}
