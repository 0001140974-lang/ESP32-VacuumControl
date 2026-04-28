# Guia de Testes e Troubleshooting

## Checklist de Montagem

- [ ] Sensor I2C conectado em GPIO 21 (SDA) e GPIO 22 (SCL)
- [ ] Resistores pull-up 4.7kΩ instalados em SDA e SCL
- [ ] Servo conectado em GPIO 33 com alimentação 5V externa
- [ ] Relay conectado em GPIO 32
- [ ] Bomba ligada aos contatos COM e NO do relay
- [ ] Todos os GNDs conectados
- [ ] ESP32 alimentado com 3.3V (USB ou fonte)
- [ ] Servo alimentado com 5V separado (IMPORTANTE!)

## Testes Progressivos

### TESTE 1: Upload e Serial
```
1. Abrir Arduino IDE ou PlatformIO
2. Upload do firmware main.cpp
3. Abrir Serial Monitor (115200 baud)
4. Esperado:
   ========================================
   ESP32 VACUUM CONTROL SYSTEM
   2-Stage Vacuum Control
   ========================================
   
   Inicializando sensor de pressão...
```

**Se não aparecer nada:**
- Verificar conexão USB
- Verificar board selecionada (ESP32 DOIT DevKit)
- Apertar RESET no ESP32

### TESTE 2: Detecção do Sensor

**Serial deve mostrar:**
```
[SENSOR] Raw: 3580 | Pressão: 1012.50 mbar
```

**Se não detectar sensor:**
```
Inicializando sensor de pressão... ERRO!
(Trava no setup)
```

**Ações:**
1. Verificar voltagem em VCC do sensor (3.3V)
2. Verificar voltagem em GND (0V)
3. Testar I2C com código de scan:

```cpp
// Teste de I2C
void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);  // SDA=21, SCL=22
    Serial.println("Escaneando I2C...");
    
    for(int i = 0; i < 127; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
            Serial.print("Dispositivo encontrado em: 0x");
            Serial.println(i, HEX);
        }
    }
}

void loop() {}
```

4. Se não encontrar em 0x6D, verificar datasheet do sensor
5. Se encontrar em endereço diferente, atualizar `config.h`

### TESTE 3: Servo Motor

**Teste manual (sem vácuo):**

Enviar pela serial:
```
START
```

**Esperado:**
```
>>> Iniciando ciclo de vácuo...
[STATE] Transição para: STAGE1 (1013→6 mbar)
[RELAY] Bomba LIGADA
[SERVO] Ângulo: 50.0°
```

**Se servo não move:**
1. Verificar alimentação 5V (5V = sim, não confundir com 3.3V)
2. Verificar voltagem no pino amarelo do servo (deve oscilar PWM)
3. Testar servo com sketch simples:

```cpp
#include <ESP32Servo.h>

Servo s;
void setup() {
    s.setPeriodHertz(50);
    s.attach(33, 500, 2500);  // GPIO 33, 500-2500 µs
    s.write(0);  // Fechado
    delay(1000);
    s.write(90); // Aberto
}

void loop() {}
```

4. Se não responder, possível servo queimado

### TESTE 4: Relay (Bomba)

**Teste:** Enviar `START` e ouvir "CLICK" no relay

**Se não funciona:**
```
[RELAY] Bomba LIGADA
[RELAY] Bomba DESLIGADA (tempo: XXXms)
```
mas SEM SOM no relay:

1. Testar voltagem em VCC do módulo relay (5V)
2. Verificar voltagem em GPIO 32 (deve ser 3.3V ou 0V)
3. Testar relay com sketch simples:

```cpp
void setup() {
    pinMode(32, OUTPUT);
}

void loop() {
    digitalWrite(32, HIGH);   // Liga
    delay(500);
    digitalWrite(32, LOW);    // Desliga
    delay(500);
}
```

4. Se still não funciona, trocar módulo relay

### TESTE 5: Ciclo Completo de Vácuo

**Com sistema montado:**

```
1. Serial: "START"
2. Observar transição de estados
3. Pressão deve cair de 1013 para 6 mbar (estágio 1)
4. Depois cair de 6 para 0.8 mbar (estágio 2)
5. Manter em 0.8 mbar (holding)
6. Serial: "STOP" para interromper
```

