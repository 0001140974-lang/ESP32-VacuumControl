# 📚 Explicação Completa - Linha por Linha de Todos os Códigos

## 1️⃣ config.h - CONFIGURAÇÕES CENTRALIZADAS

```cpp
// ============================================================================
// CONFIGURAÇÃO - ESP32 Vacuum Control System
// ============================================================================
```
**Linhas 1-3**: Comentário de cabeçalho (visual, sem efeito no código)

```cpp
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
```
**Linhas 5-7**: 
- `#ifndef CONFIG_H` = "Se CONFIG_H NÃO foi definido"
- `#define CONFIG_H` = "Defina agora"
- Isso impede que o arquivo seja incluído 2 vezes (proteção)
- `#include <Arduino.h>` = Importa funções do ESP32 (Serial, pinMode, etc)

```cpp
// ===== PINOS ESP32 =====
#define PIN_RELAY         32  // GPIO32 - Relay controla bomba de vácuo
#define PIN_SERVO         33  // GPIO33 - PWM para servo motor (válvula)
#define PIN_I2C_SDA       21  // GPIO21 - I2C Data
#define PIN_I2C_SCL       22  // GPIO22 - I2C Clock
```
**Linhas 10-14**: Define os pinos que vamos usar
- PIN_RELAY = 32 (GPIO32) → Relay que controla a bomba
- PIN_SERVO = 33 (GPIO33) → PWM que controla o servo
- PIN_I2C_SDA = 21 (GPIO21) → Fio de dados I2C do sensor
- PIN_I2C_SCL = 22 (GPIO22) → Fio de clock I2C do sensor
- **Vantagem**: Mudar tudo em 1 lugar. Se precisar trocar, só edita aqui!

```cpp
// ===== I2C CONFIG =====
#define I2C_FREQUENCY     100000  // 100 kHz
#define SENSOR_I2C_ADDR   0x6D    // Endereço padrão XGZP6847D001MP
```
**Linhas 16-18**: Configuração da comunicação I2C
- `I2C_FREQUENCY 100000` = Velocidade de comunicação 100kHz (lento, confiável)
- `SENSOR_I2C_ADDR 0x6D` = Endereço hexadecimal do sensor (padrão de fábrica)

```cpp
// ===== PRESSÃO - SETPOINTS (em mbar) =====
#define PRESSURE_ATMOS    1013.25f  // Pressão atmosférica (~1 bar)
#define SETPOINT_STAGE1   6.0f      // Fim estágio 1
#define SETPOINT_STAGE2   0.8f      // Fim estágio 2 (vácuo máximo)
#define HYSTERESIS        0.5f      // Margem de histerese para evitar oscilações
```
**Linhas 20-24**: Define valores de pressão (alvo do sistema)
- `PRESSURE_ATMOS 1013.25f` = Pressão normal ao nível do mar
- `SETPOINT_STAGE1 6.0f` = Quando atingir 6 mbar, vai para estágio 2
- `SETPOINT_STAGE2 0.8f` = Vácuo máximo que queremos (objetivo final)
- `HYSTERESIS 0.5f` = Margem de segurança (evita ficar ligando/desligando)
- O `f` no final significa "float" (número decimal)

```cpp
// ===== SERVO CONFIG =====
#define SERVO_MIN_ANGLE   0         // Fechado (sem fluxo)
#define SERVO_MAX_ANGLE   90        // Aberto (fluxo máximo)
#define SERVO_MIN_PWM     500       // Microondas mínimo (1ms)
#define SERVO_MAX_PWM     2500      // Microondas máximo (2ms)
```
**Linhas 26-30**: Configuração do servo motor
- `SERVO_MIN_ANGLE 0` = Posição 0 graus (válvula fechada)
- `SERVO_MAX_ANGLE 90` = Posição 90 graus (válvula aberta)
- `SERVO_MIN_PWM 500` = Sinal de 500 microsegundos = 0°
- `SERVO_MAX_PWM 2500` = Sinal de 2500 microsegundos = 90°
- PWM = "Pulse Width Modulation" (varia largura do pulso)

