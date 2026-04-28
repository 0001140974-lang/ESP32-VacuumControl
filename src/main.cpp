// ============================================================================
// MAIN - ESP32 Vacuum Control System
// ============================================================================

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "PressureSensor.h"
#include "ServoControl.h"
#include "RelayControl.h"
#include "VacuumController.h"

// Instâncias globais dos componentes
PressureSensor pressureSensor;
ServoControl servo;
RelayControl relay;
VacuumController vacuumController(&pressureSensor, &servo, &relay);

// Timers
unsigned long lastSensorRead = 0;
unsigned long lastStatusPrint = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(500);
    
    Serial.println("\n========================================");
    Serial.println("ESP32 VACUUM CONTROL SYSTEM");
    Serial.println("2-Stage Vacuum Control");
    Serial.println("========================================\n");
    
    // Inicializa sensor de pressão
    Serial.print("Inicializando sensor de pressão...");
    if (pressureSensor.begin()) {
        Serial.println(" OK");
    } else {
        Serial.println(" ERRO!");
        while(1);  // Trava se sensor não encontrado
    }
    
    // Inicializa servo
    Serial.print("Inicializando servo motor...");
    servo.begin();
    Serial.println(" OK");
    
    // Inicializa relay
    Serial.print("Inicializando relay...");
    relay.begin();
    Serial.println(" OK");
    
    // Inicializa controlador
    Serial.print("Inicializando controlador de vácuo...");
    vacuumController.begin();
    Serial.println(" OK");
    
    Serial.println("\nSistema pronto!");
    Serial.println("Digite 'START' para iniciar vácuo, 'STOP' para parar");
    Serial.println("========================================\n");
}

void loop() {
    // Lê sensor a cada SENSOR_READ_INTERVAL ms
    if ((millis() - lastSensorRead) >= SENSOR_READ_INTERVAL) {
        vacuumController.update();
        lastSensorRead = millis();
    }
    
    // Imprime status a cada 2 segundos
    if ((millis() - lastStatusPrint) >= 2000) {
        printStatus();
        lastStatusPrint = millis();
    }
    
    // Processa comandos da serial
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toUpperCase();
        
        processCommand(command);
    }
    
    delay(10);  // Anti-busy-loop
}

void printStatus() {
    Serial.print("\n[STATUS] ");
    Serial.print(vacuumController.getStateMessage());
    Serial.print(" | P=");
    Serial.print(vacuumController.getCurrentPressure(), 1);
    Serial.print("mbar | Servo=");
    Serial.print(vacuumController.getServoPosition() * 100, 0);
    Serial.print("% | Bomba=");
    Serial.println(vacuumController.isPumpOn() ? "ON" : "OFF");
}

void processCommand(String cmd) {
    if (cmd == "START") {
        Serial.println("\n>>> Iniciando ciclo de vácuo...");
        vacuumController.startVacuum();
    }
    else if (cmd == "STOP") {
        Serial.println("\n>>> Parando ciclo de vácuo...");
        vacuumController.stopVacuum();
    }
    else if (cmd == "STATUS") {
        Serial.println("\n>>> Status do sistema:");
        printStatus();
    }
    else if (cmd == "HELP") {
        printHelp();
    }
    else if (cmd != "") {
        Serial.print("Comando desconhecido: ");
        Serial.println(cmd);
        Serial.println("Digite 'HELP' para ver comandos disponíveis");
    }
}

void printHelp() {
    Serial.println("\n========== COMANDOS DISPONÍVEIS ==========");
    Serial.println("START   - Inicia ciclo de vácuo");
    Serial.println("STOP    - Para ciclo de vácuo");
    Serial.println("STATUS  - Mostra status atual do sistema");
    Serial.println("HELP    - Mostra esta mensagem");
    Serial.println("==========================================\n");
}
