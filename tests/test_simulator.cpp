// ============================================================================
// TESTE DO SIMULADOR - Potenciômetro em GPIO 34
// Use este sketch para validar se potenciômetro está funcionando
// ============================================================================

#include <Arduino.h>

#define PIN_POT 34  // GPIO 34 (ADC1_CH6)

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n=== TESTE POTENCIÔMETRO (Simulador) ===\n");
    Serial.println("Conecte potenciômetro 10kΩ em GPIO 34:");
    Serial.println("  - Pino 1 (esquerda)   → 3.3V");
    Serial.println("  - Pino 2 (centro)     → GPIO 34");
    Serial.println("  - Pino 3 (direita)    → GND\n");
    
    Serial.println("Girando potenciômetro:\n");
}

void loop() {
    uint16_t adcValue = analogRead(PIN_POT);
    float voltage = (adcValue / 4095.0f) * 3.3f;
    
    // Simular pressão (0 ADC = 1013 mbar, 4095 ADC = 0.8 mbar)
    float pressure = 1013.25f - (adcValue / 4095.0f) * (1013.25f - 0.8f);
    
    // Barra visual
    int barLength = adcValue / 256;  // 4096 / 16 = 256
    Serial.print("[");
    for (int i = 0; i < 16; i++) {
        if (i < barLength) Serial.print("█");
        else Serial.print("░");
    }
    Serial.print("] ");
    
    Serial.print("ADC: ");
    Serial.print(adcValue, 4);
    Serial.print(" | V: ");
    Serial.print(voltage, 2);
    Serial.print("V | P: ");
    Serial.print(pressure, 1);
    Serial.println("mbar");
    
    delay(500);
}