```cpp
// ===== RELAY CONFIG =====
#define RELAY_ON          HIGH      // Relay ativo em HIGH
#define RELAY_OFF         LOW       // Relay desativo em LOW
```
**Linhas 32-34**: Estados do relay
- `RELAY_ON HIGH` = Quando relay recebe 3.3V (HIGH), liga
- `RELAY_OFF LOW` = Quando relay recebe 0V (LOW), desliga

```cpp
// ===== CONTROL PARAMETERS =====
#define PUMP_MIN_RUNTIME  1000      // Mínimo 1 segundo bomba ligada
#define PUMP_MAX_RUNTIME  300000    // Máximo 5 minutos (proteção)
#define STAGE_TIMEOUT     60000     // Timeout 60 segundos por estágio
#define SENSOR_READ_INTERVAL 100    // Lê sensor a cada 100ms
```
**Linhas 36-40**: Parâmetros de controle e proteção
- `PUMP_MIN_RUNTIME 1000` = 1 segundo (1000 ms) mínimo
- `PUMP_MAX_RUNTIME 300000` = 5 minutos máximo (proteção: se ligar mais, desliga)
- `STAGE_TIMEOUT 60000` = Se estágio levar >60s, algo errado (proteção)
- `SENSOR_READ_INTERVAL 100` = Lê sensor a cada 100 milissegundos

```cpp
// ===== SERIAL DEBUG =====
#define DEBUG_SERIAL      true
#define SERIAL_BAUD       115200

#endif // CONFIG_H
```
**Linhas 42-46**:
- `DEBUG_SERIAL true` = Mostra mensagens no serial (pode desligar depois)
- `SERIAL_BAUD 115200` = Velocidade da comunicação serial
- `#endif` = Fecha o bloco de proteção que começou com `#ifndef`

---

## 2️⃣ PressureSensor.h - INTERFACE DO SENSOR I2C

```cpp
#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
```
**Linhas 1-6**: Proteção e imports
- `Wire.h` = Biblioteca I2C nativa do ESP32
- `config.h` = Usa constantes de config.h

```cpp
class PressureSensor {
public:
    PressureSensor(uint8_t i2c_addr = SENSOR_I2C_ADDR);
```
**Linhas 8-10**:
- `class PressureSensor` = Define uma CLASSE (modelo reutilizável)
- `public:` = Funções que QUALQUER pessoa pode usar
- `PressureSensor(...)` = CONSTRUTOR (executa quando cria a classe)
- `uint8_t` = "unsigned integer 8-bit" (número 0-255)
- `= SENSOR_I2C_ADDR` = Valor padrão se não passar nada

```cpp
    // Inicializa o sensor e barramento I2C
    bool begin();
    
    // Lê valor de pressão em mbar
    float readPressure();
    
    // Lê valor bruto do sensor (debug)
    uint16_t readRaw();
    
    // Verifica se sensor está disponível
    bool isConnected();
```
**Linhas 12-21**: Funções públicas
- `bool begin()` = Retorna true/false, inicializa I2C
- `float readPressure()` = Retorna pressão em decimal (1013.25)
- `uint16_t readRaw()` = Retorna valor bruto 0-4095
- `bool isConnected()` = Verifica se sensor responde

```cpp
private:
    uint8_t _addr;
    float _lastPressure;
```
**Linhas 23-25**: Variáveis PRIVADAS (só a classe usa)
- `_addr` = Endereço I2C do sensor
- `_lastPressure` = Última leitura (para comparações)
- O `_` no começo = Convenção de "privada"

```cpp
    // Converte valor digital para pressão em mbar
    float convertRawToPressure(uint16_t raw);
};

#endif // PRESSURE_SENSOR_H
```
**Linhas 27-31**:
- `convertRawToPressure()` = Função privada (cálculo matemático)
- `};` = Fecha a classe

---

## 3️⃣ PressureSensor.cpp - IMPLEMENTAÇÃO DO SENSOR

```cpp
#include "PressureSensor.h"

PressureSensor::PressureSensor(uint8_t i2c_addr) 
    : _addr(i2c_addr), _lastPressure(0.0f) {
}
```
**Linhas 3-5**:
- `PressureSensor::` = Significa "função da classe PressureSensor"
- `: _addr(i2c_addr)` = Inicializa _addr com valor passado
- `, _lastPressure(0.0f)` = Inicializa _lastPressure com 0
- O `{}` vazio = Construtor não faz nada além de inicializar