**Se pressão não cai:**
- [ ] Bomba está ligando? (verificar GPIO 32)
- [ ] Servo está abrindo? (ouvir movimento)
- [ ] Há vazamento? (colocar mão perto, sentir sucção)
- [ ] Tubo está entupido?

**Se pressão cai muito rápido:**
- [ ] Normal! Sistema novo pode ter vazios rápidos
- [ ] Aumentar `HYSTERESIS` em `config.h` se oscilar

## Modo Debug Avançado

### Ativar Logs Detalhados

Em `config.h`:
```cpp
#define DEBUG_SERIAL true  // Mais mensagens
```

### Monitorar Estados em Tempo Real

Serial monitor mostra:
- Estado atual
- Pressão instantânea
- Posição do servo
- Status da bomba

### Registrar Dados para Análise

Adicionar no `main.cpp`:
```cpp
void logData() {
    static File f;
    if (!f) f = SD.open("/vacuum.csv", FILE_WRITE);
    
    char buffer[100];
    sprintf(buffer, "%lu,%f,%f,%d\n",
            millis(),
            vacuumController.getCurrentPressure(),
            vacuumController.getServoPosition(),
            vacuumController.isPumpOn());
    
    f.println(buffer);
}
```

## Problemas Comuns e Soluções

| Problema | Causa | Solução |
|----------|-------|--------|
| Sensor não detectado | Fio solto ou I2C errado | Verificar conexões, testar scan |
| Servo não move | Sem alimentação 5V | Providenciar fonte 5V separada |
| Pressão não diminui | Bomba não funciona | Testar relay independente |
| Pressão sobe rápido | Vazamento | Verificar conexões da mangueira |
| Servo fica em posição estranha | PWM errado | Calibrar SERVO_MIN/MAX_PWM |
| Sistema entra em ERROR | Timeout ou proteção | Ver logs, aumentar timeout |
| Leitura de pressão ruim | Sensor desconectado | Resetar ESP32 e rescan I2C |

## Calibração Fina

### Ajustar Ganho de Abertura de Válvula

Em `VacuumController.cpp`, função `calculateServoOpening()`:

```cpp
// Estágio 1: aumentar de 0.9 para mais rápido
float ratio = (PRESSURE_ATMOS - pressure) / (PRESSURE_ATMOS - SETPOINT_STAGE1);
opening = ratio * 0.95f;  // Era 0.9f
```

### Ajustar Velocidade de Bomba

Por enquanto bomba é ON/OFF. Para futuro com PWM:

```cpp
// Modular intensidade em STATE_HOLDING
analogWrite(PUMP_PWM_PIN, 150);  // 0-255
```

### Ajustar Histerese (Oscilações)

Se pressão oscilar muito em `HOLDING`:

```cpp
#define HYSTERESIS 1.0f  // Aumentar de 0.5f
```

## Teste de Durabilidade

Rodar por 10 ciclos completos:

```cpp
// Adicionar no main.cpp
int cycleCount = 0;
unsigned long lastCycleEnd = 0;

if (vacuumController.getState() == STATE_IDLE && cycleCount < 10) {
    if ((millis() - lastCycleEnd) > 30000) {  // 30s entre ciclos
        vacuumController.startVacuum();
        cycleCount++;
    }
}
```

Observar:
- Servo não tranca
- Relay continua fazendo "click"
- Pressão permanece consistente
- Sem sobreaquecimento

## Próximos Passos

1. ✅ Validar leitura de sensor (Teste 2)
2. ✅ Validar movimento de servo (Teste 3)
3. ✅ Validar funcionamento de relay (Teste 4)
4. ✅ Ciclo completo (Teste 5)
5. 🔄 Calibração de velocidade
6. 🔄 Testes de durabilidade (10-100 ciclos)
7. 🔄 Implementar PID para precisão (opcional)
8. 🔄 Adicionar sensor de vazamento
9. 🔄 Integração com SCADA

---

**Autor**: Gabriel  
**Versão**: 1.0  
**Data**: 23/04/2026
