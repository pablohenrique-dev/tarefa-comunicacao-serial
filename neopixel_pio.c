#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/font.h"

#include "inc/numbers.h"
#include "inc/button_config.h"
#include "inc/leds_config.h"
#include "inc/matrix_leds_config.h"
#include "inc/uart_config.h"

void print_led_status(const char *botao, const char *cor, bool led_status);

int main()
{
  stdio_init_all();

  setup_uart();

  setup_gpio_leds();

  setup_i2c_init();

  setup_gpio_i2c();
  setup_ssd1306_init(); // Envia os dados para o display

  clear_display();

  setup_gpio_buttons();

  gpio_set_irq_enabled_with_callback(PIN_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(PIN_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  init_uart_with_message();

  npInit(LED_PIN);
  npClear();

  while (true)
  {
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, false);                           // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);        // Desenha um retângulo
    ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 20);    // Desenha a primeira string
    ssd1306_draw_string(&ssd, "PABLO HENRIQUE", 10, 40); // Desenha a segunda string
    ssd1306_send_data(&ssd);                             // Atualiza o display

    sleep_ms(1000);

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
