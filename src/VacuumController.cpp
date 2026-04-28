// ============================================================================
// VACUUM CONTROLLER IMPLEMENTATION
// ============================================================================

#include <Arduino.h>
#include "VacuumController.h"

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
    // Lê pressão do sensor
    _currentPressure = _sensor->readPressure();
    
    // Verifica proteções
    checkSafeties();
    
    // Executa lógica do estado atual
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

void VacuumController::handleIdle() {
    // Sistema em repouso
    _relay->off();
    _servo->close();
    
    // Aguarda comando para iniciar
    // (será iniciado por startVacuum())
}

void VacuumController::handleStage1() {
    // Estágio 1: 1.013 bar → 6 mbar
    // Abertura rápida da válvula, bomba em velocidade máxima
    
    _relay->on();  // Liga bomba
    
    // Calcula abertura progressiva
    float opening = calculateServoOpening(_currentPressure);
    _servo->setOpeningRatio(opening);
    
    // Verifica se chegou ao setpoint do estágio 1
    if (_currentPressure <= (SETPOINT_STAGE1 + HYSTERESIS)) {
        transitionTo(STATE_STAGE2);
    }
    
    // Proteção: timeout no estágio
    if ((millis() - _stateStartTime) > STAGE_TIMEOUT) {
        if (DEBUG_SERIAL) {
            Serial.println("[ERROR] Timeout no STAGE1!");
        }
        transitionTo(STATE_ERROR);
    }
}

void VacuumController::handleStage2() {
    // Estágio 2: 6 mbar → 0.8 mbar
    // Abertura mais controlada para precisão
    
    _relay->on();  // Bomba pode funcionar em modo modulado
    
    // Abertura mais suave
    float opening = calculateServoOpening(_currentPressure);
    _servo->setOpeningRatio(opening);
    
    // Verifica se chegou ao setpoint final
    if (_currentPressure <= (SETPOINT_STAGE2 + HYSTERESIS)) {
        transitionTo(STATE_HOLDING);
    }
    
    // Proteção: timeout
    if ((millis() - _stateStartTime) > STAGE_TIMEOUT) {
        if (DEBUG_SERIAL) {
            Serial.println("[ERROR] Timeout no STAGE2!");
        }
        transitionTo(STATE_ERROR);
    }
}

void VacuumController::handleHolding() {
    // Mantém vácuo no setpoint
    // Bomba modulada para compensar vazamentos
    
    // Se pressão subiu, abre válvula um pouco
    if (_currentPressure > (SETPOINT_STAGE2 + HYSTERESIS * 2)) {
        _relay->on();
        _servo->setOpeningRatio(0.3f);  // Pequena abertura
    }
    // Se pressão está próxima do setpoint, apenas mantem bomba
    else if (_currentPressure > SETPOINT_STAGE2) {
        _relay->on();
        _servo->setOpeningRatio(0.1f);  // Mínima abertura
    }
    // Se está bom, pode desligar bomba temporariamente
    else {
        _relay->off();
        _servo->setOpeningRatio(0.0f);
    }
}

void VacuumController::handleError() {
    // Proteção: desliga tudo
    _relay->off();
    _servo->close();
    
    if (DEBUG_SERIAL) {
        Serial.println("[ERROR] Sistema em proteção!");
    }
}

float VacuumController::calculateServoOpening(float pressure) {
    // Calcula abertura da válvula proporcional à pressão
    // Lógica: quanto maior a pressão, maior a abertura (mais vazão)
    
    float opening = 0.0f;
    
    if (_currentState == STATE_STAGE1) {
        // Estágio 1: variação de 1013 mbar até 6 mbar
        // Abertura rápida e proporcional
        float ratio = (PRESSURE_ATMOS - pressure) / (PRESSURE_ATMOS - SETPOINT_STAGE1);
        opening = ratio * 0.9f;  // Máximo 90% de abertura
    }
    else if (_currentState == STATE_STAGE2) {
        // Estágio 2: variação de 6 mbar até 0.8 mbar
        // Abertura mais suave para precisão
        float ratio = (SETPOINT_STAGE1 - pressure) / (SETPOINT_STAGE1 - SETPOINT_STAGE2);
        opening = 0.3f + ratio * 0.5f;  // 30% a 80% de abertura
    }
    
    // Limita abertura
    if (opening < 0.0f) opening = 0.0f;
    if (opening > 1.0f) opening = 1.0f;
    
    return opening;
}

void VacuumController::checkSafeties() {
    // Proteção 1: Bomba ligada por muito tempo
    if (_relay->isOn() && (_relay->getRuntime() > PUMP_MAX_RUNTIME)) {
        if (DEBUG_SERIAL) {
            Serial.println("[SAFETY] Proteção: Bomba ligada > 5 min!");
        }
        transitionTo(STATE_ERROR);
    }
    
    // Proteção 2: Pressão anormalmente alta em vácuo
    if (_currentState != STATE_IDLE && _currentPressure > PRESSURE_ATMOS + 50) {
        if (DEBUG_SERIAL) {
            Serial.println("[SAFETY] Proteção: Pressão > atmosférica!");
        }
        transitionTo(STATE_ERROR);
    }
}

ControlState VacuumController::getState() const {
    return _currentState;
}

float VacuumController::getCurrentPressure() const {
    return _currentPressure;
}

float VacuumController::getServoPosition() const {
    return _servo->getAngle() / SERVO_MAX_ANGLE;
}

bool VacuumController::isPumpOn() const {
    return _relay->isOn();
}

String VacuumController::getStateMessage() const {
    switch (_currentState) {
        case STATE_IDLE:
            return "IDLE";
        case STATE_STAGE1:
            return "STAGE1 (1013→6 mbar)";
        case STATE_STAGE2:
            return "STAGE2 (6→0.8 mbar)";
        case STATE_HOLDING:
            return "HOLDING (mantendo vácuo)";
        case STATE_ERROR:
            return "ERROR (proteção ativa)";
        default:
            return "UNKNOWN";
    }
}