```cpp
bool PressureSensor::begin() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_FREQUENCY);
    
    return isConnected();
}
```
**Linhas 7-12**:
- `Wire.begin(21, 22)` = Inicializa I2C no ESP32
  - GPIO 21 = Linha de dados (SDA)
  - GPIO 22 = Linha de clock (SCL)
- `Wire.setClock(100000)` = Define velocidade 100kHz
- `return isConnected()` = Testa se sensor existe

```cpp
bool PressureSensor::isConnected() {
    Wire.beginTransmission(_addr);
    return Wire.endTransmission() == 0;
}
```
**Linhas 14-17**:
- `Wire.beginTransmission(0x6D)` = Tenta falar com sensor em 0x6D
- `Wire.endTransmission()` = Retorna 0 se sucesso, outros se erro
- `== 0` = Verifica se foi sucesso (retorna true/false)

```cpp
uint16_t PressureSensor::readRaw() {
    Wire.beginTransmission(_addr);
    Wire.write(0xFE);  // Comando de leitura do XGZP6847D
    Wire.endTransmission(false);
    
    delay(5);  // Aguarda conversão
    
    Wire.requestFrom(_addr, (uint8_t)2);
    
    uint16_t raw = 0;
    if (Wire.available() >= 2) {
        uint8_t msb = Wire.read();
        uint8_t lsb = Wire.read();
        raw = (msb << 8) | lsb;
    }
    
    return raw;
}
```
**Linhas 19-35**:
- `Wire.write(0xFE)` = Envia comando para sensor ler dados
- `Wire.endTransmission(false)` = false = não desconecta (continua conversando)
- `delay(5)` = Espera sensor processar (5 milissegundos)
- `Wire.requestFrom(_addr, 2)` = Pede 2 bytes de volta
- `Wire.available()` = Verifica quantos bytes chegaram
- `msb = Wire.read()` = Lê primeiro byte (mais significativo)
- `lsb = Wire.read()` = Lê segundo byte (menos significativo)
- `(msb << 8)` = Desloca 8 posições para esquerda (multiplicação por 256)
- `| lsb` = Junta os 2 bytes (operador OR binário)
- Exemplo: msb=0x0F, lsb=0xFF → (0x0F << 8) | 0xFF = 0x0FFF = 4095

```cpp
float PressureSensor::convertRawToPressure(uint16_t raw) {
    // XGZP6847D: 
    // Output: 0-4095 (12-bit)
    // Faixa: -100 a 1000 kPa
    
    // Fórmula: Pressão (kPa) = (raw / 4095) * (1000 - (-100)) + (-100)
    //          Pressão (mbar) = Pressão (kPa) * 10
    
    float pressure_kpa = ((float)raw / 4095.0f) * 1100.0f - 100.0f;
    float pressure_mbar = pressure_kpa * 10.0f;
    
    return pressure_mbar;
}
```
**Linhas 37-48**:
- `(float)raw` = Converte para decimal (senão inteiro divide inteiro)
- `raw / 4095.0f` = Valor entre 0 e 1
- `* 1100.0f` = Multiplica pelo intervalo (-100 a 1000 = 1100)
- `- 100.0f` = Subtrai -100 (fica -100 a 1000 kPa)
- `* 10.0f` = Converte kPa para mbar (multiplica por 10)

```cpp
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
```
**Linhas 50-62**:
- `readRaw()` = Chama função para ler valor bruto
- `convertRawToPressure()` = Converte para pressão
- `if (DEBUG_SERIAL)` = Se debug ativado em config.h
- `Serial.print(..., 2)` = Imprime com 2 casas decimais
- `Serial.println()` = Imprime COM quebra de linha

---

## 4️⃣ ServoControl.h - INTERFACE DO SERVO

```cpp
#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"

class ServoControl {
public:
    ServoControl(uint8_t pin = PIN_SERVO);
    
    void begin();
    void setAngle(float angle);
    float getAngle() const;
    void open();
    void close();
    void setOpeningRatio(float ratio);
    
private:
    Servo _servo;
    uint8_t _pin;
    float _currentAngle;
};

#endif
```
**Estrutura parecida com PressureSensor**:
- `Servo _servo` = Objeto da biblioteca ESP32Servo
- `_pin` = GPIO do servo (33)
- `_currentAngle` = Ângulo atual (0-90)

