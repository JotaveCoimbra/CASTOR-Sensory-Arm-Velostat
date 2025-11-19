// --- Pinos da sua matriz ---
int linhas[6]  = {25,26,27,14,12,13};     // Todas as linhas possíveis
int colunas[6] = {33,32,35,34,39,36};     // Todas as colunas possíveis

// --- ESCOLHA AQUI O PONTO QUE QUER LER ---
int linhaEscolhida = 25;    // GPIO da linha que você quer ativar
int colunaEscolhida = 35;   // GPIO da coluna que você quer ler

// --------------------------------------------------------------

void setup() {
  Serial.begin(115200);

  // Configura todas as linhas como saída e zera
  for (int i = 0; i < 6; i++) {
    pinMode(linhas[i], OUTPUT);
    digitalWrite(linhas[i], LOW);
  }

  // Configura todas as colunas como entrada analógica (não precisa pinMode)
  
  Serial.println("Lendo o ponto selecionado no código...");
}

int lerPonto(int l, int c) {

  // Garante que nenhuma outra linha esteja alta
  for (int i = 0; i < 6; i++)
    digitalWrite(linhas[i], LOW);

  digitalWrite(l, HIGH);
  delayMicroseconds(150);   // estabilizar o divisor

  int valor = analogRead(c);

  digitalWrite(l, LOW);

  return valor;
}

void loop() {

  int leitura = lerPonto(linhaEscolhida, colunaEscolhida);

  Serial.print("Ponto (Linha GPIO ");
  Serial.print(linhaEscolhida);
  Serial.print(" → Coluna GPIO ");
  Serial.print(colunaEscolhida);
  Serial.print(") = ");
  Serial.println(leitura);

  delay(100);   // pode diminuir/ajustar a taxa
}
