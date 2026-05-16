#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

class PressureSensor {
public:
    // Construtor com o endereço padrão (0x6D)
    PressureSensor(uint8_t i2c_addr = 0x6D);

    // Inicializa o I2C passando os pinos (Padrão ESP32: SDA 21, SCL 22)
    bool begin(int sda_pin = 21, int scl_pin = 22);
    
    // Verifica se o sensor responde no barramento
    bool isConnected();
    
    // Faz várias leituras para zerar a pressão (tara)
    void calibrateZero(int samples = 50);
    
    // Lê e retorna o vácuo relativo em mbar
    float readPressure();

private:
    uint8_t _addr;
    float _lastPressure;
    float _offset;
    bool _debug;

    uint32_t readRaw();
    float convertRawToPressure(uint32_t raw);
};

#endif
