int linhas[6]  = {25,26,27,14,12,13};
int colunas[6] = {33,32,35,34,39,36};

int baseline[6][6];   // matriz de offset
bool calibrado = false;

// ======================================================
// Calibra baseline automaticamente
// ======================================================
void calibrarBaseline() {

  Serial.println("Calibrando baseline...");
  delay(500);

  for (int l = 0; l < 6; l++) {

    for (int i = 0; i < 6; i++)
      pinMode(linhas[i], INPUT);  // alta impedância

    pinMode(linhas[l], OUTPUT);
    digitalWrite(linhas[l], HIGH);
    delayMicroseconds(300);

    for (int c = 0; c < 6; c++)
      baseline[l][c] = analogRead(colunas[c]);

    pinMode(linhas[l], INPUT);
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

  for (int i = 0; i < 6; i++)
    pinMode(linhas[i], INPUT);

  delay(1000);   // deixa roupa/espuma estabilizar
  calibrarBaseline();
}

// ======================================================
// Loop principal
// ======================================================
void loop() {

  if (!calibrado) return;

  int matriz[6][6];

  for (int l = 0; l < 6; l++) {

    // todas as linhas em alta impedância
    for (int i = 0; i < 6; i++)
      pinMode(linhas[i], INPUT);

    // ativa só essa linha
    pinMode(linhas[l], OUTPUT);
    digitalWrite(linhas[l], HIGH);
    delayMicroseconds(300);

    // leitura + DELTA (subtração do baseline)
    for (int c = 0; c < 6; c++) {
      int leitura = analogRead(colunas[c]);
      int delta = leitura - baseline[l][c];
      if (delta < 0) delta = 0;
      matriz[l][c] = delta;
    }

    pinMode(linhas[l], INPUT);
    delayMicroseconds(300);
  }

  // =================== IMPRESSÃO NO SEU FORMATO ===================
  Serial.println("----- MATRIZ -----");
  for (int l = 0; l < 6; l++) {
    for (int c = 5; c >= 0; c--) {
      Serial.print(matriz[l][c]);
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println("------------------\n");

  delay(200);
}
