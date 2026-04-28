# ESP32 Vacuum Control System

Sistema de controle de vácuo em 2 estágios para ESP32 DOIT DevKit, com sensor de pressão I2C e servos para controle proporcional de válvulas.

## Características

- **Controlador**: ESP32 DOIT DevKit
- **Linguagem**: C++
- **Sensor de Pressão**: XGZP6847D001MP (I2C, -100 a 1000 kPa)
- **Bomba de Vácuo**: Controlada via relay (GPIO)
- **Válvulas**: Servo motor MG995 (controle proporcional)
- **Estágios**: 2 (1013 mbar → 6 mbar → 0.8 mbar)

## Arquitetura do Projeto

```
src/
├── main.cpp              # Programa principal, loop e interface serial
├── config.h              # Configurações centralizadas
├── PressureSensor.h/.cpp # Driver do sensor I2C
├── ServoControl.h/.cpp   # Controle de servo motor
├── RelayControl.h/.cpp   # Controle do relay da bomba
└── VacuumController.h/.cpp # State machine principal (2 estágios)
```

## Pinagem (ESP32)

| Componente | Pino ESP32 | GPIO | Função |
|-----------|-----------|------|--------|
| I2C SDA   | GPIO 21   | 21   | Sensor pressão |
| I2C SCL   | GPIO 22   | 22   | Sensor pressão |
| Servo     | GPIO 33   | 33   | PWM (válvula proporcional) |
| Relay     | GPIO 32   | 32   | Controle bomba |

## Conexões Hardware

### Sensor de Pressão (XGZP6847D001MP)
- **VCC**: 3.3V (ESP32)
- **GND**: GND (ESP32)
- **SDA**: GPIO 21
- **SCL**: GPIO 22

### Servo Motor (MG995)
- **Vermelho (VCC)**: 5V
- **Preto (GND)**: GND
- **Amarelo (Signal)**: GPIO 33

### Relay
- **GND**: GND (ESP32)
- **Signal**: GPIO 32
- **VCC**: 5V (com proteção)

## Estágios de Vácuo

### Estágio 1: Pré-Vácuo (1013 → 6 mbar)
- Válvula aberta progressivamente (0% → 90%)
- Bomba em velocidade máxima
- Tempo típico: 10-30 segundos

### Estágio 2: Vácuo Fino (6 → 0.8 mbar)
- Válvula com abertura controlada (30% → 80%)
- Bomba pode ser modulada
- Tempo típico: 30-60 segundos

### Estágio 3: Retenção (Holding)
- Mantém vácuo em 0.8 mbar
- Bomba liga/desliga conforme necessário
- Compensa pequenos vazamentos

## Protocolo Serial (Debug/Controle)

**Baud Rate**: 115200

### Comandos
```
START   - Inicia ciclo de vácuo
STOP    - Para e retorna à pressão atmosférica
STATUS  - Mostra status atual
HELP    - Lista de comandos
```

### Saída (Status a cada 2s)
```
[STATUS] STAGE1 (1013→6 mbar) | P=500.2mbar | Servo=75% | Bomba=ON
```

## Protocolos de Segurança

1. **Timeout por Estágio**: 60 segundos máximo
2. **Tempo Máximo de Bomba**: 5 minutos
3. **Detecção de Pressão Anormal**: > 1063 mbar em vácuo
4. **Auto-Stop em Erro**: Desliga tudo e entra em STATE_ERROR

## Calibração

### Sensor de Pressão
- Faixa: -100 a 1000 kPa
- Resolução: 12-bit (4095 níveis)
- Conversão: `P(mbar) = (raw/4095) * 1100 - 100 * 10`

### Servo Motor
- PWM: 500-2500 µs
- Frequência: 50 Hz
- Ângulo: 0° (fechado) a 90° (aberto)

## Build e Upload

### Usando PlatformIO
```bash
# Build
platformio run

# Upload
platformio run --target upload

# Monitor serial
platformio device monitor --baud 115200
```

### Usando Arduino IDE
1. Instalar ESP32 board package
2. Selecionar: Tools → Board → ESP32 DOIT DevKit
3. Instalar biblioteca: Sketch → Include Library → Manage → "ESP32Servo"
4. Fazer upload

## Modificações para Produção

### Próximos passos
1. **Substituir Servo por Válvula Proporcional**: Adaptar saída PWM
2. **Implementar PID**: Para controle mais preciso
3. **Logging em EEPROM**: Histórico de ciclos
4. **Comunicação Modbus/Ethernet**: Para integração com SCADA
5. **Sensor de Vazamento**: Detectar vazios antes de iniciar

## Troubleshooting

### Sensor não detectado
- Verificar conexões I2C (SDA/SCL)
- Testar endereço I2C com `Wire.scan()`
- Checar alimentação (3.3V)

### Servo não move
- Verificar PWM no pino 33
- Verificar alimentação 5V do servo
- Testar servo separadamente

### Pressão não diminui
- Verificar se bomba está ligando (GPIO 32)
- Checar válvula do servo
- Verificar vazamentos no sistema

## Referências

- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [XGZP6847D Datasheet](https://www.xsensor.cn/)
- [MG995 Servo Specs](https://www.electronicoscope.com/servo-mg995)

---

**Autor**: Gabriel  
**Data**: 23/04/2026  
**Status**: Protótipo - Fase 1
