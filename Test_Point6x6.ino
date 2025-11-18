int linhas[6]  = {25,26,27,14,12,13};     
int colunas[6] = {33,32,35,34,39,36};

int linhaSelecionada = -1;
int colunaSelecionada = -1;
bool monitorando = false;

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 6; i++) {
    pinMode(linhas[i], OUTPUT);
    digitalWrite(linhas[i], LOW);
  }

  Serial.println();
  Serial.println("Digite: linha coluna   (ex: 3 5)");
  Serial.println("Digite Q para parar.");
  Serial.println("----------------------------------");
}

int lerPonto(int l, int c) {

  for (int i = 0; i < 6; i++)
    digitalWrite(linhas[i], LOW);

  digitalWrite(linhas[l], HIGH);
  delayMicroseconds(150);

  int valor = analogRead(colunas[c]);

  digitalWrite(linhas[l], LOW);

  return valor;
}

void loop() {

  // --- CHECA SE CHEGOU COMANDO ---
  if (Serial.available() > 0) {

    String entrada = Serial.readStringUntil('\n');
    entrada.trim();

    // ------ COMANDO Q ------
    if (entrada.equalsIgnoreCase("Q")) {
      monitorando = false;
      linhaSelecionada = -1;
      colunaSelecionada = -1;
      Serial.println("Monitoramento parado. Digite outro ponto.");
      return;
    }

    // ------ COMANDO LINHA COLUNA ------
    int espaco = entrada.indexOf(' ');
    if (espaco != -1) {
      int l = entrada.substring(0, espaco).toInt();
      int c = entrada.substring(espaco + 1).toInt();

      if (l >= 0 && l < 6 && c >= 0 && c < 6) {

        linhaSelecionada = l;
        colunaSelecionada = c;
        monitorando = true;

        Serial.print("Monitorando o ponto (");
        Serial.print(l);
        Serial.print(",");
        Serial.print(c);
        Serial.println(")");
      } else {
        Serial.println("Entrada inválida. Use: linha coluna   (0 a 5)");
      }
    } else {
      Serial.println("Formato inválido. Exemplo: 3 5");
    }
  }

  // --- PRINTA O PONTO EM LOOP ---
  if (monitorando) {
    int leitura = lerPonto(linhaSelecionada, colunaSelecionada);

    Serial.print("Ponto (");
    Serial.print(linhaSelecionada);
    Serial.print(",");
    Serial.print(colunaSelecionada);
    Serial.print(") = ");
    Serial.println(leitura);

    delay(100);
  }
}
