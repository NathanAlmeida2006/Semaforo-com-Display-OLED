#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definição dos pinos para os LEDs dos carros e pedestres
#define CARRO_VERDE 13
#define CARRO_AMARELO 12
#define CARRO_VERMELHO 11
#define PEDESTRE_VERDE 10
#define PEDESTRE_VERMELHO 9
#define LED_INTERMITENTE 8
#define BOTAO_PUSH_INTERMITENTE 2
#define BOTAO_PUSH_PEDESTRE 3
#define BUZZER 4 

// Definição das durações dos estados em milissegundos
const unsigned long DURACAO_CARROS_VERDE = 10000;
const unsigned long DURACAO_CARROS_AMARELO = 3000;
unsigned long DURACAO_PEDESTRE_VERDE = 10000; // Duração atual do verde para pedestres
const unsigned long INTERVALO_INTERMITENTE = 500;
const unsigned long TEMPO_MAXIMO_PEDESTRE = 5000;
const unsigned long TEMPO_EXTRA_PEDESTRE = 1000;

// Configuração do display OLED
#define SCREEN_WIDTH 128 // Largura do display OLED em pixels
#define SCREEN_HEIGHT 64 // Altura do display OLED em pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Estados do semáforo
enum Estado {
  CARROS_VERDE,
  CARROS_AMARELO,
  CARROS_VERMELHO_PEDESTRE_VERDE,
  INTERMITENTE
};

Estado estadoAtual = CARROS_VERDE;
unsigned long tempoAnterior = 0;
unsigned long tempoIntermitenteAnterior = 0;
unsigned long tempoUltimoBip = 0;
bool estadoIntermitente = false;
bool botaoPressionadoAnteriorIntermitente = false;
bool botaoPressionadoAnteriorPedestre = false;
bool pedestreSolicitou = false;

/**
 * @brief Configura os pinos e inicializa o semáforo.
 */
void setup() {
  pinMode(CARRO_VERDE, OUTPUT);
  pinMode(CARRO_AMARELO, OUTPUT);
  pinMode(CARRO_VERMELHO, OUTPUT);
  pinMode(PEDESTRE_VERDE, OUTPUT);
  pinMode(PEDESTRE_VERMELHO, OUTPUT);
  pinMode(LED_INTERMITENTE, OUTPUT);
  pinMode(BOTAO_PUSH_INTERMITENTE, INPUT_PULLUP); 
  pinMode(BOTAO_PUSH_PEDESTRE, INPUT_PULLUP); 
  pinMode(BUZZER, OUTPUT);

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  inicializarSemaforo();
  tempoAnterior = millis();
}

/**
 * @brief Função principal que é executada continuamente.
 */
void loop() {
  unsigned long tempoAtual = millis();
  bool botaoPressionadoIntermitente = digitalRead(BOTAO_PUSH_INTERMITENTE) == LOW;
  bool botaoPressionadoPedestre = digitalRead(BOTAO_PUSH_PEDESTRE) == LOW;

  // Detecta a mudança de estado do botão intermitente (pressionado para não pressionado e vice-versa)
  if (botaoPressionadoIntermitente && !botaoPressionadoAnteriorIntermitente) {
    alternarEstadoIntermitente();
    tempoAnterior = tempoAtual; // Reseta o tempo anterior
  }
  botaoPressionadoAnteriorIntermitente = botaoPressionadoIntermitente;

  // Detecta a mudança de estado do botão pedestre (pressionado para não pressionado e vice-versa)
  if (botaoPressionadoPedestre && !botaoPressionadoAnteriorPedestre && estadoAtual == CARROS_VERDE) {
    if (tempoAtual - tempoAnterior <= TEMPO_MAXIMO_PEDESTRE) {
      pedestreSolicitou = true;
    }
  }
  botaoPressionadoAnteriorPedestre = botaoPressionadoPedestre;

  switch (estadoAtual) {
    case CARROS_VERDE:
      if (pedestreSolicitou && (tempoAtual - tempoAnterior >= (TEMPO_MAXIMO_PEDESTRE + TEMPO_EXTRA_PEDESTRE))) {
        mudarParaEstado(CARROS_AMARELO, tempoAtual);
        pedestreSolicitou = false;
      } else if (!pedestreSolicitou && (tempoAtual - tempoAnterior >= DURACAO_CARROS_VERDE)) {
        mudarParaEstado(CARROS_AMARELO, tempoAtual);
      }
      break;

    case CARROS_AMARELO:
      if (tempoAtual - tempoAnterior >= DURACAO_CARROS_AMARELO) {
        mudarParaEstado(CARROS_VERMELHO_PEDESTRE_VERDE, tempoAtual);
      }
      break;

    case CARROS_VERMELHO_PEDESTRE_VERDE:
      if (tempoAtual - tempoAnterior >= DURACAO_PEDESTRE_VERDE) {
        mudarParaEstado(CARROS_VERDE, tempoAtual);
      } else {
        // Atualiza o display para contagem regressiva
        unsigned long tempoRestante = DURACAO_PEDESTRE_VERDE - (tempoAtual - tempoAnterior);
        mostrarContagemRegressiva(tempoRestante / 1000);

        // Tocar o buzzer nos últimos 3 segundos
        if (tempoRestante <= 3000) {
          if (tempoAtual - tempoUltimoBip >= 1000) { // Apita a cada 1 segundo
            tone(BUZZER, 1000); // Toca o buzzer com frequência de 1000 Hz
            delay(100); // Toca por 100ms
            noTone(BUZZER); // Para o buzzer
            tempoUltimoBip = tempoAtual;
          }
        }
      }
      break;

    case INTERMITENTE:
      piscarLedIntermitente(tempoAtual);
      // Atualiza o display para "CUIDADO"
      mostrarMensagem("CUIDADO");
      break;
  }
}

