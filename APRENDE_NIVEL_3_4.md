# 🎓 APRENDE DO ZERO - Níveis 3-4

---

## 📍 NÍVEL 3: OOP AVANÇADO (Conceitos Profissionais)

### Você já sabe:
- ✅ Classes (molde)
- ✅ `public:` e `private:`
- ✅ Construtores

### Agora vamos aprender:
- 🔄 **Herança** (reutilizar classes)
- 🎭 **Polimorfismo** (mesma função, comportamentos diferentes)
- 🔒 **Encapsulamento** (proteção de dados)

---

## 🔄 HERANÇA (Inheritance)

### O que é?

Uma classe **herda** características de outra classe (classe pai/base).

**Analogia:**

```
Animal (classe pai)
  ├─ Cachorro (classe filha herda de Animal)
  ├─ Gato (classe filha herda de Animal)
  └─ Pássaro (classe filha herda de Animal)

Todos têm: nome, idade, fazer som()
Mas cada um faz som diferente!
```

### Exemplo Simples

```cpp
// ===== Classe PAI (Base) =====
class Animal {
public:
    Animal(const char* name) {
        strcpy(nom, name);
    }
    
    void displayName() {
        Serial.println(nom);
    }
    
    // Função VIRTUAL (pode ser modificada nas filhas)
    virtual void fazerSom() {
        Serial.println("Som genérico");
    }
    
private:
    char nom[50];
};

// ===== Classe FILHA 1 =====
class Cachorro : public Animal {  // ← HERDA de Animal
public:
    Cachorro(const char* name) : Animal(name) {
        // Construtor chama construtor do pai
    }
    
    // SOBRESCREVE fazerSom()
    void fazerSom() override {  // ← Sobrescreve a função do pai
        Serial.println("Au au!");
    }
};

// ===== Classe FILHA 2 =====
class Gato : public Animal {
public:
    Gato(const char* name) : Animal(name) {}
    
    void fazerSom() override {
        Serial.println("Miau!");
    }
};
```

### Usando:

```cpp
void setup() {
    Serial.begin(115200);
    
    Animal* dog = new Cachorro("Rex");
    Animal* cat = new Gato("Whiskers");
    
    dog->displayName();  // Mostra "Rex"
    dog->fazerSom();     // Mostra "Au au!"
    
    cat->displayName();  // Mostra "Whiskers"
    cat->fazerSom();     // Mostra "Miau!"
}
```

### Nomenclatura

```cpp
class Filha : public Pai {  // ← "herda publicamente de Pai"
    // ...
};

virtual void funcao() { }   // ← "virtual" = pode ser mudada nas filhas
void funcao() override { }  // ← "override" = esta é uma versão modificada
```

---

## 🎭 POLIMORFISMO (Polymorphism)

### O que é?

**"Muitas formas"** - Mesma função, comportamentos diferentes.

**Já vimos no exemplo acima!**

```cpp
Animal* dog = new Cachorro("Rex");
dog->fazerSom();  // Chama Cachorro::fazerSom()

Animal* cat = new Gato("Whiskers");
cat->fazerSom();  // Chama Gato::fazerSom()
```

Mesmo nome (`fazerSom`), mas **comportamentos diferentes**!

### Vantagem

```cpp
// SEM polimorfismo:
Cachorro dog("Rex");
Gato cat("Whiskers");

if (tipo == DOG) {
    dog.fazerSom();
} else if (tipo == CAT) {
    cat.fazerSom();
}
// Muito código repetido!

// COM polimorfismo:
Animal* animals[] = {new Cachorro("Rex"), new Gato("Whiskers")};

for (int i = 0; i < 2; i++) {
    animals[i]->fazerSom();  // Funciona para todos!
}
```

---

## 🔒 ENCAPSULAMENTO (Encapsulation)

### O que é?

**Esconder dados** e fornecer funções para acessá-los.

### Exemplo

```cpp
// ❌ SEM encapsulamento:
class Pessoa {
public:
    int idade = -5;  // ← Alguém pode colocar -5!
};

// ✅ COM encapsulamento:
class Pessoa {
private:
    int idade;  // ← Escondido!
    
public:
    // Função para SET (definir) com validação
    void setIdade(int i) {
        if (i > 0 && i < 150) {
            idade = i;  // Só aceita valores válidos
        } else {
            Serial.println("Idade inválida!");
        }
    }
    
    // Função para GET (pegar)
    int getIdade() {
        return idade;
    }
};
```

### Usando:

```cpp
Pessoa p;
p.setIdade(-5);     // Rejeita!
p.setIdade(25);     // OK
Serial.println(p.getIdade());  // Mostra 25
```

---

## 🎯 Resumo Nível 3

