#ifndef LEDS_CONFIG_H
#define LEDS_CONFIG_H

#include "pico/stdlib.h"

#define led_pin_g 11
#define led_pin_b 12
#define led_pin_r 13

void setup_gpio_leds()
{
  gpio_init(led_pin_g);
  gpio_set_dir(led_pin_g, GPIO_OUT);
  gpio_init(led_pin_b);
  gpio_set_dir(led_pin_b, GPIO_OUT);
  gpio_init(led_pin_r);
  gpio_set_dir(led_pin_r, GPIO_OUT);
}

#endif // LEDS_CONFIG_H
