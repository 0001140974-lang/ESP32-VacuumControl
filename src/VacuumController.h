// ============================================================================
// VACUUM CONTROLLER - State Machine (2 Estágios)
// ============================================================================

#ifndef VACUUM_CONTROLLER_H
#define VACUUM_CONTROLLER_H

#include "PressureSensor.h"
#include "ServoControl.h"
#include "RelayControl.h"
#include "config.h"

// Estados da máquina de estados
enum ControlState {
    STATE_IDLE,       // Parado (pressão = atmosférica)
    STATE_STAGE1,     // Estágio 1: 1.013 bar → 6 mbar
    STATE_STAGE2,     // Estágio 2: 6 mbar → 0.8 mbar
    STATE_HOLDING,    // Mantendo vácuo
    STATE_ERROR       // Erro/proteção
};

class VacuumController {
public:
    VacuumController(PressureSensor* sensor, 
                     ServoControl* servo, 
                     RelayControl* relay);
    
    // Inicializa o controlador
    void begin();
    
    // Atualiza o estado da máquina (chamado periodicamente)
    void update();
    
    // Inicia ciclo de vácuo
    void startVacuum();
    
    // Para ciclo de vácuo (retorna à pressão atmosférica)
    void stopVacuum();
    
    // Retorna estado atual
    ControlState getState() const;
    
    // Retorna pressão atual
    float getCurrentPressure() const;
    
    // Retorna posição atual do servo (0-1)
    float getServoPosition() const;
    
    // Retorna se bomba está ligada
    bool isPumpOn() const;
    
    // Retorna mensagem de estado para debug
    String getStateMessage() const;
    
private:
    PressureSensor* _sensor;
    ServoControl* _servo;
    RelayControl* _relay;
    
    ControlState _currentState;
    ControlState _nextState;
    float _currentPressure;
    unsigned long _stateStartTime;
    
    // Funções de transição de estado
    void transitionTo(ControlState newState);
    
    // Lógica de cada estado
    void handleIdle();
    void handleStage1();
    void handleStage2();
    void handleHolding();
    void handleError();
    
    // Calcula abertura da válvula baseado em pressão e estágio
    float calculateServoOpening(float pressure);
    
    // Verifica proteções
    void checkSafeties();
};

#endif // VACUUM_CONTROLLER_H
