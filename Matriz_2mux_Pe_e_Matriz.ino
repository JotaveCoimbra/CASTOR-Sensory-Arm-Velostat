#include <Arduino.h>

// ==========================
// Thresholds
// ==========================
const int THRESHOLD  = 500;
const int THRESHOLD2 = 400;

// ==========================
// MUX DAS LINHAS
// SIG/COM desse mux no 3V3
// ==========================
const int L_S0 = 16;
const int L_S1 = 17;
const int L_S2 = 18;
const int L_S3 = 19;

int canaisLinhaM1[6] = {0, 1, 2, 3, 4, 5};
int canaisLinhaM2[6] = {7, 8, 9, 10, 11, 12};

const int CANAL_PE_ESQ = 14;
const int CANAL_PE_DIR = 15;

// ==========================
// MUX DAS COLUNAS
// SIG/COM desse mux no ADC
// ==========================
const int C_S0 = 32;
const int C_S1 = 33;
const int C_S2 = 14;
const int C_S3 = 12;

const int SIG_COL = 34; // ADC

int canaisColunaM1[6] = {0, 1, 2, 3, 4, 5};
int canaisColunaM2[6] = {7, 8, 9, 10, 11, 12};

// ==========================
// Baselines
// ==========================
int baseline1[6][6];
int baseline2[6][6];

int baselinePeEsquerdo = 0;
int baselinePeDireito  = 0;

bool calibrado = false;

// ==========================
// Seleciona canal do MUX das linhas
// ==========================
void setCanalLinha(int ch) {
  digitalWrite(L_S0, (ch & 1) ? HIGH : LOW);
  digitalWrite(L_S1, (ch & 2) ? HIGH : LOW);
  digitalWrite(L_S2, (ch & 4) ? HIGH : LOW);
  digitalWrite(L_S3, (ch & 8) ? HIGH : LOW);
}

// ==========================
// Seleciona canal do MUX das colunas
// ==========================
void setCanalColuna(int ch) {
  digitalWrite(C_S0, (ch & 1) ? HIGH : LOW);
  digitalWrite(C_S1, (ch & 2) ? HIGH : LOW);
  digitalWrite(C_S2, (ch & 4) ? HIGH : LOW);
  digitalWrite(C_S3, (ch & 8) ? HIGH : LOW);
}

// ==========================
// Leitura estável no ADC
// ==========================
int lerEstavel() {
  analogRead(SIG_COL);
  delayMicroseconds(50);
  return analogRead(SIG_COL);
}

// ==========================
// Ativa linha das matrizes
// ==========================
void ativarLinhaMatriz1(int linha) {
  setCanalLinha(canaisLinhaM1[linha]);
  delayMicroseconds(300);
}

void ativarLinhaMatriz2(int linha) {
  setCanalLinha(canaisLinhaM2[linha]);
  delayMicroseconds(300);
}

// ==========================
// Leitura dos pés
// Seleciona o mesmo canal nos dois MUX
// ==========================
int lerPe(int canalPe) {
  setCanalLinha(canalPe);
  setCanalColuna(canalPe);
  delayMicroseconds(300);
  return lerEstavel();
}

// ==========================
// Baseline dos pés por média
// ==========================
int calibrarPe(int canalPe) {
  long soma = 0;

  for (int i = 0; i < 30; i++) {
    soma += lerPe(canalPe);
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

  for (int l = 0; l < 6; l++) {
    // Matriz 1
    ativarLinhaMatriz1(l);

    for (int c = 0; c < 6; c++) {
      setCanalColuna(canaisColunaM1[c]);
      delayMicroseconds(100);
      baseline1[l][c] = lerEstavel();
    }

    // Matriz 2
    ativarLinhaMatriz2(l);

    for (int c = 0; c < 6; c++) {
      setCanalColuna(canaisColunaM2[c]);
      delayMicroseconds(100);
      baseline2[l][c] = lerEstavel();
    }
  }

  // Baseline dos pés
  baselinePeEsquerdo = calibrarPe(CANAL_PE_ESQ);
  baselinePeDireito  = calibrarPe(CANAL_PE_DIR);

  Serial.print("Baseline pe esquerdo: ");
  Serial.println(baselinePeEsquerdo);

  Serial.print("Baseline pe direito: ");
  Serial.println(baselinePeDireito);

  Serial.println("Baseline capturado com sucesso!");
  calibrado = true;
}

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  pinMode(L_S0, OUTPUT);
  pinMode(L_S1, OUTPUT);
  pinMode(L_S2, OUTPUT);
  pinMode(L_S3, OUTPUT);

  pinMode(C_S0, OUTPUT);
  pinMode(C_S1, OUTPUT);
  pinMode(C_S2, OUTPUT);
  pinMode(C_S3, OUTPUT);

  pinMode(SIG_COL, INPUT);

  calibrarBaseline();
}

void loop() {
  if (!calibrado) return;

  int m1[6][6];
  int m2[6][6];

  for (int l = 0; l < 6; l++) {

    // ==========================
    // Matriz 1
    // ==========================
    ativarLinhaMatriz1(l);

    for (int c = 0; c < 6; c++) {
      setCanalColuna(canaisColunaM1[c]);
      delayMicroseconds(80);

      int val = lerEstavel();
      int d = val - baseline1[l][c];

      if (d < 0) d = 0;
      m1[l][c] = (d < THRESHOLD) ? 0 : d;
    }

    // ==========================
    // Matriz 2
    // ==========================
    ativarLinhaMatriz2(l);

    for (int c = 0; c < 6; c++) {
      setCanalColuna(canaisColunaM2[c]);
      delayMicroseconds(80);

      int val = lerEstavel();
      int d = val - baseline2[l][c];

      if (d < 0) d = 0;
      m2[l][c] = (d < THRESHOLD2) ? 0 : d;
    }
  }

  // ==========================
  // Pés com baseline subtraída
  // ==========================
  int peEsquerdoRaw = lerPe(CANAL_PE_ESQ);
  int peDireitoRaw  = lerPe(CANAL_PE_DIR);

  int peEsquerdo = peEsquerdoRaw - baselinePeEsquerdo;
  int peDireito  = peDireitoRaw - baselinePeDireito;

  if (peEsquerdo < 0) peEsquerdo = 0;
  if (peDireito < 0) peDireito = 0;

  // ==========================
  // Serial
  // ==========================
  Serial.println(">> MATRIZ 1 | MATRIZ 2 <<");

  for (int l = 0; l < 6; l++) {
    for (int c = 5; c >= 0; c--) {
      Serial.print(m1[l][c]);
      Serial.print("\t");
    }

    Serial.print(" |   ");

    for (int c = 0; c < 6; c++) {
      Serial.print(m2[l][c]);
      Serial.print("\t");
    }

    Serial.println();
  }

  Serial.println("-------------------------");

  Serial.print("PE ESQUERDO: ");
  Serial.println(peEsquerdo);

  Serial.print("PE DIREITO:  ");
  Serial.println(peDireito);

  Serial.println("-------------------------");

  delay(50);
}