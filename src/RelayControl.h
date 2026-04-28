// ============================================================================
// RELAY CONTROL - Bomba de Vácuo
// ============================================================================

#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>
#include "config.h"

class RelayControl {
public:
    RelayControl(uint8_t pin = PIN_RELAY);
    
    // Inicializa relay
    void begin();
    
    // Liga bomba
    void on();
    
    // Desliga bomba
    void off();
    
    // Retorna estado (true = ligado)
    bool isOn() const;
    
    // Tempo de funcionamento da bomba em ms
    unsigned long getRuntime() const;
    
private:
    uint8_t _pin;
    bool _state;
    unsigned long _onTime;  // Timestamp quando ligou
};

#endif // RELAY_CONTROL_H
