#define TRIG_PIN 2
#define ECHO_PIN 4
#define LED_RED 12
#define LED_GREEN 13
#define LED_BLUE 14
#define BUZZER_PIN 15

float nivel_agua = 0;
int risco_atual = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  Serial.println("=== SISTEMA DE ENCHENTES ATIVO ===");
  setLED(0, 255, 0); // Verde inicial
}

void loop() {
  // 1. Ler sensor ultrassônico
  nivel_agua = lerSensor();
  
  // 2. Simular outros dados
  float precipitacao = random(0, 30);
  float taxa_subida = random(-5, 15);
  
  // 3. Avaliar risco (algoritmo simplificado)
  risco_atual = avaliarRisco(nivel_agua, precipitacao, taxa_subida);
  
  // 4. Atualizar alertas
  atualizarAlertas(risco_atual);
  
  // 5. Mostrar dados
  mostrarDados(nivel_agua, precipitacao, taxa_subida, risco_atual);
  
  delay(3000); // A cada 3 segundos
}

float lerSensor() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duracao = pulseIn(ECHO_PIN, HIGH);
  float distancia = (duracao * 0.034) / 2;
  float nivel = 200 - distancia; // Assumindo sensor a 200cm do fundo
  
  return max(0.0, min(200.0, nivel));
}

int avaliarRisco(float nivel, float chuva, float taxa) {
  int pontos = 0;
  
  // Avaliar nível da água
  if (nivel > 150) pontos += 3;
  else if (nivel > 100) pontos += 2;
  else if (nivel > 80) pontos += 1;
  
  // Avaliar chuva
  if (chuva > 20) pontos += 2;
  else if (chuva > 10) pontos += 1;
  
  // Avaliar taxa de subida
  if (taxa > 10) pontos += 2;
  else if (taxa > 5) pontos += 1;
  
  // Classificar
  if (pontos >= 5) return 2; // ALTO
  else if (pontos >= 2) return 1; // MÉDIO
  else return 0; // BAIXO
}

void atualizarAlertas(int risco) {
  switch (risco) {
    case 0: // BAIXO
      setLED(0, 255, 0); // Verde
      digitalWrite(BUZZER_PIN, LOW);
      break;
    case 1: // MÉDIO
      setLED(255, 255, 0); // Amarelo
      if ((millis() / 1000) % 2) digitalWrite(BUZZER_PIN, HIGH);
      else digitalWrite(BUZZER_PIN, LOW);
      break;
    case 2: // ALTO
      setLED(255, 0, 0); // Vermelho
      if ((millis() / 500) % 2) digitalWrite(BUZZER_PIN, HIGH);
      else digitalWrite(BUZZER_PIN, LOW);
      break;
  }
}

void setLED(int r, int g, int b) {
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}

void mostrarDados(float nivel, float chuva, float taxa, int risco) {
  Serial.println("=====================================");
  Serial.println("    MONITORAMENTO DE ENCHENTES");
  Serial.println("=====================================");
  Serial.print("Nível da água: "); Serial.print(nivel); Serial.println(" cm");
  Serial.print("Precipitação: "); Serial.print(chuva); Serial.println(" mm/h");
  Serial.print("Taxa subida: "); Serial.print(taxa); Serial.println(" cm/h");
  
  String niveis[] = {"BAIXO 🟢", "MÉDIO 🟡", "ALTO 🔴"};
  Serial.print("RISCO: "); Serial.println(niveis[risco]);
  Serial.println("=====================================");
}
