#include <Arduino.h>
#include <Wire.h>

const int SDA_PIN = 8;
const int SCL_PIN = 9;
uint8_t foundAddr = 0;
bool writeDAC(uint8_t addr, uint8_t value);
int readADC(uint8_t addr, uint8_t channel);
void runLoopTest(uint8_t addr);

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);
    delay(100);
    Serial.println("I2C scan");
    for (uint8_t a = 3; a < 120; ++a) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0) {
            foundAddr = a;
            Serial.print("found addr 0x");
            if (a < 16) Serial.print('0');
            Serial.println(a, HEX);
        }
    }
    if (foundAddr == 0) {
        Serial.println("no device found");
        while (1) delay(1000);
    }
    Serial.print("using address 0x");
    if (foundAddr < 16) Serial.print('0');
    Serial.println(foundAddr, HEX);
    delay(200);
    runLoopTest(foundAddr);
}

void loop() { delay(1000); }

bool writeDAC(uint8_t addr, uint8_t value) {
    Wire.beginTransmission(addr);
    Wire.write(0x40);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

int readADC(uint8_t addr, uint8_t channel) {
    uint8_t control = 0x40 | (channel & 0x03);
    Wire.beginTransmission(addr);
    Wire.write(control);
    if (Wire.endTransmission() != 0) return -1;
    Wire.requestFrom((int)addr, 2);
    if (Wire.available() < 2) return -1;
    Wire.read();
    int v = Wire.read();
    return v;
}

void runLoopTest(uint8_t addr) {
    Serial.println("DAC->ADC loop test");
    for (int v = 0; v <= 255; v += 16) {
        bool ok = writeDAC(addr, (uint8_t)v);
        if (!ok) {
            Serial.println("DAC write fail");
            break;
        }
        delay(20);
        int r = readADC(addr, 0);
        if (r < 0)
            Serial.println("ADC read fail");
        else {
            Serial.print("DAC ");
            Serial.print(v);
            Serial.print("  ADC ");
            Serial.println(r);
        }
        delay(100);
    }
    Serial.println("test complete");
    while (1) delay(1000);
}

