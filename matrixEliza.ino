int pinMatrix1 = 32;
int Matrixread1 = 0;
int pinMatrix2 = 34;
int Matrixread2 = 0;
void setup() {
  pinMode(pinMatrix1,INPUT);
  pinMode(pinMatrix2,INPUT);
  Serial.begin(115200);

}

void loop() {
  Matrixread1 = analogRead(pinMatrix1);
  Matrixread2 = analogRead(pinMatrix2);
  if(Matrixread1 > 3000){
    Serial.println("Tocou matriz 1!");
  }
    if(Matrixread2 > 3000){
    Serial.println("Tocou matriz 2!");
  }
  delay(200);

}