| Conceito | O Quê | Por Quê |
|----------|-------|--------|
| **Herança** | Uma classe herda de outra | Reutilizar código |
| **Polimorfismo** | Mesma função, comportamentos diferentes | Flexibilidade |
| **Encapsulamento** | Esconder dados, expor métodos | Proteção |
| **`virtual`** | Função que pode ser modificada | Permite sobrescrita |
| **`override`** | Marca que estou modificando função do pai | Clareza no código |

---

---

## 📍 NÍVEL 4: FERRAMENTAS REAIS

Agora vamos aprender as 3 ferramentas principais do seu projeto:

1. **Wire.h** - Comunicação I2C
2. **ESP32Servo.h** - Controle de servo
3. **State Machine** - Máquina de estados

---

## 🔌 FERRAMENTA 1: Wire.h (I2C)

### O que é I2C?

**I2C = Inter-Integrated Circuit**

- Protocolo de comunicação entre chips
- Usa 2 fios: **SDA** (dados) e **SCL** (clock)
- Sensor ← (2 fios) → ESP32

### Conceptualmente

```
ESP32                         SENSOR
├─ SDA (GPIO 21) ────────── SDA
├─ SCL (GPIO 22) ────────── SCL
├─ 3.3V  ────────────────── VDD
└─ GND   ────────────────── GND
```

### Como funciona?

```
1. ESP32 fala: "Oi, endereço 0x58?"
2. Sensor: "Sim, sou eu!"
3. ESP32: "Me dá dados de pressão?"
4. Sensor: "Aqui está: 0x04, 0x05, 0x06"
5. ESP32: "Obrigado, até logo!"
```

### Wire.h Functions

```cpp
#include <Wire.h>

// Inicialização
Wire.begin(SDA_PIN, SCL_PIN);  // Ativa I2C nos pinos
Wire.setClock(100000);          // Velocidade 100 kHz

// ESCREVER para sensor
Wire.beginTransmission(0x58);   // Começa transmissão pro endereço 0x58
Wire.write(0x01);               // Envia comando 0x01
Wire.endTransmission();         // Termina

// LER do sensor
Wire.requestFrom(0x58, 3);      // Pede 3 bytes do sensor
byte1 = Wire.read();            // Lê byte 1
byte2 = Wire.read();            // Lê byte 2
byte3 = Wire.read();            // Lê byte 3
```

### No Seu Projeto

```cpp
// PressureSensor.cpp
uint32_t PressureSensor::readRaw() {
    // Escreve: solicita dados de pressão
    Wire.beginTransmission(_addr);
    Wire.write(0x04);
    Wire.endTransmission(false);
    
    delay(10);  // Aguarda sensor processar
    
    // Lê: 3 bytes de pressão
    Wire.requestFrom(_addr, (uint8_t)3);
    uint8_t byte_h = Wire.read();
    uint8_t byte_m = Wire.read();
    uint8_t byte_l = Wire.read();
    
    // Combina: 3 bytes → 1 número de 24 bits
    uint32_t raw = ((uint32_t)byte_h << 16) | 
                   ((uint32_t)byte_m << 8) | 
                   byte_l;
    
    return raw;
}
```

---

## 🎛️ FERRAMENTA 2: ESP32Servo.h

### O que é?

Biblioteca para controlar servo motores via PWM.

### Conceptualmente

```
PWM (Pulse Width Modulation):

Quer 0°?   ──┐ 500µs ├─────────────────────┤
            
Quer 45°?  ──┐1500µs ├──────────┤
            
Quer 90°?  ──┐2500µs ├────┤
```

O servo mede o **tempo ligado** (pulse width) para saber o ângulo.

### ESP32Servo Functions

```cpp
#include <ESP32Servo.h>

Servo servo;

// Anexa servo a um pino
servo.attach(GPIO_PIN);  // Ex: GPIO 33

// Posiciona em ângulo (0-180°)
servo.write(0);          // 0°
servo.write(90);         // 90°
servo.write(180);        // 180°

// Lê ângulo atual
int ang = servo.read();
```

### No Seu Projeto

```cpp
// ServoControl.cpp
void ServoControl::begin() {
    _servo.attach(_pin);  // Anexa servo ao GPIO 33
    setAngle(0);          // Começa em 0°
}

void ServoControl::setAngle(uint8_t angle) {
    if (angle <= SERVO_MAX_ANGLE) {
        _servo.write(angle);  // Posiciona
    }
}

void ServoControl::setOpeningRatio(float ratio) {
    // ratio = 0.0 a 1.0
    // 0.0 = 0° (fechado)
    // 1.0 = 90° (aberto)
    
    uint8_t angle = ratio * 90;
    setAngle(angle);
}
```

---

## 🤖 FERRAMENTA 3: State Machine (Máquina de Estados)

### O que é?

Um sistema que muda de **estado** conforme condições.

**Analogia:**

```
Semáforo:
├─ VERMELHO: parado
├─ AMARELO: preparar
└─ VERDE: ir

Transições:
VERMELHO → AMARELO (após tempo)
AMARELO → VERDE (após tempo)
VERDE → VERMELHO (após tempo)
```