---

## 5️⃣ ServoControl.cpp - IMPLEMENTAÇÃO DO SERVO

```cpp
ServoControl::ServoControl(uint8_t pin)
    : _pin(pin), _currentAngle(0.0f) {
}

void ServoControl::begin() {
    _servo.setPeriodHertz(50);
    _servo.attach(_pin, SERVO_MIN_PWM, SERVO_MAX_PWM);
    close();
}
```
**Linhas 1-8**:
- `setPeriodHertz(50)` = Frequência do PWM = 50 Hz (período 20ms)
- `attach(pin, 500, 2500)` = Conecta servo no GPIO com range PWM
- `close()` = Inicia na posição fechada (0°)

```cpp
void ServoControl::setAngle(float angle) {
    if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
    if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
    
    _currentAngle = angle;
    _servo.write((int)angle);
    
    if (DEBUG_SERIAL) {
        Serial.print("[SERVO] Ângulo: ");
        Serial.print(angle, 1);
        Serial.println("°");
    }
}
```
**Linhas 10-22**:
- Limita entre 0 e 90 graus
- `_servo.write(angle)` = Envia comando PWM para servo
- Imprime debug se ativado

```cpp
float ServoControl::getAngle() const {
    return _currentAngle;
}

void ServoControl::open() {
    setAngle(SERVO_MAX_ANGLE);  // 90°
}

void ServoControl::close() {
    setAngle(SERVO_MIN_ANGLE);  // 0°
}

void ServoControl::setOpeningRatio(float ratio) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    
    float angle = SERVO_MIN_ANGLE + ratio * (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE);
    setAngle(angle);
}
```
**Funções auxiliares**:
- `getAngle()` = Retorna ângulo atual
- `open()` / `close()` = Atalhos
- `setOpeningRatio()` = Abertura de 0% a 100%
  - Exemplo: ratio=0.5 → angle = 0 + 0.5 * 90 = 45°

---

## 6️⃣ RelayControl.h - INTERFACE DO RELAY

```cpp
class RelayControl {
public:
    RelayControl(uint8_t pin = PIN_RELAY);
    
    void begin();
    void on();
    void off();
    bool isOn() const;
    unsigned long getRuntime() const;
    
private:
    uint8_t _pin;
    bool _state;
    unsigned long _onTime;
};
```
**Simples**:
- `_pin` = GPIO 32
- `_state` = true se ligado
- `_onTime` = Quando ligou (para calcular tempo)

---

## 7️⃣ RelayControl.cpp - IMPLEMENTAÇÃO DO RELAY

```cpp
RelayControl::RelayControl(uint8_t pin)
    : _pin(pin), _state(false), _onTime(0) {
}

void RelayControl::begin() {
    pinMode(_pin, OUTPUT);
    off();
}
```
**Linhas 1-7**:
- `pinMode(32, OUTPUT)` = GPIO 32 como saída
- Inicia desligado

```cpp
void RelayControl::on() {
    if (!_state) {
        digitalWrite(_pin, RELAY_ON);
        _state = true;
        _onTime = millis();
        
        if (DEBUG_SERIAL) {
            Serial.println("[RELAY] Bomba LIGADA");
        }
    }
}
```
**Linhas 9-18**:
- `if (!_state)` = Se NÃO está ligado
- `digitalWrite(32, HIGH)` = Manda 3.3V para GPIO 32
- `_onTime = millis()` = Guarda tempo atual (em milissegundos desde boot)

```cpp
void RelayControl::off() {
    if (_state) {
        digitalWrite(_pin, RELAY_OFF);
        _state = false;
        
        unsigned long runtime = millis() - _onTime;
        if (DEBUG_SERIAL) {
            Serial.print("[RELAY] Bomba DESLIGADA (tempo: ");
            Serial.print(runtime);
            Serial.println("ms)");
        }
    }
}
```
**Linhas 20-31**:
- `digitalWrite(32, LOW)` = Manda 0V para GPIO 32
- `runtime = millis() - _onTime` = Calcula tempo que ficou ligado

