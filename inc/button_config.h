#ifndef BUTTON_CONFIG_H
#define BUTTON_CONFIG_H

#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "leds_config.h"

#define PIN_BOTAO_A 5 // Define o pino do botão A
#define PIN_BOTAO_B 6 // Define o pino do botão B

static volatile uint32_t last_time_A = 0;
static volatile uint32_t last_time_B = 0;

static void gpio_irq_handler(uint gpio, uint32_t events);

void setup_gpio_buttons()
{
  gpio_init(PIN_BOTAO_A);
  gpio_set_dir(PIN_BOTAO_A, GPIO_IN);
  gpio_pull_up(PIN_BOTAO_A);

  gpio_init(PIN_BOTAO_B);
  gpio_set_dir(PIN_BOTAO_B, GPIO_IN);
  gpio_pull_up(PIN_BOTAO_B);
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
  uint32_t current_time = to_us_since_boot(get_absolute_time());

  if (gpio == PIN_BOTAO_A)
  {
    if (current_time - last_time_A > 200000)
    {
      last_time_A = current_time;
      bool led_status = gpio_get(led_pin_g);
      gpio_put(led_pin_g, !led_status);
    }
  }

  if (gpio == PIN_BOTAO_B)
  {
    if (current_time - last_time_B > 200000)
    {
      last_time_B = current_time;
      bool led_status = gpio_get(led_pin_b);
      gpio_put(led_pin_b, !led_status);
    }
  }
}

extern int button_states[5];

#endif // BUTTON_CONFIG_H
