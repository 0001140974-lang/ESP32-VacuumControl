// ============================================================================
// PRESSURE SENSOR DRIVER - XGZP6847D001MP (I2C)
// ============================================================================

#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "config.h"

class PressureSensor {
public:
    PressureSensor(uint8_t i2c_addr = SENSOR_I2C_ADDR);
    
    // Inicializa o sensor e barramento I2C
    bool begin();
    
    // Lê valor de pressão em mbar
    float readPressure();
    
    // Lê valor bruto do sensor (24 bits → 21 bits efetivos)
    uint32_t readRaw();
    
    // Verifica se sensor está disponível
    bool isConnected();
    
private:
    uint8_t _addr;
    float _lastPressure;
    
    // Converte valor digital para pressão em mbar
    // Sensor retorna 24 bits para -100 a 1000 kPa
    // ✅ CORRIGIDO: uint32_t ao invés de uint16_t
    float convertRawToPressure(uint32_t raw);
};

#endif // PRESSURE_SENSOR_H
