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

// ✅ CORRIGIDO: Retorna uint32_t ao invés de uint16_t
uint32_t PressureSensor::readRaw() {
    Wire.beginTransmission(_addr);
    Wire.write(0xFE);  // Comando de leitura do XGZP6847D
    Wire.endTransmission(false);
    
    // Aguarda conversão
    delay(5);
    
    // ✅ CORRIGIDO: Lendo 3 bytes (24 bits) ao invés de 2 bytes
    Wire.requestFrom(_addr, (uint8_t)3);
    
    // ✅ CORRIGIDO: uint32_t para armazenar 24 bits
    uint32_t raw = 0;
    
    // ✅ CORRIGIDO: Verifica se 3 bytes estão disponíveis
    if (Wire.available() >= 3) {
        uint8_t msb = Wire.read();       // Byte mais significativo
        uint8_t mid = Wire.read();       // Byte do meio
        uint8_t lsb = Wire.read();       // Byte menos significativo
        
        // ✅ CORRIGIDO: Combinação correta dos 3 bytes para 24 bits
        raw = ((uint32_t)msb << 16) | ((uint32_t)mid << 8) | lsb;
    }
    
    return raw;
}

// ✅ CORRIGIDO: Parâmetro muda de uint16_t para uint32_t
float PressureSensor::convertRawToPressure(uint32_t raw) {
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
    // ✅ CORRIGIDO: uint32_t ao invés de uint16_t
    uint32_t raw = readRaw();
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