```cpp
bool RelayControl::isOn() const {
    return _state;
}

unsigned long RelayControl::getRuntime() const {
    if (!_state) return 0;
    return millis() - _onTime;
}
```
**Funções auxiliares**:
- `isOn()` = Retorna estado
- `getRuntime()` = Retorna tempo ligado (0 se desligado)

---

## 8️⃣ VacuumController.h - MÁQUINA DE ESTADOS

```cpp
enum ControlState {
    STATE_IDLE,
    STATE_STAGE1,
    STATE_STAGE2,
    STATE_HOLDING,
    STATE_ERROR
};

class VacuumController {
public:
    VacuumController(PressureSensor* sensor, 
                     ServoControl* servo, 
                     RelayControl* relay);
```
**Estados e construtor**:
- Enum = Lista de constantes (IDLE=0, STAGE1=1, etc)
- Construtor recebe **ponteiros** para os 3 componentes
- `*` = Ponteiro (referência ao objeto, não cópia)

```cpp
    void begin();
    void update();
    void startVacuum();
    void stopVacuum();
    
    ControlState getState() const;
    float getCurrentPressure() const;
    float getServoPosition() const;
    bool isPumpOn() const;
    String getStateMessage() const;
    
private:
    PressureSensor* _sensor;
    ServoControl* _servo;
    RelayControl* _relay;
    
    ControlState _currentState;
    ControlState _nextState;
    float _currentPressure;
    unsigned long _stateStartTime;
    
    void transitionTo(ControlState newState);
    void handleIdle();
    void handleStage1();
    void handleStage2();
    void handleHolding();
    void handleError();
    
    float calculateServoOpening(float pressure);
    void checkSafeties();
};
```
**Estrutura completa**:
- Ponteiros aos componentes
- Estado atual + próximo estado
- Pressão lida
- Tempo que começou estado
- Função para cada estado

---

## 9️⃣ VacuumController.cpp - IMPLEMENTAÇÃO

```cpp
VacuumController::VacuumController(PressureSensor* sensor, 
                                   ServoControl* servo, 
                                   RelayControl* relay)
    : _sensor(sensor), _servo(servo), _relay(relay),
      _currentState(STATE_IDLE), _nextState(STATE_IDLE),
      _currentPressure(PRESSURE_ATMOS), _stateStartTime(0) {
}

void VacuumController::begin() {
    _currentState = STATE_IDLE;
    _stateStartTime = millis();
    Serial.println("[VACUUM] Sistema inicializado em IDLE");
}

void VacuumController::update() {
    _currentPressure = _sensor->readPressure();
    checkSafeties();
    
    switch (_currentState) {
        case STATE_IDLE:
            handleIdle();
            break;
        case STATE_STAGE1:
            handleStage1();
            break;
        case STATE_STAGE2:
            handleStage2();
            break;
        case STATE_HOLDING:
            handleHolding();
            break;
        case STATE_ERROR:
            handleError();
            break;
    }
}
```
**Início e loop**:
- `update()` = Chamada a cada 100ms
- `switch` = Executa função do estado atual

```cpp
void VacuumController::startVacuum() {
    if (_currentState == STATE_IDLE) {
        transitionTo(STATE_STAGE1);
    }
}

void VacuumController::stopVacuum() {
    _relay->off();
    _servo->close();
    transitionTo(STATE_IDLE);
}

void VacuumController::transitionTo(ControlState newState) {
    if (_currentState != newState) {
        _currentState = newState;
        _stateStartTime = millis();
        
        if (DEBUG_SERIAL) {
            Serial.print("[STATE] Transição para: ");
            Serial.println(getStateMessage());
        }
    }
}
```
**Comandos e transições**:
- `startVacuum()` = Só começa se está em IDLE
- `stopVacuum()` = Desliga tudo
- `transitionTo()` = Muda estado e registra tempo

