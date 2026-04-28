// ============================================================================
// TESTE INDIVIDUAL - Relay (Controle de Bomba)
// Usar este sketch para validar se relay está funcionando
// ============================================================================

#include <Arduino.h>

#define PIN_RELAY 32

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n=== TESTE RELAY - CONTROLE DE BOMBA ===\n");
    Serial.println("Pino: GPIO 32");
    Serial.println("Esperado: Som de 'CLICK' ao ligar/desligar\n");
    
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, LOW);  // Inicia desligado
    
    Serial.println("Relay inicializado (desligado)\n");
}

void loop() {
    // Teste 1: Liga por 2 segundos
    Serial.println("→ RELAY LIGANDO (esperado: CLICK)");
    digitalWrite(PIN_RELAY, HIGH);
    delay(2000);
    
    // Teste 2: Desliga por 2 segundos
    Serial.println("→ RELAY DESLIGANDO (esperado: CLICK)");
    digitalWrite(PIN_RELAY, LOW);
    delay(2000);
    
    // Teste 3: Ciclo rápido (simula bomba ligando/desligando)
    Serial.println("→ CICLO RÁPIDO (10 vezes):");
    for (int i = 0; i < 10; i++) {
        digitalWrite(PIN_RELAY, HIGH);
        delay(200);
        digitalWrite(PIN_RELAY, LOW);
        delay(200);
        Serial.print("  ");
        Serial.print(i + 1);
        Serial.println(" ✓");
    }
    
    // Teste 4: Repouso
    Serial.println("→ Repouso (5 segundos)\n");
    delay(5000);
}
