// ============================================================================
// RELAY IMPLEMENTATION
// ============================================================================

#include <Arduino.h>
#include "RelayControl.h"

RelayControl::RelayControl(uint8_t pin)
    : _pin(pin), _state(false), _onTime(0) {
}

void RelayControl::begin() {
    pinMode(_pin, OUTPUT);
    off();
}

void RelayControl::on() {
    if (!_state) {

        digitalWrite(_pin, RELAY_ON);
        _state = true;
        _onTime = millis();
        
        if (DEBUG_SERIAL) {
            Serial.println("[RELAY] Bomba LIGADA");
        }
    }
}

void RelayControl::off() {
    if (_state) {
        digitalWrite(_pin, RELAY_OFF);
        _state = false;
        
        unsigned long runtime = millis() - _onTime;
        if (DEBUG_SERIAL) {
            Serial.print("[RELAY] Bomba DESLIGADA (tempo: ");
            Serial.print(runtime);
            Serial.println("ms)");
        }
    }
}

bool RelayControl::isOn() const {
    return _state;
}

unsigned long RelayControl::getRuntime() const {
    if (!_state) return 0;
    return millis() - _onTime;
}
