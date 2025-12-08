int linhas[6]  = {25,26,27,14,12,13};
int colunas[6] = {33,32,35,34,39,36};

void setup() {
  Serial.begin(115200);

  // No setup, pode até deixar tudo como INPUT inicialmente
  for (int i = 0; i < 6; i++) {
    pinMode(linhas[i], INPUT);  // alta impedância
  }
}

void loop() {

  int matriz[6][6];

  for (int l = 0; l < 6; l++) {

    // 1) todas as linhas em alta impedância
    for (int i = 0; i < 6; i++) {
      pinMode(linhas[i], INPUT);
    }

    // 2) ativa só essa linha como saída em HIGH
    pinMode(linhas[l], OUTPUT);
    digitalWrite(linhas[l], HIGH);

    delayMicroseconds(200); // dá tempo estabilizar

    // 3) lê todas as colunas
    for (int c = 0; c < 6; c++) {
      matriz[l][c] = analogRead(colunas[c]);
    }

    // opcional: voltar essa linha pra INPUT aqui
    pinMode(linhas[l], INPUT);
  }

  // imprime a matriz
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
