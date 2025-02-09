#include <stdio.h>
#include "pico/stdlib.h"
#include "inc/font.h"

#include "inc/numbers.h"
#include "inc/button_config.h"
#include "inc/leds_config.h"
#include "inc/matrix_leds_config.h"
#include "inc/uart_config.h"

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

  // 🛠️ Limpa qualquer caractere residual da UART
  while (uart_is_readable(UART_ID))
  {
    uart_getc(UART_ID);
  }

  // 🕒 Pequeno delay para estabilizar o sistema
  sleep_ms(100);

  // 📟 Exibe a mensagem inicial corretamente
  ssd1306_fill(&ssd, false);
  ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 20);    // Desenha a primeira string
  ssd1306_draw_string(&ssd, "PABLO HENRIQUE", 10, 40); // Desenha a segunda string
  ssd1306_send_data(&ssd);                             // Atualiza o display

  while (true)
  {
    if (uart_is_readable(UART_ID))
    {
      // Lê um caractere da UART
      char c = uart_getc(UART_ID);

      // Converte o caractere em uma string válida para exibição
      char buffer[2] = {c, '\0'};

      // Limpa o display e exibe o caractere lido
      ssd1306_fill(&ssd, false);
      ssd1306_draw_string(&ssd, "TECLA", 44, 20);
      ssd1306_draw_string(&ssd, buffer, 63, 40);
      ssd1306_send_data(&ssd);

      // Controla os LEDs com base nos caracteres recebidos
      if (c == 'r')
        gpio_put(led_pin_r, !gpio_get(led_pin_r));
      if (c == 'g')
        gpio_put(led_pin_g, !gpio_get(led_pin_g));
      if (c == 'b')
        gpio_put(led_pin_b, !gpio_get(led_pin_b));

      // Atualiza a matriz de LED conforme o número recebido
      if (c >= '0' && c <= '9')
      {
        atualizar_matriz_led(c - '0');
      }

      // Envia o caractere de volta (eco)
      uart_putc(UART_ID, c);
      uart_puts(UART_ID, " <- Tecla pressionada\r\n");
    }

    sleep_ms(40);
  }

  return 0;
}
