int linhas[8]  = {25,26,27,14,12,13,4,0};          // OUTPUT
int colunas[8] = {32,33,34,35,36,39,2,15};         // ADC

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 8; i++) {
    pinMode(linhas[i], OUTPUT);
    digitalWrite(linhas[i], LOW);  // começa tudo em LOW
  }
  // colunas: não precisa pinMode pra analogRead
}

void loop() {
  for (int l = 0; l < 8; l++) {

    // ativa só essa linha
    digitalWrite(linhas[l], HIGH);
    delayMicroseconds(150);  // dá tempo estabilizar

    for (int c = 0; c < 8; c++) {
      int valor = analogRead(colunas[c]);
      Serial.print("Linha ");
      Serial.print(l);
      Serial.print(" | Coluna ");
      Serial.print(c);
      Serial.print(" = ");
      Serial.println(valor);
    }

    digitalWrite(linhas[l], LOW);
    delay(50);
  }

  Serial.println("---------");
  delay(300);
}
