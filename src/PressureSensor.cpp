// ============================================================================
// PRESSURE SENSOR IMPLEMENTATION
// ============================================================================

#include <Arduino.h>
#include "PressureSensor.h"

PressureSensor::PressureSensor(uint8_t i2c_addr) 
    : _addr(i2c_addr), _lastPressure(0.0f) {
}

bool PressureSensor::begin() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);
    
    // Verifica se sensor está presente no barramento
    return isConnected();
}

bool PressureSensor::isConnected() {
    Wire.beginTransmission(_addr);
    return Wire.endTransmission() == 0;
}

uint16_t PressureSensor::readRaw() {
    Wire.beginTransmission(_addr);
    Wire.write(0xFE);  // Comando de leitura do XGZP6847D
    Wire.endTransmission(false);
    
    // Aguarda conversão
    delay(5);
    
    // Lê 2 bytes de dados
    Wire.requestFrom(_addr, (uint8_t)2);
    
    uint16_t raw = 0;
    if (Wire.available() >= 2) {
        uint8_t msb = Wire.read();
        uint8_t lsb = Wire.read();
        raw = (msb << 8) | lsb;
    }
    
    return raw;
}

float PressureSensor::convertRawToPressure(uint16_t raw) {
    // XGZP6847D Datasheet V3.0:
    // Output: 21 bits efetivos (em 24 bits = 0-2097152)
    // Faixa padrão: -100 a 1000 kPa (para modelo GPN)
    
    // Fórmula oficial (página 7 do datasheet):
    // If sum < 8388608 (2^23): P = sum / 2^21 * (PMAX - PMIN) + PMIN
    // If sum >= 8388608:       P = (sum - 2^24) / 2^21 * (PMAX - PMIN) + PMIN
    // Resultado em Pa, depois converte para mbar
    
    float pmax = 100000.0f;  // 1000 kPa = 100000 Pa
    float pmin = -100000.0f; // -100 kPa = -100000 Pa
    
    float pressure_pa;
    if (raw < 8388608) {  // 2^23
        pressure_pa = ((float)raw / 2097152.0f) * (pmax - pmin) + pmin;  // 2^21
    } else {
        pressure_pa = (((float)raw - 16777216.0f) / 2097152.0f) * (pmax - pmin) + pmin;  // 2^24 = 16777216
    }
    
    // Converte Pa para mbar (1 Pa = 0.01 mbar)
    float pressure_mbar = pressure_pa / 100.0f;
    
    return pressure_mbar;
}

float PressureSensor::readPressure() {
    uint16_t raw = readRaw();
    _lastPressure = convertRawToPressure(raw);
    
    if (DEBUG_SERIAL) {
        Serial.print("[SENSOR] Raw: ");
        Serial.print(raw);
        Serial.print(" | Pressão: ");
        Serial.print(_lastPressure, 2);
        Serial.println(" mbar");
    }
    
    return _lastPressure;
}
