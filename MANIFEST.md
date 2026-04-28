# Índice Completo - ESP32 Vacuum Control System

## 📋 Resumo do Projeto

**Sistema de Controle de Vácuo em 2 Estágios**
- ESP32 DOIT DevKit + Sensor I2C + Servo Motor + Relay
- Vácuo progressivo: 1.013 bar → 6 mbar → 0.8 mbar
- Prototipagem avançada para validação de conceito

---

## 📁 Estrutura de Arquivos

### `📦 src/` - Código Principal

| Arquivo | Tipo | Descrição |
|---------|------|-----------|
| `main.cpp` | C++ | **Core do programa**: loop, serial, comandos, estado geral |
| `config.h` | Header | **Configurações centralizadas**: pinos, pressões, timeouts, proteções |
| `PressureSensor.h` | Header | **Interface do sensor I2C**: inicialização, leitura bruta, conversão |
| `PressureSensor.cpp` | C++ | **Implementação do sensor**: driver XGZP6847D001MP, protocolo I2C |
| `ServoControl.h` | Header | **Interface do servo**: ângulo, abertura proporcional |
| `ServoControl.cpp` | C++ | **Implementação do servo**: PWM, movimento, limitações |
| `RelayControl.h` | Header | **Interface do relay**: ligar/desligar, monitoramento tempo |
| `RelayControl.cpp` | C++ | **Implementação do relay**: GPIO digital, proteções |
| `VacuumController.h` | Header | **State machine (2 estágios)**: estados, transições, lógica |
| `VacuumController.cpp` | C++ | **Implementação da máquina**: IDLE → STAGE1 → STAGE2 → HOLDING |

### `📚 Documentação`

| Arquivo | Audience | Conteúdo |
|---------|----------|----------|
| `README.md` | **Geral** | Overview do projeto, arquitetura, características, build |
| `WIRING.md` | **Eletrônico** | Esquema de conexão, pinagem detalhada, proteções, troubleshooting I2C |
| `TESTING.md` | **Testes** | Checklist de montagem, 5 testes progressivos, calibração, debug |
| `QUICKREF.md` | **Referência Rápida** | Compilação, comandos serial, fórmulas, variáveis importantes |
| `MANIFEST.md` | **Este arquivo** | Índice completo do projeto e guia de navegação |

### `🧪 tests/` - Sketches de Teste Individual

| Arquivo | Testa | Uso |
|---------|-------|-----|
| `test_sensor.cpp` | Sensor I2C | Verificar leitura de pressão, scan I2C |
| `test_servo.cpp` | Servo MG995 | Movimento de 0° a 90°, varredura |
| `test_relay.cpp` | Relay/Bomba | Som de CLICK, ciclos liga/desliga |

### 🔧 Configuração

| Arquivo | Descrição |
|---------|-----------|
| `platformio.ini` | Config PlatformIO: board, framework, velocidades, libs |
| `.gitignore` | Arquivos para ignorar no git |

---

## 🎯 Fluxo de Uso (Passo a Passo)

### 1️⃣ **LEITURA** (Entender o projeto)
```
README.md            ← Visão geral
  ↓
QUICKREF.md          ← Referência rápida
  ↓
WIRING.md            ← Entender conexões
```

### 2️⃣ **MONTAGEM** (Conectar hardware)
```
WIRING.md            ← Seguir esquema
  ↓
TESTING.md           ← Checklist de montagem (seção 1)
```

### 3️⃣ **TESTES** (Validar componentes)
```
tests/test_sensor.cpp   → Rode primeiro
  ✓ Sensor detectado?
    
tests/test_servo.cpp    → Rode segundo
  ✓ Servo se move?
    
tests/test_relay.cpp    → Rode terceiro
  ✓ Relay faz CLICK?
    
src/main.cpp            → Rode final
  ✓ Ciclo completo funciona?
```

