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
  
  delay(2000); // Aguardar inicialização
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
  // Trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Ler echo
  long duracao = pulseIn(ECHO_PIN, HIGH);
  
  // Calcular distância
  float distancia = (duracao * 0.034) / 2;
  
  // Converter para nível da água (sensor a 200cm do fundo)
  float nivel = 200.0 - distancia;
  
  // CORREÇÃO DO ERRO: Usar constantes do mesmo tipo
  if (nivel < 0.0) nivel = 0.0;
  if (nivel > 200.0) nivel = 200.0;
  
  return nivel;
}

int avaliarRisco(float nivel, float chuva, float taxa) {
  int pontos = 0;
  
  // Avaliar nível da água
  if (nivel > 150.0) {
    pontos += 3;
  } else if (nivel > 100.0) {
    pontos += 2;
  } else if (nivel > 80.0) {
    pontos += 1;
  }
  
  // Avaliar chuva
  if (chuva > 20.0) {
    pontos += 2;
  } else if (chuva > 10.0) {
    pontos += 1;
  }
  
  // Avaliar taxa de subida
  if (taxa > 10.0) {
    pontos += 2;
  } else if (taxa > 5.0) {
    pontos += 1;
  }
  
  // Classificar risco
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
      // Buzzer intermitente lento
      if ((millis() / 2000) % 2) {
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
      }
      break;
      
    case 2: // ALTO
      setLED(255, 0, 0); // Vermelho
      // Buzzer intermitente rápido
      if ((millis() / 500) % 2) {
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
      }
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
  
  Serial.print("Nivel da agua: "); 
  Serial.print(nivel, 1); 
  Serial.println(" cm");
  
  Serial.print("Precipitacao: "); 
  Serial.print(chuva, 1); 
  Serial.println(" mm/h");
  
  Serial.print("Taxa subida: "); 
  Serial.print(taxa, 1); 
  Serial.println(" cm/h");
  
  Serial.print("Timestamp: ");
  Serial.print(millis());
  Serial.println(" ms");
  
  // Mostrar status do risco
  Serial.print("RISCO: ");
  switch(risco) {
    case 0:
      Serial.println("BAIXO 🟢");
      Serial.println("Status: SITUACAO NORMAL");
      break;
    case 1:
      Serial.println("MEDIO 🟡");
      Serial.println("Status: ATENCAO REDOBRADA");
      break;
    case 2:
      Serial.println("ALTO 🔴");
      Serial.println("Status: ALERTA MAXIMO!");
      break;
  }
  
  Serial.println("=====================================");
  Serial.println();
}

// Função adicional para demonstrar funcionalidades
void demonstrarSistema() {
  Serial.println("🌊 DEMONSTRACAO DO SISTEMA 🌊");
  Serial.println("Sistema detecta mudancas no nivel da agua");
  Serial.println("e aciona alertas automaticamente!");
  Serial.println("Verde = Normal | Amarelo = Atencao | Vermelho = Perigo");
  Serial.println();
}
