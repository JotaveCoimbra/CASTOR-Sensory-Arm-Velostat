#include <Arduino.h>
#include <Wire.h>
#include <vl53lx_class.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define DEV_I2C Wire
#define SerialPort Serial

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define LedPin LED_BUILTIN
#define XSHUT_PIN 0

// =========================
// SENSOR LASER VL53L3CX
// =========================
VL53LX sensor_vl53lx_sat(&DEV_I2C, XSHUT_PIN);

// =========================
// SENSORES DE TOQUE
// =========================
int sensores[2] = {34, 35};
int baseline[2];
bool calibrado = false;

const int TH = 300;

// =========================
// VARIÁVEIS DO LASER
// =========================
int ultimaDistancia = -1;
bool laserValido = false;

// =========================
// CALIBRAÇÃO DO TOQUE
// =========================
void calibrarBaseline() {
  Serial.println("Calibrando baseline...");
  delay(500);

  for (int i = 0; i < 2; i++) {
    long soma = 0;
    for (int j = 0; j < 20; j++) {
      soma += analogRead(sensores[i]);
      delay(5);
    }
    baseline[i] = soma / 20;
  }

  Serial.println("Baseline capturado!");
  calibrado = true;
}

// =========================
// SETUP
// =========================
void setup() {
  pinMode(LedPin, OUTPUT);

  SerialPort.begin(115200);
  Serial.println("Starting...");

  // I2C
  DEV_I2C.begin();

  // Inicialização do VL53L3CX
  sensor_vl53lx_sat.begin();
  sensor_vl53lx_sat.VL53LX_Off();
  sensor_vl53lx_sat.InitSensor(0x12);
  sensor_vl53lx_sat.VL53LX_StartMeasurement();

  // Calibração dos sensores de toque
  calibrarBaseline();
}

// =========================
// LOOP
// =========================
void loop() {
  // -------------------------
  // LEITURA DOS SENSORES DE TOQUE
  // -------------------------
  bool toque1 = false;
  bool toque2 = false;

  if (calibrado) {
    for (int i = 0; i < 2; i++) {
      int leitura = analogRead(sensores[i]);
      int delta = leitura - baseline[i];

      if (delta < 0) {
        delta = 0;
      }

      if (delta > TH) {
        if (i == 0) toque1 = true;
        if (i == 1) toque2 = true;
      }
    }
  }

  // -------------------------
  // LEITURA DO SENSOR LASER
  // -------------------------
  VL53LX_MultiRangingData_t MultiRangingData;
  VL53LX_MultiRangingData_t *pMultiRangingData = &MultiRangingData;
  uint8_t NewDataReady = 0;
  int no_of_object_found = 0;
  int status;

  status = sensor_vl53lx_sat.VL53LX_GetMeasurementDataReady(&NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    digitalWrite(LedPin, HIGH);

    status = sensor_vl53lx_sat.VL53LX_GetMultiRangingData(pMultiRangingData);
    no_of_object_found = pMultiRangingData->NumberOfObjectsFound;

    if (no_of_object_found > 0) {
      ultimaDistancia = pMultiRangingData->RangeData[0].RangeMilliMeter;
      laserValido = true;
    } else {
      laserValido = false;
    }

    sensor_vl53lx_sat.VL53LX_ClearInterruptAndStartMeasurement();
    digitalWrite(LedPin, LOW);
  }

  // -------------------------
  // SAÍDA SERIAL UNIFICADA
  // -------------------------
  Serial.print("DIREITA: ");
  Serial.print(toque1 ? "1" : "0");

  Serial.print(" | ESQUERDA: ");
  Serial.print(toque2 ? "1" : "0");

  Serial.print(" | LASER: ");
  if (laserValido) {
    Serial.print(ultimaDistancia);
    Serial.println(" mm");
  } else {
    Serial.println("sem leitura");
  }

  delay(50);
}