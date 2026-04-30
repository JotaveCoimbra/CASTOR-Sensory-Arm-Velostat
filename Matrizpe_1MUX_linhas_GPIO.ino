#include <Arduino.h>

// ==========================
// Configurações de Threshold
// ==========================
const int THRESHOLD_M1 = 500;
const int THRESHOLD_M2 = 400;
const int THRESHOLD_PE = 400;

// ==========================
// Sensores dos pés
// ==========================
const int leftFoot = 36;
const int rightFoot = 39;

int baselineLeft = 0;
int baselineRight = 0;

// ==========================
// Linhas separadas (12 pinos)
// Continuam ligadas direto no ESP32
// ==========================
int linhas1[6] = {16, 17, 18, 19, 21, 22};       // Matriz 1
int linhas2[6] = {4, 23, 5, 25, 26, 27};          // Matriz 2

// ==========================
// MUX 74HC4067 unico para as colunas
// ==========================
const int S0 = 32;
const int S1 = 33;
const int S2 = 14;
const int S3 = 12;

// COM/SIG do unico MUX de colunas
const int SIG = 34;

// Canais do 4067 usados nas colunas
// Matriz 1 usa C0 ate C5
// C6 fica sem uso
// Matriz 2 usa C7 ate C12
int canaisM1[6] = {0, 1, 2, 3, 4, 5};
int canaisM2[6] = {7, 8, 9, 10, 11, 12};

// Baselines das matrizes
int baseline1[6][6];
int baseline2[6][6];

bool calibrado = false;

// ==========================
// Seleciona canal do mux
// ==========================
void setCanal(int ch) {
  digitalWrite(S0, (ch & 1) ? HIGH : LOW);
  digitalWrite(S1, (ch & 2) ? HIGH : LOW);
  digitalWrite(S2, (ch & 4) ? HIGH : LOW);
  digitalWrite(S3, (ch & 8) ? HIGH : LOW);
}

// ==========================
// Desativa todas as linhas
// Deixa em alta impedancia para evitar interferencia
// ==========================
void desativarTodasAsLinhas() {
  for (int i = 0; i < 6; i++) {
    pinMode(linhas1[i], INPUT);
    pinMode(linhas2[i], INPUT);
  }
}

// ==========================
// Ativa uma linha especifica
// ==========================
void ativarLinhaM1(int l) {
  desativarTodasAsLinhas();
  pinMode(linhas1[l], OUTPUT);
  digitalWrite(linhas1[l], HIGH);
}

void ativarLinhaM2(int l) {
  desativarTodasAsLinhas();
  pinMode(linhas2[l], OUTPUT);
  digitalWrite(linhas2[l], HIGH);
}

// ==========================
// Leitura estavel simples
// ==========================
int lerEstavel(int pin) {
  analogRead(pin);              // descarta primeira leitura apos chaveamento
  delayMicroseconds(50);
  return analogRead(pin);
}

// ==========================
// Média para os sensores dos pés
// ==========================
int lerMedia(int pin) {
  long soma = 0;

  for (int i = 0; i < 30; i++) {
    soma += analogRead(pin);
    delay(5);
  }

  return soma / 30;
}

// ==========================
// Calibração geral
// ==========================
void calibrarBaseline() {
  Serial.println("Calibrando baseline... NAO PRESSIONE OS SENSORES");
  delay(1000);

  // Calibra as matrizes com 1 MUX nas colunas e linhas direto nos GPIOs
  for (int l = 0; l < 6; l++) {
    // Matriz 1 - linha l
    ativarLinhaM1(l);
    delayMicroseconds(500);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM1[c]);
      delayMicroseconds(100);
      baseline1[l][c] = lerEstavel(SIG);
    }

    // Matriz 2 - linha l
    ativarLinhaM2(l);
    delayMicroseconds(500);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM2[c]);
      delayMicroseconds(100);
      baseline2[l][c] = lerEstavel(SIG);
    }
  }

  desativarTodasAsLinhas();

  // Calibra os pés
  baselineLeft = lerMedia(leftFoot);
  baselineRight = lerMedia(rightFoot);

  Serial.println("Baseline capturado com sucesso!");

  Serial.print("Baseline pe esquerdo: ");
  Serial.println(baselineLeft);

  Serial.print("Baseline pe direito: ");
  Serial.println(baselineRight);

  Serial.println("-------------------------------------------------");

  calibrado = true;
}

void setup() {
  Serial.begin(115200);

  //analogReadResolution(12);
  //analogSetAttenuation(ADC_11db);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(SIG, INPUT);
  pinMode(leftFoot, INPUT);
  pinMode(rightFoot, INPUT);

  desativarTodasAsLinhas();

  calibrarBaseline();
}

void loop() {
  if (!calibrado) return;

  int m1[6][6];
  int m2[6][6];

  // ==========================
  // Leitura das matrizes
  // 1 MUX nas colunas, linhas direto nos GPIOs
  // Intercalado por linha:
  // linha l da matriz 1 -> depois linha l da matriz 2
  // ==========================
  for (int l = 0; l < 6; l++) {
    // Matriz 1 - linha l
    ativarLinhaM1(l);
    delayMicroseconds(300);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM1[c]);
      delayMicroseconds(80);

      int val1 = lerEstavel(SIG);
      int d1 = val1 - baseline1[l][c];

      if (d1 < 0) d1 = 0;
      m1[l][c] = (d1 < THRESHOLD_M1) ? 0 : d1;
    }

    // Matriz 2 - linha l
    ativarLinhaM2(l);
    delayMicroseconds(300);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM2[c]);
      delayMicroseconds(80);

      int val2 = lerEstavel(SIG);
      int d2 = val2 - baseline2[l][c];

      if (d2 < 0) d2 = 0;
      m2[l][c] = (d2 < THRESHOLD_M2) ? 0 : d2;
    }
  }

  desativarTodasAsLinhas();

  // ==========================
  // Leitura dos pés
  // ==========================
  int leftRead = analogRead(leftFoot);
  int rightRead = analogRead(rightFoot);

  int deltaLeft = abs(leftRead - baselineLeft);
  int deltaRight = abs(rightRead - baselineRight);

  bool leftTouch = deltaLeft > THRESHOLD_PE;
  bool rightTouch = deltaRight > THRESHOLD_PE;

  // Evita warning caso nao use as variaveis no Serial
  (void)leftTouch;
  (void)rightTouch;

    // ==========================
  // Serial Monitor
  // ==========================
  Serial.println(">> MATRIZ 1                         | MATRIZ 2 <<");

  for (int l = 0; l < 6; l++) {
    // Matriz 1
    for (int c = 5; c >= 0; c--) {
      Serial.print(m1[l][c]);
      Serial.print("\t");
    }

    Serial.print(" | ");

    // Matriz 2
    for (int c = 0; c < 6; c++) {
      Serial.print(m2[l][c]);
      Serial.print("\t");
    }

    Serial.println();
  }

  // Separador entre matrizes e pés
  Serial.println("-------------------------");

  Serial.print("PE ESQUERDO: ");
  Serial.println(deltaLeft);
  //Serial.println(leftTouch ? "TOCOU" : "NAO");

  Serial.print("PE DIREITO:  ");
  Serial.print(deltaRight);
 
  //Serial.println(rightTouch ? "TOCOU" : "NAO");

 

  delay(50);
}