```cpp
void VacuumController::handleIdle() {
    _relay->off();
    _servo->close();
}

void VacuumController::handleStage1() {
    _relay->on();
    float opening = calculateServoOpening(_currentPressure);
    _servo->setOpeningRatio(opening);
    
    if (_currentPressure <= (SETPOINT_STAGE1 + HYSTERESIS)) {
        transitionTo(STATE_STAGE2);
    }
    
    if ((millis() - _stateStartTime) > STAGE_TIMEOUT) {
        if (DEBUG_SERIAL) {
            Serial.println("[ERROR] Timeout no STAGE1!");
        }
        transitionTo(STATE_ERROR);
    }
}
```
**Lógica STAGE1**:
- Liga bomba
- Abre válvula gradualmente
- Se P < 6 mbar, vai para STAGE2
- Se levar >60s, erro

```cpp
void VacuumController::handleStage2() {
    _relay->on();
    float opening = calculateServoOpening(_currentPressure);
    _servo->setOpeningRatio(opening);
    
    if (_currentPressure <= (SETPOINT_STAGE2 + HYSTERESIS)) {
        transitionTo(STATE_HOLDING);
    }
    
    if ((millis() - _stateStartTime) > STAGE_TIMEOUT) {
        transitionTo(STATE_ERROR);
    }
}
```
**Lógica STAGE2**:
- Liga bomba
- Abre válvula mais controlada
- Se P < 0.8 mbar, vai para HOLDING
- Proteção timeout

```cpp
void VacuumController::handleHolding() {
    if (_currentPressure > (SETPOINT_STAGE2 + HYSTERESIS * 2)) {
        _relay->on();
        _servo->setOpeningRatio(0.3f);
    }
    else if (_currentPressure > SETPOINT_STAGE2) {
        _relay->on();
        _servo->setOpeningRatio(0.1f);
    }
    else {
        _relay->off();
        _servo->setOpeningRatio(0.0f);
    }
}
```
**Lógica HOLDING**:
- Se P subiu muito (>0.8+1.0), abre 30%
- Se P subiu pouco (>0.8), abre 10%
- Se está bom, desliga

```cpp
void VacuumController::handleError() {
    _relay->off();
    _servo->close();
}

float VacuumController::calculateServoOpening(float pressure) {
    float opening = 0.0f;
    
    if (_currentState == STATE_STAGE1) {
        float ratio = (PRESSURE_ATMOS - pressure) / (PRESSURE_ATMOS - SETPOINT_STAGE1);
        opening = ratio * 0.9f;
    }
    else if (_currentState == STATE_STAGE2) {
        float ratio = (SETPOINT_STAGE1 - pressure) / (SETPOINT_STAGE1 - SETPOINT_STAGE2);
        opening = 0.3f + ratio * 0.5f;
    }
    
    if (opening < 0.0f) opening = 0.0f;
    if (opening > 1.0f) opening = 1.0f;
    
    return opening;
}
```
**Cálculo de abertura**:
- STAGE1: Proporção de 1013 até 6 mbar
  - Começou: P=1013 → ratio=0 → abertura=0%
  - Meio: P=500 → ratio=0.5 → abertura=45%
  - Fim: P=6 → ratio=1 → abertura=90%
- STAGE2: 30% a 80% (mais controlado)

```cpp
void VacuumController::checkSafeties() {
    if (_relay->isOn() && (_relay->getRuntime() > PUMP_MAX_RUNTIME)) {
        transitionTo(STATE_ERROR);
    }
    
    if (_currentState != STATE_IDLE && _currentPressure > PRESSURE_ATMOS + 50) {
        transitionTo(STATE_ERROR);
    }
}
```
**Proteções**:
- Se bomba >5min, erro
- Se P >1063 mbar em vácuo, erro

---

## 🔟 main.cpp - PROGRAMA PRINCIPAL

```cpp
#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "PressureSensor.h"
#include "ServoControl.h"
#include "RelayControl.h"
#include "VacuumController.h"

PressureSensor pressureSensor;
ServoControl servo;
RelayControl relay;
VacuumController vacuumController(&pressureSensor, &servo, &relay);

unsigned long lastSensorRead = 0;
unsigned long lastStatusPrint = 0;
```
**Setup global**:
- Importa todas as classes
- Cria objetos globais (usáveis de qualquer lugar)
- Passa ponteiros ao controlador

