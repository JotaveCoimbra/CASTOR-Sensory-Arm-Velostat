int linhas[6]  = {25,26,27,14,12,13};  // OUTPUT
int colunas[6] = {33,32,35,34,39,36};  // ADC

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 6; i++) {
    pinMode(linhas[i], OUTPUT);
    digitalWrite(linhas[i], LOW);  // começa tudo em LOW
  }
}

void loop() {

  int matriz[6][6];  // buffer dos valores

  for (int l = 0; l < 6; l++) {

    // ativa só essa linha
    digitalWrite(linhas[l], HIGH);
    delayMicroseconds(50);

    for (int c = 0; c < 6; c++) {
      matriz[l][c] = analogRead(colunas[c]);
    }

    digitalWrite(linhas[l], LOW);
  }

  // ------------ PRINT BONITO EM FORMATO DE MATRIZ (COLUNAS INVERTIDAS) ------------
  Serial.println("----- MATRIZ -----");
  for (int l = 0; l < 6; l++) {
    for (int c = 5; c >= 0; c--) {   // <<< AQUI: imprime colunas de trás pra frente
      Serial.print(matriz[l][c]);
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println("------------------");
  Serial.println();

  delay(200);   // taxa de atualização
}
