int linhas[6]  = {25,26,27,14,12,13};
int colunas[6] = {33,32,35,34,39,36};

int baseline[6][6];
bool calibrado = false;

const int TH = 300;   

bool toque_ativo = false;   


void calibrarBaseline() {

  Serial.println("Calibrando baseline...");
  delay(500);

  for (int l = 0; l < 6; l++) {

    for (int i = 0; i < 6; i++)
      pinMode(linhas[i], INPUT);

    pinMode(linhas[l], OUTPUT);
    digitalWrite(linhas[l], HIGH);
    delayMicroseconds(300);

    for (int c = 0; c < 6; c++)
      baseline[l][c] = analogRead(colunas[c]);

    pinMode(linhas[l], INPUT);
  }

  Serial.println("Baseline capturado!");
  calibrado = true;
}


void setup() {
  Serial.begin(115200);
  calibrarBaseline();
}


void loop() {

  if (!calibrado) return;

  int matriz[6][6];
  bool algum_toque = false;

  // ===== LER MATRIZ + DELTA =====
  for (int l = 0; l < 6; l++) {

    for (int i = 0; i < 6; i++)
      pinMode(linhas[i], INPUT);

    pinMode(linhas[l], OUTPUT);
    digitalWrite(linhas[l], HIGH);

    for (int c = 0; c < 6; c++) {

      int leitura = analogRead(colunas[c]);
      int delta = leitura - baseline[l][c];
      if (delta < 0) delta = 0;

      matriz[l][c] = delta;

      
      if (delta > TH) {
        algum_toque = true;
      }
    }

    pinMode(linhas[l], INPUT);
  }

  if (algum_toque && !toque_ativo) {
    Serial.println("1");
    toque_ativo = true;
  }

  if (!algum_toque && toque_ativo) {
    Serial.println("0");
    toque_ativo = false;
  }

  delay(50);
}
