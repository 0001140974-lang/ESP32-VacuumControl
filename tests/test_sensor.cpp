// ============================================================================
// TESTE INDIVIDUAL - Sensor de Pressão I2C
// Usar este sketch para validar se sensor está funcionando
// ============================================================================

#include <Arduino.h>
#include <Wire.h>

#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22
#define SENSOR_ADDR 0x6D

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n=== TESTE SENSOR DE PRESSÃO I2C ===\n");
    
    // Inicializa I2C
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(100000);
    
    Serial.println("I2C inicializado (SDA=21, SCL=22, 100kHz)");
    Serial.println("Escaneando barramento I2C...\n");
    
    scanI2C();
    
    Serial.println("\nTestando leitura do sensor em 0x6D...\n");
}

void loop() {
    // Tenta ler do sensor
    uint16_t raw = readSensorRaw();
    float pressure = convertToPressure(raw);
    
    Serial.print("Raw: ");
    Serial.print(raw);
    Serial.print(" | Pressão: ");
    Serial.print(pressure, 2);
    Serial.println(" mbar");
    
    delay(500);
}''

void scanI2C() {
    int count = 0;
    for (uint8_t addr = 0x01; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("  ✓ Dispositivo encontrado: 0x");
            Serial.print(addr, HEX);
            Serial.print(" (");
            Serial.print(addr);
            Serial.println(")");
            count++;
        }
    }
    Serial.print("\nTotal: ");
    Serial.print(count);
    Serial.println(" dispositivo(s) encontrado(s)");
}

uint16_t readSensorRaw() {
    Wire.beginTransmission(SENSOR_ADDR);
    Wire.write(0xFE);
    Wire.endTransmission(false);
    
    delay(5);
    
    Wire.requestFrom(SENSOR_ADDR, (uint8_t)2);
    
    uint16_t raw = 0;
    if (Wire.available() >= 2) {
        uint8_t msb = Wire.read();
        uint8_t lsb = Wire.read();
        raw = (msb << 8) | lsb;
    }
    
    return raw;
}

float convertToPressure(uint16_t raw) {
    // XGZP6847D: 0-4095 → -100 a 1000 kPa
    float pressure_kpa = ((float)raw / 4095.0f) * 1100.0f - 100.0f;
    float pressure_mbar = pressure_kpa * 10.0f;
    return pressure_mbar;
}
