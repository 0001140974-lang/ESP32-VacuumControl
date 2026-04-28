// ============================================================================
// SERVO MOTOR IMPLEMENTATION
// ============================================================================

#include <Arduino.h>
#include "ServoControl.h"

ServoControl::ServoControl(uint8_t pin)
    : _pin(pin), _currentAngle(0.0f) {
}

void ServoControl::begin() {
    _servo.setPeriodHertz(50);  // Frequência padrão servo (50 Hz = 20ms)
    _servo.attach(_pin, SERVO_MIN_PWM, SERVO_MAX_PWM);
    close();  // Inicia fechado
}

void ServoControl::setAngle(float angle) {
    // Limita ângulo entre 0 e 90 graus
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

float ServoControl::getAngle() const {
    return _currentAngle;
}

void ServoControl::open() {
    setAngle(SERVO_MAX_ANGLE);
}

void ServoControl::close() {
    setAngle(SERVO_MIN_ANGLE);
}

void ServoControl::setOpeningRatio(float ratio) {
    // ratio: 0.0 (fechado) a 1.0 (aberto)
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    
    float angle = SERVO_MIN_ANGLE + ratio * (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE);
    setAngle(angle);
}
