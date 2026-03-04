#include <Arduino.h>

// ==========================
// Linhas separadas (12 pinos)
// ==========================
int linhas1[6] = {16, 17, 18, 19, 21, 22};       // Matriz 1
int linhas2[6] = {4, 5, 13, 15, 23, 2};          // Matriz 2

// ==========================
// MUX 74HC4067 (S0..S3) compartilhados
// ==========================
const int S0 = 25;
const int S1 = 26;
const int S2 = 27;
const int S3 = 14;

// COM de cada mux (um por matriz)
const int SIG1 = 32;   // Matriz 1 (ADC1)
const int SIG2 = 33;   // Matriz 2 (ADC1)

// Canais do 4067 usados nas 6 colunas
int canais[6] = {0, 1, 2, 3, 4, 5};

// Baselines
int baseline1[6][6];
int baseline2[6][6];
bool calibrado = false;

// Seleciona canal do mux (mexendo em S0..S3)
void setCanal(int ch) {
  digitalWrite(S0, (ch & 1) ? HIGH : LOW);
  digitalWrite(S1, (ch & 2) ? HIGH : LOW);
  digitalWrite(S2, (ch & 4) ? HIGH : LOW);
  digitalWrite(S3, (ch & 8) ? HIGH : LOW);
}

// Leitura mais estável (evita “vazar” leitura entre pinos do ADC)
int lerEstavel(int pin) {
  analogRead(pin);              // descarta 1ª
  delayMicroseconds(40);
  return analogRead(pin);
}

// ======================================================
// Calibra baseline automaticamente (DUAS matrizes juntas)
// ======================================================
void calibrarBaseline() {
  Serial.println("Calibrando baseline... NAO APERTE NADA");
  delay(500);

  for (int l = 0; l < 6; l++) {

    // coloca TODAS as 12 linhas em alta impedância
    for (int i = 0; i < 6; i++) {
      pinMode(linhas1[i], INPUT);
      pinMode(linhas2[i], INPUT);
    }

    // ativa a linha l das DUAS matrizes ao mesmo tempo
    pinMode(linhas1[l], OUTPUT);
    digitalWrite(linhas1[l], HIGH);

    pinMode(linhas2[l], OUTPUT);
    digitalWrite(linhas2[l], HIGH);

    delayMicroseconds(300);

    // lê 6 colunas (mesmo canal pros dois mux)
    for (int c = 0; c < 6; c++) {
      setCanal(canais[c]);
      delayMicroseconds(80);

      baseline1[l][c] = lerEstavel(SIG1);
      baseline2[l][c] = lerEstavel(SIG2);
    }

    // desativa linhas
    pinMode(linhas1[l], INPUT);
    pinMode(linhas2[l], INPUT);
    delayMicroseconds(300);
  }

  Serial.println("Baseline capturado!");
  calibrado = true;
}

// ======================================================
// Setup
// ======================================================
void setup() {
  Serial.begin(115200);

  // ESP32 ADC (ajuda)
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // pinos do mux
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // começa tudo em Hi-Z
  for (int i = 0; i < 6; i++) {
    pinMode(linhas1[i], INPUT);
    pinMode(linhas2[i], INPUT);
  }

  calibrarBaseline();
}

// ======================================================
// Loop principal
// ======================================================
void loop() {
  if (!calibrado) return;

  int m1[6][6];
  int m2[6][6];

  for (int l = 0; l < 6; l++) {

    // todas as 12 linhas em alta impedância
    for (int i = 0; i < 6; i++) {
      pinMode(linhas1[i], INPUT);
      pinMode(linhas2[i], INPUT);
    }

    // ativa a linha l das DUAS matrizes
    pinMode(linhas1[l], OUTPUT);
    digitalWrite(linhas1[l], HIGH);

    pinMode(linhas2[l], OUTPUT);
    digitalWrite(linhas2[l], HIGH);

    delayMicroseconds(300);

    // lê as 6 colunas (mesmo canal) e salva os deltas
    for (int c = 0; c < 6; c++) {
      setCanal(canais[c]);
      delayMicroseconds(80);

      int le1 = lerEstavel(SIG1);
      int le2 = lerEstavel(SIG2);

      int d1 = le1 - baseline1[l][c];
      int d2 = le2 - baseline2[l][c];

      if (d1 < 0) d1 = 0;
      if (d2 < 0) d2 = 0;

      m1[l][c] = d1;
      m2[l][c] = d2;
    }

    // desativa a linha
    pinMode(linhas1[l], INPUT);
    pinMode(linhas2[l], INPUT);
    delayMicroseconds(300);
  }

  // PRINT lado a lado
  Serial.println("----- MATRIZ 1 | MATRIZ 2 -----");
  for (int l = 0; l < 6; l++) {

    // M1 (se quiser espelhar, faça c=5..0)
    for (int c = 0; c < 6; c++) {
      Serial.print(m1[l][c]);
      Serial.print("\t");
    }

    Serial.print(" | \t");

    // M2
    for (int c = 0; c < 6; c++) {
      Serial.print(m2[l][c]);
      Serial.print("\t");
    }

    Serial.println();
  }
  Serial.println("------------------------------\n");

  delay(200);
}