#include <Arduino.h>

// ==========================
// Configuracoes de Threshold
// ==========================
const int THRESHOLD = 500;   // Matriz 1
const int THRESHOLD2 = 400;  // Matriz 2

// ==========================
// Linhas separadas (12 pinos)
// ==========================
int linhas1[6] = {16, 17, 18, 19, 21, 22};       // Matriz 1
int linhas2[6] = {4, 23, 5, 25, 26, 27};          // Matriz 2

// ==========================
// MUX 74HC4067 unico
// ==========================
const int S0 = 32;
const int S1 = 33;
const int S2 = 14;
const int S3 = 12;

// COM/SIG do unico mux
const int SIG = 34;   // ADC1 - Input Only

// ==========================
// Canais do 74HC4067
// ==========================
// Ligue as 6 colunas da matriz 1 nos canais C0 a C5
// Ligue as 6 colunas da matriz 2 nos canais C7 a C12
// O canal C6 fica sem uso por questao de espaco/fiacao

int canaisM1[6] = {0, 1, 2, 3, 4, 5};
int canaisM2[6] = {7, 8, 9, 10, 11, 12};

// Baselines da estrutura
int baseline1[6][6];
int baseline2[6][6];
bool calibrado = false;

// Seleciona canal do mux
void setCanal(int ch) {
  digitalWrite(S0, (ch & 1) ? HIGH : LOW);
  digitalWrite(S1, (ch & 2) ? HIGH : LOW);
  digitalWrite(S2, (ch & 4) ? HIGH : LOW);
  digitalWrite(S3, (ch & 8) ? HIGH : LOW);
}

// Leitura estavel no unico SIG
int lerEstavel() {
  analogRead(SIG);
  delayMicroseconds(50);
  return analogRead(SIG);
}

// Desliga todas as linhas, deixando em alta impedancia
void desativarTodasAsLinhas() {
  for (int i = 0; i < 6; i++) {
    pinMode(linhas1[i], INPUT);
    pinMode(linhas2[i], INPUT);
  }
}

// Ativa somente uma linha da matriz 1
void ativarLinhaMatriz1(int linha) {
  desativarTodasAsLinhas();
  pinMode(linhas1[linha], OUTPUT);
  digitalWrite(linhas1[linha], HIGH);
}

// Ativa somente uma linha da matriz 2
void ativarLinhaMatriz2(int linha) {
  desativarTodasAsLinhas();
  pinMode(linhas2[linha], OUTPUT);
  digitalWrite(linhas2[linha], HIGH);
}

void calibrarBaseline() {
  Serial.println("Calibrando baseline... NAO PRESSIONE OS SENSORES");
  delay(1000);

  for (int l = 0; l < 6; l++) {
    // ---- Matriz 1: linha l ----
    ativarLinhaMatriz1(l);
    delayMicroseconds(500);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM1[c]);
      delayMicroseconds(100);
      baseline1[l][c] = lerEstavel();
    }

    // ---- Matriz 2: linha l ----
    ativarLinhaMatriz2(l);
    delayMicroseconds(500);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM2[c]);
      delayMicroseconds(100);
      baseline2[l][c] = lerEstavel();
    }

    desativarTodasAsLinhas();
  }

  Serial.println("Baseline capturado com sucesso!");
  calibrado = true;
}

void setup() {
  Serial.begin(115200);

  analogReadResolution(12); // 0 a 4095
  analogSetAttenuation(ADC_11db);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  desativarTodasAsLinhas();

  calibrarBaseline();
}

void loop() {
  if (!calibrado) return;

  int m1[6][6];
  int m2[6][6];

  for (int l = 0; l < 6; l++) {
    // ======================================================
    // Intercalado por linha:
    // 1) Liga linha l da matriz 1 e le as 6 colunas dela
    // 2) Liga linha l da matriz 2 e le as 6 colunas dela
    // ======================================================

    // ---- Matriz 1: linha l ----
    ativarLinhaMatriz1(l);
    delayMicroseconds(300);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM1[c]);   // canais 0 a 5
      delayMicroseconds(80);

      int val1 = lerEstavel();
      int d1 = val1 - baseline1[l][c];

      if (d1 < 0) d1 = 0;
      m1[l][c] = (d1 < THRESHOLD) ? 0 : d1;
    }

    // ---- Matriz 2: linha l ----
    ativarLinhaMatriz2(l);
    delayMicroseconds(300);

    for (int c = 0; c < 6; c++) {
      setCanal(canaisM2[c]);   // canais C7 a C12
      delayMicroseconds(80);

      int val2 = lerEstavel();
      int d2 = val2 - baseline2[l][c];

      if (d2 < 0) d2 = 0;
      m2[l][c] = (d2 < THRESHOLD2) ? 0 : d2;
    }

    desativarTodasAsLinhas();
  }

  // Visualizacao no Serial Monitor
  Serial.println(">> MATRIZ 1 | MATRIZ 2 <<");
  for (int l = 0; l < 6; l++) {
    // Exibe M1
    for (int c = 5; c >= 0; c--) {
      Serial.print(m1[l][c]);
      Serial.print("\t");
    }

    Serial.print(" |   ");

    // Exibe M2
    for (int c = 0; c < 6; c++) {
      Serial.print(m2[l][c]);
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println("-------------------------------------------------");

  delay(50);
}
