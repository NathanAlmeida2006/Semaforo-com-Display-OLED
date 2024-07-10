# Projeto de Semáforo com Display OLED e Buzzer

Este projeto visa implementar um sistema de semáforo utilizando um Arduino, LEDs, um display OLED e um buzzer. O sistema controla o fluxo de tráfego e pedestres, alternando entre estados de sinal verde, amarelo e vermelho para carros e pedestres. Além disso, possui modos intermitentes para avisos e contagem regressiva no display OLED.

## Componentes Necessários

- Arduino (qualquer modelo compatível)
- LEDs (vermelho, amarelo e verde para carros; vermelho e verde para pedestres)
- Resistores (adequados para LEDs)
- Display OLED (Adafruit SSD1306)
- Buzzer
- Botões de pressão (dois)
- Jumpers e protoboard
- Fios de conexão

## Esquema de Ligação

### Pinos Utilizados

- **Carros Verde:** Pino 13
- **Carros Amarelo:** Pino 12
- **Carros Vermelho:** Pino 11
- **Pedestre Verde:** Pino 10
- **Pedestre Vermelho:** Pino 9
- **LED Intermitente:** Pino 8
- **Botão Intermitente:** Pino 2
- **Botão Pedestre:** Pino 3
- **Buzzer:** Pino 4
- **Display OLED:** Pinos I2C (SDA, SCL)

### Conexão Básica

1. Conecte os LEDs aos pinos correspondentes do Arduino, cada um com seu resistor.
2. Conecte os botões de pressão nos pinos 2 e 3, utilizando resistores de pull-up internos.
3. Conecte o buzzer ao pino 4.
4. Conecte o display OLED aos pinos I2C do Arduino (SDA, SCL).

## Funcionamento do Código

### Estrutura Básica

O código está estruturado com as seguintes funcionalidades principais:

1. **Configuração dos Pinos e Inicialização:**
   - Configura os pinos como entradas ou saídas.
   - Inicializa o display OLED.
   - Define o estado inicial do semáforo.

2. **Estados do Semáforo:**
   - **CARROS_VERDE:** Sinal verde para carros e vermelho para pedestres.
   - **CARROS_AMARELO:** Sinal amarelo para carros, preparando para o vermelho.
   - **CARROS_VERMELHO_PEDESTRE_VERDE:** Sinal vermelho para carros e verde para pedestres.
   - **INTERMITENTE:** Modo intermitente para avisos.

3. **Controle de Tempo e Mudança de Estados:**
   - Utiliza `millis()` para controlar a duração de cada estado.
   - Muda de estado conforme os tempos pré-definidos ou quando um botão é pressionado.

4. **Exibição no Display OLED:**
   - Mostra mensagens de "ANDE" ou "NÃO ANDE".
   - Realiza contagem regressiva quando o sinal está verde para pedestres.

5. **Buzzer:**
   - Emite sons nos últimos segundos do sinal verde para pedestres.

### Funções Principais

- **setup()**
  - Configura os pinos e inicializa os componentes.
- **loop()**
  - Verifica os botões e atualiza os estados do semáforo conforme necessário.
- **inicializarSemaforo()**
  - Define o estado inicial dos LEDs e mostra a mensagem "NÃO ANDE".
- **alternarEstadoIntermitente()**
  - Alterna entre o modo intermitente e o modo normal.
- **mudarParaEstado(Estado novoEstado, unsigned long tempoAtual)**
  - Muda para o novo estado especificado.
- **apagarTodosOsLeds()**
  - Apaga todos os LEDs.
- **piscarLedIntermitente(unsigned long tempoAtual)**
  - Pisca o LED intermitente no intervalo especificado.
- **mostrarMensagem(const char mensagem)**
  - Mostra uma mensagem centralizada no display OLED.
- **mostrarContagemRegressiva(unsigned long segundosRestantes)**
  - Mostra a contagem regressiva no display OLED.

## Como Usar

1. Monte o circuito conforme o esquema.
2. Carregue o código no Arduino.
3. O semáforo iniciará no estado verde para carros.
4. Pressione o botão de pedestres para solicitar a travessia.
5. Pressione o botão intermitente para alternar para o modo de aviso intermitente.
6. Observe as mudanças de estado e as mensagens no display OLED.

## Visualização do Circuito 

Link para acesso: 
- https://wokwi.com/projects/401054814407529473

![image](https://github.com/NathanAlmeida2006/Semaforo-com-Display-OLED/assets/65135206/43703bb0-1830-416b-b797-fed306820e5a)


## Contato:

Se você tiver alguma dúvida ou sugestão, sinta-se à vontade para abrir uma issue no repositório do GitHub ou enviar um e-mail para: 
[nathanfelipi2006@gmail.com].

---

Desenvolvido com ❤️ por [Nathan Felipi Almeida].


