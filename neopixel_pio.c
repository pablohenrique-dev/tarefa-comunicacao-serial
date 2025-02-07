#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#include "ws2818b.pio.h"

#include "numeros/desenhos.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define led_pin_g 11
#define led_pin_b 12
#define led_pin_r 13

#define PIN_BOTAO_A 5 // Define o pino do botão A
#define PIN_BOTAO_B 6 // Define o pino do botão B

// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

// Variáveis globais
static volatile int contador = 0;         // Variável para armazenar o número (0 a 9)
static volatile uint32_t last_time_A = 0; // Armazena o tempo do último evento para o botão A
static volatile uint32_t last_time_B = 0; // Armazena o tempo do último evento para o botão B

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);
void print_led_status(const char *botao, const char *cor, bool led_status);
void atualizar_matriz_led(int contador);

// Definição de pixel GRB
struct pixel_t
{
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t;
typedef pixel_t npLED_t; // Mudança de nome de "struct pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin)
{

  // Cria programa PIO.
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;

  // Toma posse de uma máquina PIO.
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0)
  {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
  }

  // Inicia programa na máquina PIO obtida.
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

  // Limpa buffer de pixels.
  for (uint i = 0; i < LED_COUNT; ++i)
  {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

#define INTENSIDADE 0.5

/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
  // Aplica o fator de intensidade para reduzir a cor
  leds[index].R = (uint8_t)(r * INTENSIDADE);
  leds[index].G = (uint8_t)(g * INTENSIDADE);
  leds[index].B = (uint8_t)(b * INTENSIDADE);
}

/**
 * Limpa o buffer de pixels.
 */
void npClear()
{
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite()
{
  // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
  for (uint i = 0; i < LED_COUNT; ++i)
  {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}

int getIndex(int x, int y)
{
  // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
  // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
  if (y % 2 == 0)
  {
    return 24 - (y * 5 + x); // Linha par (esquerda para direita).
  }
  else
  {
    return 24 - (y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
  }
}

int main()
{
  stdio_init_all();

  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  gpio_init(led_pin_g);
  gpio_set_dir(led_pin_g, GPIO_OUT);
  gpio_init(led_pin_b);
  gpio_set_dir(led_pin_b, GPIO_OUT);
  gpio_init(led_pin_r);
  gpio_set_dir(led_pin_r, GPIO_OUT);

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA);                                        // Pull up the data line
  gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
  ssd1306_t ssd;                                                // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd);                                         // Configura o display
  ssd1306_send_data(&ssd);                                      // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  bool cor = true;

  gpio_set_irq_enabled_with_callback(PIN_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(PIN_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  const char *init_message = "UART Demo - RP2\r\nDigite algo e veja o eco:\r\n";
  uart_puts(UART_ID, init_message);

  npInit(LED_PIN);
  npClear();

  while (true)
  {
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor);                           // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);       // Desenha um retângulo
    ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 10); // Desenha uma string
    ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 30);   // Desenha uma string
    ssd1306_draw_string(&ssd, "PROF WILTON", 15, 48);   // Desenha uma string
    ssd1306_send_data(&ssd);                            // Atualiza o display

    sleep_ms(1000);

    if (uart_is_readable(UART_ID))
    {
      // Lê um caractere da UART
      char c = uart_getc(UART_ID);

      // Controla os LEDs com base nos caracteres recebidos
      if (c == 'r')
      {
        gpio_put(led_pin_r, !gpio_get(led_pin_r));
      }
      if (c == 'g')
      {
        gpio_put(led_pin_g, !gpio_get(led_pin_g));
      }
      if (c == 'b')
      {
        gpio_put(led_pin_b, !gpio_get(led_pin_b));
      }
      if (c == '0')
      {
        atualizar_matriz_led(0);
      }
      if (c == '1')
      {
        atualizar_matriz_led(1);
      }
      if (c == '2')
      {
        atualizar_matriz_led(2);
      }
      if (c == '3')
      {
        atualizar_matriz_led(3);
      }
      if (c == '4')
      {
        atualizar_matriz_led(4);
      }
      if (c == '5')
      {
        atualizar_matriz_led(5);
      }
      if (c == '6')
      {
        atualizar_matriz_led(6);
      }
      if (c == '7')
      {
        atualizar_matriz_led(7);
      }
      if (c == '8')
      {
        atualizar_matriz_led(8);
      }
      if (c == '9')
      {
        atualizar_matriz_led(9);
      }

      // Envia o caractere de volta (eco)
      uart_putc(UART_ID, c);
      uart_puts(UART_ID, " <- Tecla pressionada\r\n");
    }
    sleep_ms(40);
  }

  return 0;
}

void atualizar_matriz_led(int contador)
{
  for (int linha = 0; linha < 5; linha++)
  {
    for (int coluna = 0; coluna < 5; coluna++)
    {
      int posicao = getIndex(linha, coluna);
      npSetLED(posicao,
               matrizes[contador][coluna][linha][0],
               matrizes[contador][coluna][linha][1],
               matrizes[contador][coluna][linha][2]);
    }
  }
  npWrite(); // Atualiza a matriz de LEDs
}

void print_led_status(const char *botao, const char *cor, bool led_status)
{
  if (led_status)
  {
    uart_puts(UART_ID, "Botão ");
    uart_puts(UART_ID, botao);
    uart_puts(UART_ID, " pressionado! LED ");
    uart_puts(UART_ID, cor);
    uart_puts(UART_ID, " desligado.\r\n");
  }
  else
  {
    uart_puts(UART_ID, "Botão ");
    uart_puts(UART_ID, botao);
    uart_puts(UART_ID, " pressionado! LED ");
    uart_puts(UART_ID, cor);
    uart_puts(UART_ID, " ligado.\r\n");
  }
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
  uint32_t current_time = to_us_since_boot(get_absolute_time());

  // Se o evento for do botão A
  if (gpio == PIN_BOTAO_A)
  {
    // Verifica se passou tempo suficiente (200ms) para o debounce
    if (current_time - last_time_A > 200000)
    {
      last_time_A = current_time;            // Atualiza o tempo do último evento
      bool led_status = gpio_get(led_pin_g); // Verifica o estado atual do LED
      gpio_put(led_pin_g, !led_status);      // Inverte o estado do LED
      // Chama a função para imprimir o estado do LED no serial monitor
      print_led_status("A", "Verde", led_status);
    }
  }

  // Se o evento for do botão B
  if (gpio == PIN_BOTAO_B)
  {
    // Verifica se passou tempo suficiente (200ms) para o debounce
    if (current_time - last_time_B > 200000)
    {
      last_time_B = current_time;            // Atualiza o tempo do último evento
      bool led_status = gpio_get(led_pin_b); // Verifica o estado atual do LED
      gpio_put(led_pin_b, !led_status);      // Inverte o estado do LED
      // Chama a função para imprimir o estado do LED no serial monitor
      print_led_status("B", "Azul", led_status);
    }
  }
}