```cpp
void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(500);
    
    Serial.println("\n========================================");
    Serial.println("ESP32 VACUUM CONTROL SYSTEM");
    Serial.println("2-Stage Vacuum Control");
    Serial.println("========================================\n");
    
    Serial.print("Inicializando sensor de pressão...");
    if (pressureSensor.begin()) {
        Serial.println(" OK");
    } else {
        Serial.println(" ERRO!");
        while(1);  // Trava se sensor não encontrado
    }
    
    Serial.print("Inicializando servo motor...");
    servo.begin();
    Serial.println(" OK");
    
    Serial.print("Inicializando relay...");
    relay.begin();
    Serial.println(" OK");
    
    Serial.print("Inicializando controlador de vácuo...");
    vacuumController.begin();
    Serial.println(" OK");
    
    Serial.println("\nSistema pronto!");
    Serial.println("Digite 'START' para iniciar vácuo, 'STOP' para parar");
    Serial.println("========================================\n");
}
```
**setup()**:
- Executa 1 vez ao ligar
- Inicializa serial (115200 baud)
- Testa cada componente
- Se sensor falhar, `while(1)` trava infinito (proteção)

```cpp
void loop() {
    if ((millis() - lastSensorRead) >= SENSOR_READ_INTERVAL) {
        vacuumController.update();
        lastSensorRead = millis();
    }
    
    if ((millis() - lastStatusPrint) >= 2000) {
        printStatus();
        lastStatusPrint = millis();
    }
    
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toUpperCase();
        
        processCommand(command);
    }
    
    delay(10);
}
```
**loop()**:
- Roda infinitamente
- A cada 100ms: Atualiza lógica (lê sensor, processa estado)
- A cada 2s: Mostra status
- Se digitar: Processa comando
- `delay(10)` = Aguarda 10ms (evita CPU 100%)

```cpp
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
```
**printStatus()**:
- Imprime todas as informações
- `* 100` = Converte 0-1 para 0-100%
- `? "ON" : "OFF"` = Se verdadeiro: "ON", senão: "OFF"

```cpp
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
```
**Comandos**:
- START = Chama `startVacuum()`
- STOP = Chama `stopVacuum()`
- STATUS = Mostra tudo
- HELP = Lista comandos
- Ignora comandos desconhecidos

---

## 1️⃣1️⃣ platformio.ini - CONFIGURAÇÃO DA COMPILAÇÃO

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
upload_speed = 921600
lib_deps = 
    ESP32Servo@3.1.2

[platformio]
default_envs = esp32doit-devkit-v1
```
**Configurações**:
- `platform = espressif32` = Plataforma ESP32
- `board = esp32doit-devkit-v1` = Board específica
- `framework = arduino` = Usa Arduino como base
- `monitor_speed = 115200` = Serial monitor
- `upload_speed = 921600` = Velocidade upload (rápido)
- `lib_deps = ESP32Servo@3.1.2` = Instala servo library versão 3.1.2

---

## 📚 Testes (test_*.cpp)

Todos seguem padrão similar:
1. `setup()` = Inicializa
2. `loop()` = Testa componente
3. Mostra resultado

Exemplo test_relay.cpp:
- Liga relay 2s
- Desliga 2s
- Ciclo rápido 10x
- Repouso 5s

---

## 📖 Resumo Geral

```
CONFIG.H
  └─ Define CONSTANTES (pinos, valores, etc)
      └─ Usado por TODOS os arquivos

SENSOR
  ├─ Header: Interface (o que faz)
  └─ CPP: Implementação (como faz)
      └─ Lê I2C, converte valor em pressão

SERVO
  ├─ Header: Interface
  └─ CPP: Implementação
      └─ Controla PWM (0-90 graus)

RELAY
  ├─ Header: Interface
  └─ CPP: Implementação
      └─ Liga/desliga GPIO

VACUUM CONTROLLER
  ├─ Header: Interface + Estados
  └─ CPP: Implementação
      └─ State machine que coordena TUDO

MAIN.CPP
  └─ Loop principal
      ├─ Inicializa setup()
      ├─ Roda loop() 100x/seg
      ├─ Mostra status 2x/seg
      └─ Processa comandos serial
```

---

**Agora você entende TUDO! 🎓**

Quer que explique alguma função específica em mais detalhe?
