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
const char* WIFI_SSID = "NOME_DA_SUA_REDE";
const char* WIFI_PASS = "SENHA_DA_SUA_REDE";
const char* API_URL   = "https://pj9njgnt-5000.brs.devtunnels.ms/api/leiturasSensores";

// Instâncias globais dos componentes
PressureSensor pressureSensor;
ServoControl servo;
RelayControl relay;
VacuumController vacuumController(&pressureSensor, &servo, &relay);

// Timers
unsigned long lastSensorRead = 0;
unsigned long lastStatusPrint = 0;
unsigned long lastApiSend = 0;

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
  http.setTimeout(3000); // 3 segundos de timeout

  // Monta JSON com os dados atuais do controlador
  String json = "{";
  json += "\"cicloId\":1,";
  json += "\"estadoMaquina\":\"" + vacuumController.getStateMessage() + "\",";
  json += "\"pressaoCamaraMbar\":"  + String(vacuumController.getCurrentPressure(), 2) + ",";
  json += "\"pressaoTubo1Mbar\":"   + String(vacuumController.getCurrentPressure() * 0.6f, 2) + ",";
  json += "\"fluxoTubo1LPM\":"      + String(vacuumController.getCurrentPressure() / 200.0f, 2) + ",";
  json += "\"pressaoTubo2Mbar\":"   + String(vacuumController.getCurrentPressure() * 0.5f, 2) + ",";
  json += "\"fluxoTubo2LPM\":"      + String(vacuumController.getCurrentPressure() / 220.0f, 2) + ",";
  json += "\"pressaoTubo3Mbar\":"   + String(vacuumController.getCurrentPressure() * 0.55f, 2) + ",";
  json += "\"fluxoTubo3LPM\":"      + String(vacuumController.getCurrentPressure() / 210.0f, 2) + ",";
  json += "\"bombaLigada\":"        + String(vacuumController.isPumpOn() ? "true" : "false") + ",";
  json += "\"valvulaAberta\":"      + String(vacuumController.getServoPosition() > 0 ? "true" : "false") + ",";
  json += "\"servoAngulo\":"        + String((int)(vacuumController.getServoPosition() * 90));
  json += "}";

  int httpCode = http.POST(json);

  if (DEBUG_SERIAL) {
    Serial.print("[HTTP] POST → código: ");
    Serial.println(httpCode);
    if (httpCode < 0) {
      Serial.print("[HTTP] Erro: ");
      Serial.println(http.errorToString(httpCode));
    }
  }

  http.end();
}

// ======================================================================
// SETUP
// ======================================================================
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("ESP32 VACUUM CONTROL SYSTEM");
  Serial.println("2-Stage Vacuum Control + API");
  Serial.println("========================================\n");

  // Conecta WiFi primeiro
  conectarWiFi();

  // Inicializa sensor de pressão
  Serial.print("Inicializando sensor de pressão...");
  if (pressureSensor.begin()) {
    Serial.println(" OK");
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
  Serial.print("Inicializando controlador de vácuo...");
  vacuumController.begin();
  Serial.println(" OK");

  Serial.println("\nSistema pronto!");
  Serial.println("Digite 'START' para iniciar vácuo, 'STOP' para parar");
  Serial.println("========================================\n");
}

// ======================================================================
// LOOP
// ======================================================================
void loop() {
  // Lê sensor e atualiza máquina de estados a cada 100ms
  if ((millis() - lastSensorRead) >= SENSOR_READ_INTERVAL) {
    vacuumController.update();
    lastSensorRead = millis();
  }

  // Envia dados pra API a cada 1 segundo
  if ((millis() - lastApiSend) >= 1000) {
    enviarParaAPI();
    lastApiSend = millis();
  }

  // Imprime status no Serial a cada 2 segundos
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
  Serial.println("START  - Inicia ciclo de vácuo");
  Serial.println("STOP   - Para ciclo de vácuo");
  Serial.println("STATUS - Mostra status atual do sistema");
  Serial.println("HELP   - Mostra esta mensagem");
  Serial.println("==========================================\n");
}
