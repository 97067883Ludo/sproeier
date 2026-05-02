#include "Arduino.h"

class Valve {
  public:
    Valve(pin_size_t output, pin_size_t statusLed);
    void close();
    void open();
    bool State();
  private:
    pin_size_t _output;
    pin_size_t _statusLed;
    bool _open;
};

Valve::Valve(pin_size_t output, pin_size_t statusLed) {
  Valve::_output = output;
  Valve::_statusLed = statusLed;
}

void Valve::open() {
  this->_open = true;
  digitalWrite(Valve::_output, HIGH);
  digitalWrite(Valve::_statusLed, HIGH);
}

void Valve::close() {
  this->_open = false;
  digitalWrite(Valve::_output, LOW);
  digitalWrite(Valve::_statusLed, LOW);
}

bool Valve::State() {
  return this->_open;
}