### Seu Projeto

```
IDLE (parado)
  ↓ (user clica START)
STAGE1 (1013 → 6 mbar)
  ↓ (pressão ≤ 6 mbar)
STAGE2 (6 → 0.8 mbar)
  ↓ (pressão ≤ 0.8 mbar)
HOLDING (mantém 0.8 mbar)
  ↓ (user clica STOP)
IDLE
```

### Implementação

```cpp
// ===== DEFINIR ESTADOS =====
enum VacuumState {
    IDLE,     // 0
    STAGE1,   // 1
    STAGE2,   // 2
    HOLDING,  // 3
    ERROR     // 4
};

// ===== CLASSE =====
class VacuumController {
private:
    VacuumState state = IDLE;  // Estado atual
    
public:
    void update() {
        switch (state) {
            case IDLE:
                // Não faz nada
                break;
                
            case STAGE1:
                // Abre válvula, liga bomba
                // Se pressão ≤ 6 mbar:
                if (pressure <= SETPOINT_STAGE1) {
                    state = STAGE2;  // ← TRANSIÇÃO!
                }
                break;
                
            case STAGE2:
                // Continua abrindo válvula
                // Se pressão ≤ 0.8 mbar:
                if (pressure <= SETPOINT_STAGE2) {
                    state = HOLDING;  // ← TRANSIÇÃO!
                }
                break;
                
            case HOLDING:
                // Pulsa bomba para manter pressão
                break;
                
            case ERROR:
                // Liga alarme
                break;
        }
    }
    
    void startVacuum() {
        state = STAGE1;  // ← MUDANÇA DE ESTADO
    }
    
    void stopVacuum() {
        state = IDLE;    // ← MUDANÇA DE ESTADO
    }
};
```

### Usando

```cpp
VacuumController vc;

void loop() {
    // Lê pressão
    float p = sensor.readPressure();
    
    // Atualiza máquina de estados
    vc.update(p);
    
    // Se pressionou botão START:
    if (buttonPressed) {
        vc.startVacuum();  // Muda pra STAGE1
    }
}
```

### Diagrama de Transições

```
         ┌─────────────┐
         │    IDLE     │
         └──────┬──────┘
                │ startVacuum()
         ┌──────▼──────┐
         │   STAGE1    │ (abre 0→90%)
         │ 1013 → 6mb  │
         └──────┬──────┘
                │ P ≤ 6mbar
         ┌──────▼──────┐
         │   STAGE2    │ (abre 30→80%)
         │ 6 → 0.8 mb  │
         └──────┬──────┘
                │ P ≤ 0.8mbar
         ┌──────▼──────┐
         │  HOLDING    │ (pulsa)
         │ mantém 0.8  │
         └──────┬──────┘
                │ stopVacuum()
         ┌──────▼──────┐
         │    IDLE     │
         └─────────────┘
```

---

## 🎯 Como Tudo Funciona Junto

```
loop() roda a cada 100ms
  │
  ├─ Sensor.readPressure()
  │  └─ Wire.read(0x04, 0x05, 0x06)  ← Wire.h
  │  └─ Converte 3 bytes → pressão
  │
  ├─ VacuumController.update(pressure)
  │  └─ Máquina de estados ← State Machine
  │  └─ Calcula ângulo servo
  │
  ├─ Servo.write(angle) ← ESP32Servo.h
  │  └─ Envia PWM ao servo
  │
  └─ Relay.on/off()
     └─ Liga/desliga bomba
```

---

## 📊 Resumo Nível 4

| Ferramenta | O Quê | Como Usa |
|-----------|-------|----------|
| **Wire.h** | Comunica com sensor via I2C | `Wire.read()`, `Wire.write()` |
| **ESP32Servo.h** | Controla servo motor | `servo.write(angle)` |
| **State Machine** | Muda comportamento conforme estado | `switch(state)` + transições |

---

---

## 🔜 AGORA ENTENDE SEU PROJETO!

Você agora sabe:

✅ **Como ler sensor** (Wire.h + PressureSensor.cpp)
✅ **Como controlar servo** (ESP32Servo.h + ServoControl.cpp)
✅ **Como coordenar tudo** (State Machine + VacuumController.cpp)

**Próximo passo:** Estudar cada arquivo do seu projeto com esse conhecimento!

---

## 📚 Próximas Lições Recomendadas

1. Ler e entender `config.h` (configurações)
2. Ler e entender `PressureSensor.cpp` (Wire.h em ação)
3. Ler e entender `ServoControl.cpp` (ESP32Servo em ação)
4. Ler e entender `VacuumController.cpp` (State Machine em ação)
5. Entender `main.cpp` (orquestração)

---

**Parabéns! Você agora sabe OOP e as ferramentas do projeto!** 🚀
