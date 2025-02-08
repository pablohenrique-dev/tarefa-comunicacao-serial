#include <stdio.h>
#include "pico/stdlib.h"
// #include "hardware/uart.h"
// #include "hardware/pio.h"
// #include "hardware/timer.h"
// #include "hardware/clocks.h"
// #include "hardware/i2c.h"
// #include "inc/ssd1306.h"
#include "inc/font.h"

#include "inc/numbers.h"
#include "inc/button_config.h"
#include "inc/leds_config.h"
#include "inc/matrix_leds_config.h"
#include "inc/uart_config.h"

// #define I2C_PORT i2c1
// #define I2C_SDA 14
// #define I2C_SCL 15
// #define endereco 0x3C

// #define UART_ID uart0
// #define BAUD_RATE 115200
// #define UART_TX_PIN 0
// #define UART_RX_PIN 1

void print_led_status(const char *botao, const char *cor, bool led_status);

int main()
{
  stdio_init_all();

  setup_uart();

  // uart_init(UART_ID, BAUD_RATE);
  // gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  // gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  setup_gpio_leds();

  setup_i2c_init();
  // I2C Initialisation. Using it at 400Khz.
  // i2c_init(I2C_PORT, 400 * 1000);

  setup_gpio_i2c();
  setup_ssd1306_init();
  // gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  // gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  // gpio_pull_up(I2C_SDA);                                        // Pull up the data line
  // gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
  // ssd1306_t ssd;                                                // Inicializa a estrutura do display
  // ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  // ssd1306_config(&ssd);                                         // Configura o display
  // ssd1306_send_data(&ssd);                                      // Envia os dados para o display

  clear_display();
  // Limpa o display. O display inicia com todos os pixels apagados.
  // ssd1306_fill(&ssd, false);
  // ssd1306_send_data(&ssd);

  setup_gpio_buttons();

  gpio_set_irq_enabled_with_callback(PIN_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(PIN_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  init_uart_with_message();
  // const char *init_message = "UART Demo - RP2\r\nDigite algo e veja o eco:\r\n";
  // uart_puts(UART_ID, init_message);

  npInit(LED_PIN);
  npClear();

  while (true)
  {
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    // ssd1306_fill(&ssd, false);                           // Limpa o display
    // ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);        // Desenha um retângulo
    // ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 20);    // Desenha a primeira string
    // ssd1306_draw_string(&ssd, "PABLO HENRIQUE", 10, 40); // Desenha a segunda string
    // ssd1306_send_data(&ssd);                             // Atualiza o display

    // sleep_ms(1000);

    if (uart_is_readable(UART_ID))
    {
      // Lê um caractere da UART
      char c = uart_getc(UART_ID);

      // Limpa o display e exibe o caractere lido
      ssd1306_fill(&ssd, false);             // Limpa o display
      ssd1306_draw_string(&ssd, &c, 10, 10); // Desenha o caractere na posição (10,10)
      ssd1306_send_data(&ssd);               // Atualiza o display

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
