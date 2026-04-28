// ============================================================================
// CONFIGURAÇÃO - ESP32 Vacuum Control System
// ============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ===== PINOS ESP32 =====
#define PIN_RELAY         32  // GPIO32 - Relay controla bomba de vácuo
#define PIN_SERVO         33  // GPIO33 - PWM para servo motor (válvula)
#define PIN_I2C_SDA       21  // GPIO21 - I2C Data
#define PIN_I2C_SCL       22  // GPIO22 - I2C Clock

// ===== I2C CONFIG =====
#define I2C_FREQUENCY     100000  // 100 kHz
#define SENSOR_I2C_ADDR   0x58    // Endereço I2C do XGZP6847D (datasheet v3.0)

// ===== PRESSÃO - SETPOINTS (em mbar) =====
#define PRESSURE_ATMOS    1013.25f  // Pressão atmosférica (~1 bar)
#define SETPOINT_STAGE1   6.0f      // Fim estágio 1
#define SETPOINT_STAGE2   0.8f      // Fim estágio 2 (vácuo máximo)
#define HYSTERESIS        0.5f      // Margem de histerese para evitar oscilações

// ===== SERVO CONFIG =====
#define SERVO_MIN_ANGLE   0         // Fechado (sem fluxo)
#define SERVO_MAX_ANGLE   90        // Aberto (fluxo máximo)
#define SERVO_MIN_PWM     500       // Microondas mínimo (1ms)
#define SERVO_MAX_PWM     2500      // Microondas máximo (2ms)

// ===== RELAY CONFIG =====
#define RELAY_ON          HIGH      // Relay ativo em HIGH
#define RELAY_OFF         LOW       // Relay desativo em LOW

// ===== CONTROL PARAMETERS =====
#define PUMP_MIN_RUNTIME  1000      // Mínimo 1 segundo bomba ligada
#define PUMP_MAX_RUNTIME  300000    // Máximo 5 minutos (proteção)
#define STAGE_TIMEOUT     60000     // Timeout 60 segundos por estágio
#define SENSOR_READ_INTERVAL 100    // Lê sensor a cada 100ms

// ===== SERIAL DEBUG =====
#define DEBUG_SERIAL      true
#define SERIAL_BAUD       115200

#endif // CONFIG_H
