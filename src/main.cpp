// ======================================================================
// MAIN - ESP32 Vacuum Control System
// COM WIFI + HTTP POST → DevTunnel
// ======================================================================

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"
#include "PressureSensor.h"
#include "ServoControl.h"
#include "RelayControl.h"
#include "VacuumController.h"

// ===== CONFIGURAÇÃO WIFI + API =====
const char* WIFI_SSID = "sua_rede_aqui";
const char* WIFI_PASS = "sua_senha_aqui";
const char* API_URL   = "https://kg6l3t40-5000.brs.devtunnels.ms/api/leiturasSensores";

// Instâncias globais dos componentes
PressureSensor pressureSensor;
ServoControl servo;
RelayControl relay;
VacuumController vacuumController(&pressureSensor, &servo, &relay);

// Timers
unsigned long lastSensorRead = 0;
unsigned long lastStatusPrint = 0;
unsigned long lastApiSend = 0;

// ✅ CORRIGIDO: Forward declarations das funções
// Isso permite usar as funções antes de defini-las
void conectarWiFi();
void enviarParaAPI();
void printStatus();
void processCommand(String cmd);
void printHelp();

// ======================================================================
// WIFI
// ======================================================================
void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Conectando ao WiFi");
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(" FALHOU! Continuando sem WiFi...");
  }
}

// ======================================================================
// HTTP POST → API
// ======================================================================
void enviarParaAPI() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[HTTP] WiFi desconectado, tentando reconectar...");
    conectarWiFi();
    return;
  }

  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000); // ✅ 5 segundos ao invés de 3

  // ✅ Concatenação adequada de strings
  String json = "{";
  json += "\"cicloId\":1,";
  
  // ✅ Separar concatenações
  json += "\"estadoMaquina\":\"";
  json += vacuumController.getStateMessage();
  json += "\",";
  
  json += "\"pressaoCamaraMbar\":";
  json += String(vacuumController.getCurrentPressure(), 2);
  json += ",";
  
  json += "\"pressaoTubo1Mbar\":";
  json += String(vacuumController.getCurrentPressure() * 0.6f, 2);
  json += ",";
  
  json += "\"fluxoTubo1LPM\":";
  json += String(vacuumController.getCurrentPressure() / 200.0f, 2);
  json += ",";
  
  json += "\"pressaoTubo2Mbar\":";
  json += String(vacuumController.getCurrentPressure() * 0.5f, 2);
  json += ",";
  
  json += "\"fluxoTubo2LPM\":";
  json += String(vacuumController.getCurrentPressure() / 220.0f, 2);
  json += ",";
  
  json += "\"pressaoTubo3Mbar\":";
  json += String(vacuumController.getCurrentPressure() * 0.55f, 2);
  json += ",";
  
  json += "\"fluxoTubo3LPM\":";
  json += String(vacuumController.getCurrentPressure() / 210.0f, 2);
  json += ",";
  
  json += "\"bombaLigada\":";
  json += (vacuumController.isPumpOn() ? "true" : "false");
  json += ",";
  
  json += "\"valvulaAberta\":";
  json += (vacuumController.getServoPosition() > 0 ? "true" : "false");
  json += ",";
  
  json += "\"servoAngulo\":";
  json += String((int)(vacuumController.getServoPosition() * 90));
  json += "}";

  // Debug: mostra JSON a enviar
  // if (DEBUG_SERIAL) {
  //   Serial.println("[HTTP] JSON a enviar:");
  //   Serial.println(json);
  // }

  int httpCode = http.POST(json);

  // ✅ Validação completa de resposta HTTP
  if (httpCode == 200) {
    Serial.println("[HTTP] ✓ Dados enviados com sucesso!");
  } else if (httpCode < 0) {
    Serial.print("[HTTP] ✗ Erro: ");
    Serial.println(http.errorToString(httpCode));
  } else {
    Serial.print("[HTTP] Status HTTP: ");
    Serial.println(httpCode);
  }

  http.end();
}

// ======================================================================
// SETUP
// ======================================================================
void setup() {
  Serial.begin(115200); // Substituído SERIAL_BAUD para garantir que conecte certo
  delay(500);

  Serial.println("\n========================================");
  Serial.println("ESP32 VACUUM CONTROL SYSTEM");
  Serial.println("2-Stage Vacuum Control + API WiFi");
  Serial.println("========================================\n");

  // Conecta WiFi primeiro
  conectarWiFi();

  // Inicializa sensor de pressão
  Serial.print("Inicializando sensor de pressao...");
  if (pressureSensor.begin()) {
    Serial.println(" OK");
    
    // ✅ O PULO DO GATO ESTÁ AQUI! Zera a pressão atmosférica lendo 50 amostras
    pressureSensor.calibrateZero(50);
    
  } else {
    Serial.println(" ERRO!");
    while(1);
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
  Serial.print("Inicializando controlador de vacuo...");
  vacuumController.begin();
  Serial.println(" OK");

  Serial.println("\nSistema pronto!");
  Serial.println("Digite 'START' para iniciar vacuo, 'STOP' para parar");
  Serial.println("========================================\n");
}

// ======================================================================
// LOOP
// ======================================================================
void loop() {
  // Lê sensor e atualiza máquina de estados a cada 100ms
  // Ajustado SENSOR_READ_INTERVAL direto para 100 para evitar erro se a macro faltar
  if ((millis() - lastSensorRead) >= 100) { 
    vacuumController.update();
    lastSensorRead = millis();
  }

  // Envia dados pra API a cada 1 segundo
  if ((millis() - lastApiSend) >= 1000) {
    enviarParaAPI();
    lastApiSend = millis();
  }

  // Imprime status no Serial a cada 1 segundo
  if ((millis() - lastStatusPrint) >= 1000) {
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

  delay(10);
}

// ======================================================================
// FUNÇÕES AUXILIARES
// ======================================================================
void printStatus() {
  Serial.print("\n[STATUS] ");
  Serial.print(vacuumController.getStateMessage());
  Serial.print(" | P=");
  Serial.print(vacuumController.getCurrentPressure(), 1);
  Serial.print("mbar | Servo=");
  Serial.print(vacuumController.getServoPosition() * 100, 0);
  Serial.print("% | Bomba=");
  Serial.print(vacuumController.isPumpOn() ? "ON" : "OFF");
  Serial.print(" | WiFi=");
  Serial.println(WiFi.status() == WL_CONNECTED ? "OK" : "DESCONECTADO");
}

void processCommand(String cmd) {
  if (cmd == "START") {
    Serial.println("\n>>> Iniciando ciclo de vacuo...");
    vacuumController.startVacuum();
  }
  else if (cmd == "STOP") {
    Serial.println("\n>>> Parando ciclo de vacuo...");
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
    Serial.println("Digite 'HELP' para ver comandos disponiveis");
  }
}

void printHelp() {
  Serial.println("\n========== COMANDOS DISPONIVEIS ==========");
  Serial.println("START  - Inicia ciclo de vacuo");
  Serial.println("STOP   - Para ciclo de vacuo");
  Serial.println("STATUS - Mostra status atual do sistema");
  Serial.println("HELP   - Mostra esta mensagem");
  Serial.println("==========================================\n");
}
