int peEs = 4;  
int peDi = 27; 

int leitura1 = 0;
int leitura2 = 0;

const int amostrasCalibracao = 100;
const int amostrasMedia = 20;

// Como seu ruído parado chega perto de 200,
// deixe o limiar acima disso.
const int limiar = 300;

// Baseline como float para permitir ajuste suave
float baselinePe = 0;
float baselinePd = 0;

// Quanto menor, mais devagar o baseline acompanha a deriva
const float alphaBaseline = 0.01;

void setup() {
  Serial.begin(115200);

  // No ESP32, isso ajuda a padronizar a leitura
  analogReadResolution(12);

  // Se sua placa aceitar, pode usar:
  // analogSetAttenuation(ADC_11db);

  baselinePe = calibracao(peEs, amostrasCalibracao);
  baselinePd = calibracao(peDi, amostrasCalibracao);
}

void loop() {
  int brutoPe = mediaAnalogica(peEs, amostrasMedia);
  int brutoPd = mediaAnalogica(peDi, amostrasMedia);

  int deltaPe = brutoPe - baselinePe;
  int deltaPd = brutoPd - baselinePd;

  // Pé esquerdo
  if (deltaPe < limiar) {
    leitura1 = 0;

    // Atualiza o baseline lentamente quando considera que NÃO há toque
    baselinePe = baselinePe * (1.0 - alphaBaseline) + brutoPe * alphaBaseline;
  } else {
    leitura1 = deltaPe;
  }

  // Pé direito
  if (deltaPd < limiar) {
    leitura2 = 0;

    // Atualiza o baseline lentamente quando considera que NÃO há toque
    baselinePd = baselinePd * (1.0 - alphaBaseline) + brutoPd * alphaBaseline;
  } else {
    leitura2 = deltaPd;
  }

  Serial.print("Esquerdo:");
  Serial.print(leitura1);
  Serial.print("\tDireito:");
  Serial.println(leitura2);



  delay(100); 
}

int calibracao(int pe, int amostras) {
  long soma = 0;

  for (int i = 0; i < amostras; i++) {
    soma += analogRead(pe);
    delay(5);
  }

  return soma / amostras;
}

int mediaAnalogica(int pe, int amostras) {
  long soma = 0;

  // Leitura descartada para estabilizar o ADC ao trocar de canal
  analogRead(pe);
  delayMicroseconds(300);

  for (int i = 0; i < amostras; i++) {
    soma += analogRead(pe);
    delayMicroseconds(500);
  }

  return soma / amostras;
}