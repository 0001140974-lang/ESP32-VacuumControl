// ============================================================================
// SERVO MOTOR CONTROL - MG995
// ============================================================================

#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"

class ServoControl {
public:
    ServoControl(uint8_t pin = PIN_SERVO);
    
    // Inicializa servo
    void begin();
    
    // Define ângulo (0-90 graus)
    void setAngle(float angle);
    
    // Retorna ângulo atual
    float getAngle() const;
    
    // Abre válvula completamente (90 graus)
    void open();
    
    // Fecha válvula completamente (0 graus)
    void close();
    
    // Define abertura proporcional (0.0 = fechado, 1.0 = aberto)
    void setOpeningRatio(float ratio);
    
private:
    Servo _servo;
    uint8_t _pin;
    float _currentAngle;
};

#endif // SERVO_CONTROL_H
