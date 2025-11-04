int linhas[2]={27,14};
int colunas[2] = {25,26};

void setup() {
 for (int i = 0; i<2;i++){
  pinMode(linhas[i],OUTPUT);
  pinMode(colunas[i],INPUT);
 }
 Serial.begin(115200);
}

void loop() {
for (int l = 0; l<2;l++){
  digitalWrite(linhas[l],HIGH);
    for(int c = 0; c<2; c++){
      float valor = analogRead(colunas[c]);
      Serial.print("Linha ");
      Serial.print(l);
      Serial.print(" | Coluna ");
      Serial.print(c);
      Serial.print(" = ");
      Serial.println(valor);
    }
  delay(100);
  digitalWrite(linhas[l],LOW); 
}
Serial.println("---------");
delay(2000);
}
