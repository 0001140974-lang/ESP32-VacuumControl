# Esquema de Conexão - ESP32 Vacuum Control

## Diagrama Geral do Sistema

```
┌─────────────────────────────────────────────────────────────┐
│                         ESP32 DOIT                           │
│                      (3.3V / 5V tolerant)                   │
└─────────────────────────────────────────────────────────────┘
         ↓                    ↓                    ↓
    ┌────────┐          ┌──────────┐        ┌──────────┐
    │ Sensor │          │  Servo   │        │  Relay   │
    │ I2C    │          │ MG995    │        │ (Bomba)  │
    │        │          │          │        │          │
    └────────┘          └──────────┘        └──────────┘
```

## Pinagem Detalhada

### 1. Sensor de Pressão I2C (XGZP6847D001MP)

```
ESP32 DOIT          SENSOR XGZP6847D001MP
─────────           ──────────────────────
GPIO 21 (SDA) ───── SDA (Pino 4)
GPIO 22 (SCL) ───── SCL (Pino 3)
3.3V ──────────────  VCC (Pino 1)
GND ───────────────  GND (Pino 2)

Resistores Pull-up (4.7kΩ a 10kΩ):
SDA ─ 4.7kΩ ─ 3.3V
SCL ─ 4.7kΩ ─ 3.3V
```

**Endereço I2C**: 0x6D (padrão do XGZP6847D)

### 2. Servo Motor MG995

```
ESP32 DOIT          SERVO MG995
─────────           ──────────
GPIO 33 (PWM) ───── Signal (Amarelo)
5V ────────────────  VCC (Vermelho) ⚠ IMPORTANTE: 5V!
GND ───────────────  GND (Preto)

⚠ NOTA: O servo MG995 requer 5V!
   Use fonte externa 5V para alimentar o servo,
   NÃO USE 3.3V do ESP32 diretamente.
   O pino de sinal (GPIO 33) suporta 3.3V.
```

### 3. Relay (Controle de Bomba)

```
Opção A: Relay 5V com módulo isolado
─────────────────────────────────────

ESP32 DOIT          MÓDULO RELAY 5V
─────────           ─────────────────
GPIO 32 ───────────  IN (sinal de controle)
GND ──────────────   GND
5V ──────────────    VCC (do módulo)

Bomba AC/DC         RELAY CONTACTS
───────────         ───────────────
Fase1 ─────────────  COM (comum)
Fase2 ─────────────  NO (normally open)
GND ──────────────   GND (compartilhado)

DIAGRAMA RELAY:
┌─────────┐
│  RELAY  │
├─────────┤
│ GND ────┼─── ESP32 GND
│ VCC ────┼─── 5V (fonte)
│ IN  ────┼─── GPIO 32 (ESP32)
└─────────┘
    ↓
  [COM] ────────→ Bomba+
  [NO]  ────────→ Bomba-
```

**Opção B: Relay 3.3V direto (menos recomendado)**
```
ESP32 DOIT          RELAY 3.3V
─────────           ──────────
GPIO 32 ───────────  IN
3.3V ──────────────  VCC
GND ──────────────   GND
```

## Esquema de Alimentação

```
Fonte Principal (5V / 2A mínimo)
│
├─→ ESP32 (via regulador 3.3V interno)
├─→ Servo Motor MG995 (5V)
├─→ Módulo Relay (5V)
└─→ Sensor XGZP6847D001MP (3.3V via ESP32 AMS1117)

⚠ IMPORTANTE:
   - ESP32 interno: 500mA máximo
   - Servo: 500mA em pico
   - Relay: 100mA típico
   TOTAL: ~1.2A em operação
```

## Proteções Recomendadas

```
1. Fusível/PTC no cabo principal (2A)
2. Diodo de proteção (1N4007) paralelo ao relay:
   ┌────[1N4007]────┐
   │   (↑ no GND)   │
   └────[RELAY]─────┘

3. Capacitor 100µF entre 5V e GND (próximo ao servo)
4. Resistor 10Ω em série com sinal PWM (GPIO 33)
```

## I2C Address Scan

Para verificar se o sensor está respondendo:

```cpp
// Código para testar I2C:
void scanI2C() {
    for(uint8_t addr = 0x01; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.print("Dispositivo encontrado: 0x");
            Serial.println(addr, HEX);
        }
    }
}
```

## Teste de Conexão

### 1. Verificar Sensor
```
- Medir tensão: VCC = 3.3V, GND = 0V
- Medir SCL e SDA: Oscilam entre 0V e 3.3V
- Serial: Deve mostrar leituras de pressão
```

### 2. Verificar Servo
```
- Medir: VCC = 5V, GND = 0V
- PWM: Sinal em GPIO 33
- Servo deve se mover ao ligar ESP32 (teste inicial)
```

### 3. Verificar Relay
```
- GPIO 32 = HIGH → Relay ATIVA (ouve "click")
- GPIO 32 = LOW → Relay DESATIVA (ouve "click")
- Medir continuidade entre COM e NO quando ativado
```

## Integração com Prototipagem

### Breadboard Layout (Recomendado para Teste)
```
┌────────────────────────────────────┐
│  ESP32 DOIT                         │
│  [SDA] [SCL] [5V] [GND] [GPIO33...] │
└────────────────────────────────────┘
         ↓      ↓      ↓    ↓    ↓
┌─────────────────────────────────────┐
│  BREADBOARD                         │
│  ┌──────────────────────────────┐   │
│  │  Rail + (5V)                │   │
│  │  Rail - (GND)               │   │
│  └──────────────────────────────┘   │
│  ┌──────────────────────────────┐   │
│  │ Sensor I2C + Resistores 4.7k│   │
│  │ Servo (conectar 5V ext)      │   │
│  │ Relay Módulo                 │   │
│  └──────────────────────────────┘   │
└─────────────────────────────────────┘
```

## Migração para PCB (Futuro)

Para versão PCB definitiva:
1. Eagle/KiCad schematic com isolação I2C
2. Plano de massa sólido
3. Capacitores de bypass (100nF) perto do sensor
4. Opto-isolador entre GPIO32 e relay (maior robustez)

---

**Versão**: 1.0  
**Data**: 23/04/2026
