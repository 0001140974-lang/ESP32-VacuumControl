#include "PressureSensor.h"

PressureSensor::PressureSensor(uint8_t i2c_addr) 
    : _addr(i2c_addr), _lastPressure(0.0f), _offset(0.0f), _debug(true) {
}

bool PressureSensor::begin(int sda_pin, int scl_pin) {
    Wire.begin(sda_pin, scl_pin);
    Wire.setClock(100000); 
    delay(100); 
    return isConnected();
}

bool PressureSensor::isConnected() {
    Wire.beginTransmission(_addr);
    return (Wire.endTransmission() == 0);
}

uint32_t PressureSensor::readRaw() {
    // 1. Comando de Início de Conversão (0x30 = Pressão)
    Wire.beginTransmission(_addr);
    Wire.write(0x30);
    Wire.write(0x0A);
    Wire.endTransmission();

    // Aguarda a conversão (mínimo 20ms segundo datasheet, 50ms é seguro)
    delay(50); 

    // 2. Aponta para o registrador de saída (0x06)
    Wire.beginTransmission(_addr);
    Wire.write(0x06);
    Wire.endTransmission(); 

    // 3. Solicita os 3 bytes (24 bits)
    Wire.requestFrom(_addr, (uint8_t)3);

    uint32_t raw = 0;
    if (Wire.available() >= 3) {
        uint8_t msb = Wire.read();
        uint8_t mid = Wire.read();
        uint8_t lsb = Wire.read();

        // Monta o valor de 24 bits
        raw = ((uint32_t)msb << 16) | ((uint32_t)mid << 8) | lsb;
    }
    return raw;
}

float PressureSensor::convertRawToPressure(uint32_t raw) {
    long signed_raw;
    
    // 1. Tratamento correto do sinal de 24 bits
    if (raw & 0x800000) { 
        signed_raw = (long)(raw | 0xFF000000); 
    } else {
        signed_raw = (long)raw;
    }

    // 2. Constantes do Modelo XGZP6847D001MPGPN
    // PMAX = 1000 kPa, PMIN = -100 kPa -> Span = 1100 kPa
    float span_pa = 1100000.0f; 

    // 3. O DIVISOR (Aqui está o segredo)
    // Se com 2097152.0f (2^21) deu -2500mbar, vamos tentar 2^22 ou um valor calibrado.
    // Tenta primeiro 5242880.0f (que é 2.5x o valor anterior) para corrigir o erro de escala.
    float divisor = 5242880.0f; 

    // Cálculo seguindo o padrão do datasheet (Pág 15)
    float pressure_pa = ((float)signed_raw / divisor) * span_pa;

    // Retorna em mbar (1 Pa = 0.01 mbar)
    return pressure_pa * 0.01f;
}

void PressureSensor::calibrateZero(int samples) {
    if (_debug) Serial.println("[SENSOR] Calibrando zero atmosferico...");
    
    float sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += convertRawToPressure(readRaw());
        delay(30);
    }
    
    _offset = sum / samples;
    
    if (_debug) {
        Serial.print("[SENSOR] Offset (Zero): ");
        Serial.print(_offset);
        Serial.println(" mbar");
    }
}

float PressureSensor::readPressure() {
    uint32_t raw = readRaw();
    float absolute_mbar = convertRawToPressure(raw);
    
    // Invertemos o sinal aqui (colocando um - na frente) 
    // para que vácuo = números negativos no gráfico
    _lastPressure = -(absolute_mbar - _offset); 

    if (_debug) {
        Serial.print("[SENSOR] Raw: ");
        Serial.print(raw);
        Serial.print(" | mbar: ");
        Serial.println(_lastPressure, 2);
    }
    
    return _lastPressure;
}
