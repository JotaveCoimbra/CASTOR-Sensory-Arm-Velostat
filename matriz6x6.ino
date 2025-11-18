int linhas[6]  = {25,26,27,14,12,13};          // OUTPUT
int colunas[6] = {33,32,35,34,39,36};         // ADC

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 6; i++) {
    pinMode(linhas[i], OUTPUT);
    digitalWrite(linhas[i], LOW);  // começa tudo em LOW
  }
  // colunas: não precisa pinMode pra analogRead
}

void loop() {
  for (int l = 0; l < 6; l++) {

    // ativa só essa linha
    digitalWrite(linhas[l], HIGH);
    delayMicroseconds(150);  // dá tempo estabilizar

    for (int c = 0; c < 6; c++) {
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
