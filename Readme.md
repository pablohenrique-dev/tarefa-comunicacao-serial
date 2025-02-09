# Sistema de Interrupções e Controle de LEDs com BitDogLab

Este projeto explora o uso de interrupções no microcontrolador RP2040 e a funcionalidade da placa BitDogLab para manipular LEDs comuns e LEDs endereçáveis WS2812.

## 🔧 Componentes Utilizados

- **Placa de desenvolvimento**: BitDogLab (RP2040)
- **Matriz de LEDs WS2812**: 5x5 (endereçáveis) conectada à GPIO 7
- **LED RGB**: conectado às GPIOs 11, 12 e 13
- **Botões**:
  - **Botão A**: conectado à GPIO 5
  - **Botão B**: conectado à GPIO 6
- **Display SSD1306**: conectado via I2C nas GPIOs 14 e 15

## 🛠️ Funcionamento do Sistema

1. **Modificação da Biblioteca `font.h`**
   - Adicionar caracteres minúsculos personalizados à biblioteca `font.h`.
2. **Entrada de Caracteres via PC**

   - Cada caractere digitado no Serial Monitor do VS Code será exibido no display SSD1306.
   - Se um número entre 0 e 9 for digitado, um símbolo correspondente será exibido na matriz de LEDs WS2812.

3. **Interação com Botões**
   - **Botão A**: alterna o estado do LED RGB Verde (liga/desliga).
   - **Botão B**: alterna o estado do LED RGB Azul (liga/desliga).
   - As ações dos botões são registradas de duas formas:
     - Uma mensagem informativa no display SSD1306.
     - Um texto descritivo enviado ao Serial Monitor.

## 📜 Implementação

O código utiliza:

- **Interrupções (IRQ)** para capturar eventos de pressionamento dos botões.
- **Debounce via software** para evitar leituras incorretas dos botões.
- **Uso de resistores de pull-up internos** para os botões de acionamento.
- **Comunicação UART** para entrada de caracteres via Serial Monitor.
- **Protocolo I2C** para comunicação com o display SSD1306.

## Como Rodar o Projeto

### Pré-requisitos:

- **Extensão Raspberry Pi Pico** instalada no ambiente de desenvolvimento.
- **Extensão Wokwi** para simulação no VSCode.

### Passos para Compilar e Executar o Código:

1. **Compilar o código**:

   - Clique no botão **Compile** na parte inferior da tela.
   - Após a compilação, abra o arquivo `diagram.json`.

2. **Simulação**:

   - Execute a simulação para visualizar o comportamento dos LEDs, botões e do display no ambiente virtual do Wokwi.

3. **Executar na Placa BitDogLab**:
   - Coloque a placa em **modo BOOTSEL** (mantenha pressionado o botão BOOTSEL enquanto conecta ao computador).
   - Transfira o código compilado para a placa e execute-o.

Agora seu projeto está pronto para explorar o funcionamento de interrupções, comunicação serial e controle de LEDs com a BitDogLab! 🚀

## 📺 Vídeo sobre o projeto

[Link do vídeo](https://www.youtube.com/watch?v=rTc49sQCXto)
