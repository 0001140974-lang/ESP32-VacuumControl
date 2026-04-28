// ============================================================================
// PRESSURE SENSOR SIMULATOR - Usando Potenciômetro (ADC)
// Para testes SEM sensor I2C real
// ============================================================================

#ifndef PRESSURE_SENSOR_SIMULATED_H
#define PRESSURE_SENSOR_SIMULATED_H

#include <Arduino.h>
#include "config.h"

#define PIN_POT_SENSOR 34  // GPIO 34 (ADC1_CH6) - entrada analógica

class PressureSensorSimulated {
public:
    PressureSensorSimulated();
    
    // Inicializa o pino analógico
    bool begin();
    
    // Lê valor de pressão em mbar (simula sensor)
    float readPressure();
    
    // Lê valor bruto do ADC (0-4095)
    uint16_t readRaw();
    
    // Sempre retorna true (simulado)
    bool isConnected();
    
private:
    float _lastPressure;
    
    // Converte ADC (0-4095) em pressão (1013 → 0.8 mbar)
    // 0 ADC = 1013 mbar (atmosférico)
    // 4095 ADC = 0.8 mbar (vácuo máximo)
    float convertRawToPressure(uint16_t raw);
};

#endif // PRESSURE_SENSOR_SIMULATED_H