/**
 * @brief Inicializa os LEDs do semáforo no estado inicial.
 */
void inicializarSemaforo() {
  digitalWrite(CARRO_VERDE, HIGH);
  digitalWrite(CARRO_AMARELO, LOW);
  digitalWrite(CARRO_VERMELHO, LOW);
  digitalWrite(PEDESTRE_VERDE, LOW);
  digitalWrite(PEDESTRE_VERMELHO, HIGH);
  digitalWrite(LED_INTERMITENTE, LOW);
  noTone(BUZZER); // Certifica-se de que o buzzer está desligado
  // Atualiza o display para "NÃO ANDE"
  mostrarMensagem("NAO ANDE");
}

/**
 * @brief Alterna para o modo intermitente ou retorna ao modo normal.
 */
void alternarEstadoIntermitente() {
  if (estadoAtual == INTERMITENTE) {
    estadoAtual = CARROS_VERDE; // Retorna ao estado inicial
    inicializarSemaforo();
  } else {
    estadoAtual = INTERMITENTE;
    // Apaga todos os LEDs do semáforo
    apagarTodosOsLeds();
  }
}

/**
 * @brief Muda o semáforo para um novo estado.
 *
 * @param novoEstado O novo estado para o qual o semáforo deve mudar.
 * @param tempoAtual O tempo atual em milissegundos.
 */
void mudarParaEstado(Estado novoEstado, unsigned long tempoAtual) {
  estadoAtual = novoEstado;
  tempoAnterior = tempoAtual;

  switch (novoEstado) {
    case CARROS_VERDE:
      inicializarSemaforo();
      break;
    case CARROS_AMARELO:
      digitalWrite(CARRO_VERDE, LOW);
      digitalWrite(CARRO_AMARELO, HIGH);
      break;
    case CARROS_VERMELHO_PEDESTRE_VERDE:
      digitalWrite(CARRO_AMARELO, LOW);
      digitalWrite(CARRO_VERMELHO, HIGH);
      digitalWrite(PEDESTRE_VERDE, HIGH);
      digitalWrite(PEDESTRE_VERMELHO, LOW);
      // Atualiza o display para "ANDE"
      mostrarMensagem("ANDE");
      break;
    default:
      break;
  }
}

/**
 * @brief Apaga todos os LEDs do semáforo.
 */
void apagarTodosOsLeds() {
  digitalWrite(CARRO_VERDE, LOW);
  digitalWrite(CARRO_AMARELO, LOW);
  digitalWrite(CARRO_VERMELHO, LOW);
  digitalWrite(PEDESTRE_VERDE, LOW);
  digitalWrite(PEDESTRE_VERMELHO, LOW);
  digitalWrite(LED_INTERMITENTE, LOW);
}

/**
 * @brief Pisca o LED intermitente.
 *
 * @param tempoAtual O tempo atual em milissegundos.
 */
void piscarLedIntermitente(unsigned long tempoAtual) {
  if (tempoAtual - tempoIntermitenteAnterior >= INTERVALO_INTERMITENTE) {
    tempoIntermitenteAnterior = tempoAtual;
    estadoIntermitente = !estadoIntermitente;
    digitalWrite(LED_INTERMITENTE, estadoIntermitente ? HIGH : LOW);
  }
}

/**
 * @brief Mostra uma mensagem no display OLED centralizada.
 *
 * @param mensagem A mensagem a ser mostrada.
 */
void mostrarMensagem(const char* mensagem) {
  display.clearDisplay();
  display.setTextSize(2); // Tamanho do texto
  display.setTextColor(SSD1306_WHITE); // Cor do texto

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(mensagem, 0, 0, &x1, &y1, &w, &h);

  // Calcula a posição para centralizar o texto
  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y); // Posição inicial do texto
  display.println(mensagem); // Escreve a mensagem
  display.display(); // Atualiza o display com a nova mensagem
}

/**
 * @brief Mostra a contagem regressiva no display OLED junto com a mensagem "ANDE".
 *
 * @param segundosRestantes O número de segundos restantes.
 */
void mostrarContagemRegressiva(unsigned long segundosRestantes) {
  display.clearDisplay();
  display.setTextSize(2); // Tamanho do texto
  display.setTextColor(SSD1306_WHITE); // Cor do texto

  char buffer[16];
  sprintf(buffer, "ANDE | %lu", segundosRestantes);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);

  // Calcula a posição para centralizar o texto
  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y); // Posição inicial do texto
  display.println(buffer); // Escreve a contagem regressiva
  display.display(); // Atualiza o display com a nova contagem
}
