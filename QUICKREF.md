# Quick Reference - ESP32 Vacuum Control

## Compilar e Fazer Upload

### PlatformIO (Recomendado)
```bash
cd c:\Users\gabri\OneDrive\Imagens\ESP32-VacuumControl

# Build
pio run

# Upload
pio run --target upload

# Monitor
pio device monitor --baud 115200
```

### Arduino IDE
1. Abrir `src/main.cpp`
2. Tools → Board → ESP32 DOIT DevKit v1
3. Sketch → Upload (Ctrl+U)
4. Tools → Serial Monitor (115200)

## Comandos Serial Essenciais

| Comando | Função | Esperado |
|---------|--------|----------|
| `START` | Inicia vácuo | Pressão cai 1013 → 0.8 mbar |
| `STOP` | Para vácuo | Pressão sobe para 1013 mbar |
| `STATUS` | Mostra estado | Imprime dados instantâneos |
| `HELP` | Lista comandos | Mostra ajuda |

## Pinagem Quick Reference

```
ESP32 PIN    COMPONENTE           TIPO
GPIO 21      Sensor I2C (SDA)     I2C Data
GPIO 22      Sensor I2C (SCL)     I2C Clock
GPIO 33      Servo Motor          PWM (3.3V)
GPIO 32      Relay (Bomba)        Digital GPIO
```

**Alimentação:**
- Sensor: 3.3V (via ESP32)
- Servo: **5V (fonte externa)**
- Relay: 5V (fonte externa)
- ESP32: USB ou 5V → 3.3V

## Estrutura de Arquivos

```
ESP32-VacuumControl/
├── src/
│   ├── main.cpp                  # Loop principal
│   ├── config.h                  # Configurações
│   ├── PressureSensor.{h,cpp}    # Driver sensor I2C
│   ├── ServoControl.{h,cpp}      # Controle servo
│   ├── RelayControl.{h,cpp}      # Controle relay
│   └── VacuumController.{h,cpp}  # State machine
├── README.md                      # Documentação geral
├── WIRING.md                      # Esquema de conexão
├── TESTING.md                     # Testes e troubleshooting
├── QUICKREF.md                    # Este arquivo
├── platformio.ini                # Configuração PlatformIO
└── .gitignore
```

## Estados da Máquina

```
IDLE (parado)
  ↓
START command
  ↓
STAGE1: 1013 mbar → 6 mbar
  ↓ (P < 6 mbar)
STAGE2: 6 mbar → 0.8 mbar
  ↓ (P < 0.8 mbar)
HOLDING: Mantém vácuo (pump ON/OFF conforme necessário)
  ↓
STOP command
  ↓
IDLE
```

## Pontos de Ajuste em config.h

```cpp
// PRESSÃO - Mudar setpoints
#define SETPOINT_STAGE1   6.0f       // Fim estágio 1
#define SETPOINT_STAGE2   0.8f       // Fim estágio 2
#define HYSTERESIS        0.5f       // Oscilação permitida

// SERVO - Mudar range
#define SERVO_MIN_ANGLE   0          // Fechado
#define SERVO_MAX_ANGLE   90         // Aberto
#define SERVO_MIN_PWM     500        // 1ms
#define SERVO_MAX_PWM     2500       // 2ms

// PROTEÇÃO - Timeouts
#define STAGE_TIMEOUT     60000      // 60s por estágio
#define PUMP_MAX_RUNTIME  300000     // 5min bomba
```

## Fórmulas Importantes

### Conversão Sensor → Pressão
```
raw_value: 0-4095 (12-bit)
pressão(mbar) = (raw / 4095) * 1100 - 1000
```

### Servo Angle → PWM
```
frequência: 50 Hz (20ms período)
0°:   500 µs (1.0ms / 20ms = 5%)
90°: 2500 µs (2.5ms / 20ms = 12.5%)
```

## Verificação Rápida

**Sensor funcionando?**
```
Serial: [SENSOR] Raw: XXXX | Pressão: YYYY.YY mbar
```

**Servo funcionando?**
```
Comando: START
Ver: [SERVO] Ângulo: X.X°
```

**Relay funcionando?**
```
Comando: START
Ouvir: "CLICK" no relay
Serial: [RELAY] Bomba LIGADA
```

**Ciclo funcionando?**
```
Comando: START
Ver:
  - Pressão cai (1013 → 6 → 0.8 mbar)
  - Estados mudam (STAGE1 → STAGE2 → HOLDING)
  - Servo abre/fecha apropriadamente
```

## Variáveis Importantes

```cpp
// VacuumController
getState()              // Retorna estado atual
getCurrentPressure()    // Pressão em mbar
getServoPosition()      // 0.0 (fechado) a 1.0 (aberto)
isPumpOn()             // true se bomba está ligada
getStateMessage()      // String do estado atual
```

## Debug Avançado

### Ver estados na serial (já habilitado)
```
[SENSOR] Raw: 3580 | Pressão: 1012.50 mbar
[STATE] Transição para: STAGE1 (1013→6 mbar)
[SERVO] Ângulo: 75.0°
[RELAY] Bomba LIGADA
[SAFETY] Proteção: Bomba ligada > 5 min!
```

### Adicionar breakpoint manual
```cpp
if (vacuumController.getCurrentPressure() < 5.0f) {
    Serial.println("🔴 PRESSÃO CRÍTICA!");
    delay(100);
}
```

## Próxima Fase

**Em desenvolvimento:**
1. Implementar PID controller (precisão ±0.1 mbar)
2. Adicionar sensor de vazamento
3. Logging em SD card
4. Integração com comunicação Modbus
5. Display OLED para monitoramento local

---

**Imprimível?** Sim! Use Ctrl+P no navegador
**Atualizado**: 23/04/2026
**Status**: Prototipagem - FUNCIONAL ✅