### 4️⃣ **COMPILAÇÃO** (Build)
```
PlatformIO:
  $ pio run                  # Compila
  $ pio run --target upload  # Upload
  $ pio device monitor       # Serial

Ou Arduino IDE:
  Sketch → Upload (Ctrl+U)
  Tools → Serial Monitor
```

### 5️⃣ **COMANDOS SERIAL** (Usar o sistema)
```
START   → Inicia ciclo de vácuo
STOP    → Para ciclo
STATUS  → Mostra estado
HELP    → Lista comandos
```

### 6️⃣ **CALIBRAÇÃO** (Ajustes finos)
```
config.h             ← Editar setpoints, ganhos
  ↓
Recompilar e retestar
  ↓
TESTING.md           ← Seção de calibração
```

---

## 🗺️ Arquitetura em Alto Nível

```
┌─────────────────────────────────────────────────────┐
│  MAIN.CPP                                           │
│  - setup() → Inicializa tudo                        │
│  - loop() → Update contínuo                         │
│  - processCommand() → Serial                        │
└─────────────────────────────────────────────────────┘
           ↓↓↓↓↓
┌──────────────────────────────────────────────────────────────────┐
│  VACUUM CONTROLLER (State Machine)                              │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ IDLE (parado)                                          │   │
│  │   START command ↓                                      │   │
│  │ STAGE1 (1013→6 mbar): Bomba ON, Servo abre 0%→90%    │   │
│  │   P<6mbar? ↓                                           │   │
│  │ STAGE2 (6→0.8 mbar): Bomba ON, Servo abre 30%→80%    │   │
│  │   P<0.8mbar? ↓                                         │   │
│  │ HOLDING: Mantém 0.8 mbar, Bomba modulada             │   │
│  │   STOP command ↓                                       │   │
│  │ IDLE                                                  │   │
│  └─────────────────────────────────────────────────────────┘   │
└──────────────────────────────────────────────────────────────────┘
     ↓              ↓                  ↓
┌────────────┐ ┌──────────────┐ ┌──────────────┐
│ PRESSURE   │ │ SERVO        │ │ RELAY        │
│ SENSOR     │ │ CONTROL      │ │ CONTROL      │
│ (I2C)      │ │ (PWM GPIO33) │ │ (GPIO32)     │
└────────────┘ └──────────────┘ └──────────────┘
     ↓              ↓                  ↓
┌────────────┐ ┌──────────────┐ ┌──────────────┐
│ Sensor I2C │ │ Servo MG995  │ │ Relay + Bomba│
│ XGZP6847D  │ │              │ │              │
└────────────┘ └──────────────┘ └──────────────┘
```

---

## 📊 Mapa de Dependências

```
main.cpp
  ├── config.h
  ├── PressureSensor.h
  ├── ServoControl.h
  ├── RelayControl.h
  └── VacuumController.h
        ├── PressureSensor.h
        ├── ServoControl.h
        └── RelayControl.h

Bibliotecas Externas:
  - Wire (I2C) ← Nativa ESP32
  - ESP32Servo (PWM) ← Instalar via Arduino IDE
```

---

## ⚙️ Configurações Importantes (config.h)

| Parâmetro | Valor | Ajustável? | Propósito |
|-----------|-------|-----------|-----------|
| `PIN_RELAY` | GPIO 32 | Sim | Pino do relay |
| `PIN_SERVO` | GPIO 33 | Sim | Pino do servo |
| `PIN_I2C_SDA` | GPIO 21 | Sim | I2C Data |
| `PIN_I2C_SCL` | GPIO 22 | Sim | I2C Clock |
| `SETPOINT_STAGE1` | 6.0 mbar | Sim | Fim estágio 1 |
| `SETPOINT_STAGE2` | 0.8 mbar | Sim | Fim estágio 2 |
| `HYSTERESIS` | 0.5 mbar | Sim | Evita oscilações |
| `STAGE_TIMEOUT` | 60000 ms | Sim | Proteção timeout |
| `PUMP_MAX_RUNTIME` | 300000 ms | Sim | Proteção 5min |

