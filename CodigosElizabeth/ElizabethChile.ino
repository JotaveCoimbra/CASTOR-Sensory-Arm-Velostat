#include <Arduino.h>

// ==========================
// Configurações de Threshold
// ==========================
const int THRESHOLD = 200; // Qualquer leitura abaixo de 200 será zerada

// ==========================
// Linhas separadas (12 pinos)
// ==========================
int linhas1[6] = {16, 17, 18, 19, 21, 22};       // Matriz 1
int linhas2[6] = {4, 23, 5, 25, 26, 27};          // Matriz 2

// ==========================
// MUX 74HC4067 (S0..S3) compartilhados
// ==========================
const int S0 = 32;
const int S1 = 33;
const int S2 = 14;
const int S3 = 12;

// COM de cada mux (um por matriz)
const int SIG1 = 34;   // Matriz 1 (ADC1 - Input Only)
const int SIG2 = 35;   // Matriz 2 (ADC1 - Input Only)

// Canais do 4067 usados nas 6 colunas
int canais[6] = {0, 1, 2, 3, 4, 5};

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

// Leitura estável
int lerEstavel(int pin) {
  analogRead(pin); 
  delayMicroseconds(50);
  return analogRead(pin);
}

void calibrarBaseline() {
  Serial.println("Calibrando baseline... NAO PRESSIONE OS SENSORES");
  delay(1000);

  for (int l = 0; l < 6; l++) {
    for (int i = 0; i < 6; i++) {
      pinMode(linhas1[i], INPUT);
      pinMode(linhas2[i], INPUT);
    }

    pinMode(linhas1[l], OUTPUT);
    digitalWrite(linhas1[l], HIGH);
    pinMode(linhas2[l], OUTPUT);
    digitalWrite(linhas2[l], HIGH);

    delayMicroseconds(500);

    for (int c = 0; c < 6; c++) {
      setCanal(canais[c]);
      delayMicroseconds(100);
      baseline1[l][c] = lerEstavel(SIG1);
      baseline2[l][c] = lerEstavel(SIG2);
    }

    pinMode(linhas1[l], INPUT);
    pinMode(linhas2[l], INPUT);
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

  for (int i = 0; i < 6; i++) {
    pinMode(linhas1[i], INPUT);
    pinMode(linhas2[i], INPUT);
  }

  calibrarBaseline();
}

void loop() {
  if (!calibrado) return;

  int m1[6][6];
  int m2[6][6];

  for (int l = 0; l < 6; l++) {
    // Desativa todas as linhas (Hi-Z)
    for (int i = 0; i < 6; i++) {
      pinMode(linhas1[i], INPUT);
      pinMode(linhas2[i], INPUT);
    }

    // Ativa a linha atual das duas matrizes
    pinMode(linhas1[l], OUTPUT);
    digitalWrite(linhas1[l], HIGH);
    pinMode(linhas2[l], OUTPUT);
    digitalWrite(linhas2[l], HIGH);

    delayMicroseconds(300);

    for (int c = 0; c < 6; c++) {
      setCanal(canais[c]);
      delayMicroseconds(80);

      int val1 = lerEstavel(SIG1);
      int val2 = lerEstavel(SIG2);

      // Diferença em relação a baseline
      int d1 = val1 - baseline1[l][c];
      int d2 = val2 - baseline2[l][c];

      // Garante que não existam valores negativos
      if (d1 < 0) d1 = 0;
      if (d2 < 0) d2 = 0;

      // Aplicação do Threshold de 200 
      if (d1 < THRESHOLD) {
        m1[l][c] = 0;
      } else {
        m1[l][c] = d1;
      }

      if (d2 < THRESHOLD) {
        m2[l][c] = 0;
      } else {
        m2[l][c] = d2;
      }
    }

    // Volta a linha para alta impedância
    pinMode(linhas1[l], INPUT);
    pinMode(linhas2[l], INPUT);
  }

  // Visualização no Serial Monitor
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
