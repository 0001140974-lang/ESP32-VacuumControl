
// ============================================================================
// PRESSURE SENSOR SIMULATOR IMPLEMENTATION
// ============================================================================

#include <Arduino.h>
#include "PressureSensorSimulated.h"

PressureSensorSimulated::PressureSensorSimulated() 
    : _lastPressure(1013.25f) {
}

bool PressureSensorSimulated::begin() {
    pinMode(PIN_POT_SENSOR, INPUT);
    Serial.println("[SENSOR_SIM] Potenciômetro inicializado em GPIO 34");
    return true;  // Simulado sempre sucede
}

bool PressureSensorSimulated::isConnected() {
    return true;  // Sempre conectado (simulado)
}

uint16_t PressureSensorSimulated::readRaw() {
    // Lê valor analógico do potenciômetro
    uint16_t raw = analogRead(PIN_POT_SENSOR);
    return raw;
}

float PressureSensorSimulated::convertRawToPressure(uint16_t raw) {
    // Mapeamento linear:
    // ADC 0 = 1013 mbar (sem vácuo)
    // ADC 4095 = 0.8 mbar (vácuo máximo)
    
    // Fórmula: P = 1013.25 - (raw / 4095) * (1013.25 - 0.8)
    float pressure = 1013.25f - ((float)raw / 4095.0f) * (1013.25f - 0.8f);
    
    // Limita valores realistas
    if (pressure < 0.8f) pressure = 0.8f;
    if (pressure > 1013.25f) pressure = 1013.25f;
    
    return pressure;
}

float PressureSensorSimulated::readPressure() {
    uint16_t raw = readRaw();
    _lastPressure = convertRawToPressure(raw);
    
    if (DEBUG_SERIAL) {
        Serial.print("[SENSOR_SIM] ADC: ");
        Serial.print(raw);
        Serial.print(" | Pressão: ");
        Serial.print(_lastPressure, 2);
        Serial.println(" mbar");
    }
    
    return _lastPressure;
}