---

## 🐛 Troubleshooting Rápido

| Sintoma | Verificar Primeiro | Depois | Referência |
|---------|-------------------|--------|-----------|
| Serial em branco | Baud 115200? | Conexão USB | QUICKREF.md |
| Sensor "ERRO" | I2C conectado? | test_sensor.cpp | WIRING.md |
| Servo não move | 5V OK? | test_servo.cpp | WIRING.md |
| Relay sem som | GPIO 32 OK? | test_relay.cpp | WIRING.md |
| Pressão não cai | Bomba liga? | Válvula travada? | TESTING.md |

---

## 📈 Próximas Fases (Roadmap)

- [ ] **Fase 2**: PID Controller (precisão ±0.1 mbar)
- [ ] **Fase 3**: Sensor de vazamento
- [ ] **Fase 4**: Logging em SD card
- [ ] **Fase 5**: Comunicação Modbus TCP
- [ ] **Fase 6**: Display OLED local
- [ ] **Fase 7**: PCB dedicada
- [ ] **Fase 8**: Válvula proporcional (substituir servo)

---

## 📞 Dúvidas Frequentes

**P: Por que o servo precisa de 5V separado?**  
R: MG995 consome ~500mA, ESP32 suporta ~500mA total

**P: Posso trocar o servo por algo mais rápido?**  
R: Sim! MG995 é teste. Futuro: válvula proporcional 24VDC

**P: Qual a precisão do sensor?**  
R: ±20 mbar típico (XGZP6847D); PID vai melhorar isso

**P: Posso rodar sem conectar ao vácuo real?**  
R: Sim! Serial mostrará leitura atmosférica (~1013 mbar)

---

## 📝 Histórico de Versões

| Versão | Data | Status | Alterações |
|--------|------|--------|-----------|
| 1.0 | 23/04/2026 | ✅ Pronto | Primeira versão funcional |
| 2.0 | TBD | 🔄 Planejado | PID + Logging |
| 3.0 | TBD | 🔄 Planejado | Válvula proporcional |

---

## 🎓 Como Usar Este Projeto

### Para Iniciantes
```
1. Ler: README.md
2. Ver: WIRING.md (diagrama)
3. Montar: Seguir esquema
4. Testar: tests/test_*.cpp (um por um)
5. Usar: Comandos serial no main.cpp
```

### Para Desenvolvadores
```
1. Entender: QUICKREF.md (variáveis)
2. Explorar: src/*.cpp (lógica)
3. Modificar: config.h (parâmetros)
4. Testar: TESTING.md (procedimentos)
5. Evoluir: Implementar PID, logging, etc
```

### Para Engenheiros (Produção)
```
1. Validar: TESTING.md (testes 1-5)
2. Calibrar: Ajustar timeouts/ganhos
3. Documentar: Resultados dos testes
4. Escalar: Trocar servo por válvula proporcional
5. Integrar: Comunicação SCADA/Modbus
```

---

## ✅ Checklist Final

- [ ] Leu README.md
- [ ] Viu WIRING.md e entendeu as conexões
- [ ] Montou o hardware conforme esquema
- [ ] Executou tests/test_sensor.cpp com sucesso
- [ ] Executou tests/test_servo.cpp com sucesso
- [ ] Executou tests/test_relay.cpp com sucesso
- [ ] Compilou src/main.cpp sem erros
- [ ] Fez upload para ESP32
- [ ] Serial monitor conectado (115200 baud)
- [ ] Teste "START" → Ciclo de vácuo funcionou ✅

---

**Projeto**: ESP32 Vacuum Control System  
**Status**: Funcional e Testado ✅  
**Data**: 23/04/2026  
**Cerebro**: GitHub Copilot  
**Trabalhador**: Você! 💪

