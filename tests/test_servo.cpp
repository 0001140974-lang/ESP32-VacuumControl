// ============================================================================
// TESTE INDIVIDUAL - Servo Motor MG995
// Usar este sketch para validar se servo está funcionando
// ============================================================================

#include <Arduino.h>
#include <ESP32Servo.h>

#define PIN_SERVO 33

Servo servo;

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n=== TESTE SERVO MOTOR MG995 ===\n");
    Serial.println("Inicializando servo no GPIO 33...");
    Serial.println("IMPORTANTE: Servo conectado a 5V externo!\n");
    
    // Configurar servo
    servo.setPeriodHertz(50);  // 50 Hz para servos padrão
    servo.attach(PIN_SERVO, 500, 2500);  // Min e Max PWM em microsegundos
    
    Serial.println("Servo pronto!");
    Serial.println("Posições de teste:\n");
    
    delay(500);
}

void loop() {
    // Teste 1: Posição fechada (0°)
    Serial.println("→ Posição FECHADA (0°)");
    servo.write(0);
    delay(2000);
    
    // Teste 2: Posição intermediária (45°)
    Serial.println("→ Posição INTERMEDIÁRIA (45°)");
    servo.write(45);
    delay(2000);
    
    // Teste 3: Posição aberta (90°)
    Serial.println("→ Posição ABERTA (90°)");
    servo.write(90);
    delay(2000);
    
    // Teste 4: Varredura completa
    Serial.println("→ VARREDURA: 0° a 90°");
    for (int angle = 0; angle <= 90; angle += 5) {
        servo.write(angle);
        Serial.print("  Ângulo: ");
        Serial.print(angle);
        Serial.println("°");
        delay(200);
    }
    
    Serial.println("→ VARREDURA: 90° a 0°");
    for (int angle = 90; angle >= 0; angle -= 5) {
        servo.write(angle);
        Serial.print("  Ângulo: ");
        Serial.print(angle);
        Serial.println("°");
        delay(200);
    }
    
    Serial.println("\n--- Ciclo completado ---\n");
    delay(2000);
}
