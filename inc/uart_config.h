#ifndef UART_CONFIG_H
#define UART_CONFIG_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

ssd1306_t ssd;
bool cor = true;

void setup_uart()
{
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

void setup_i2c_init()
{
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);
}

void setup_gpio_i2c()
{
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA);                     // Pull up the data line
  gpio_pull_up(I2C_SCL);
}

void setup_ssd1306_init()
{                                                               // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd);                                         // Configura o display
  ssd1306_send_data(&ssd);
}

void clear_display()
{
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);
}

void init_uart_with_message()
{
  const char *init_message = "UART Demo - RP2\r\nDigite algo e veja o eco:\r\n";
  uart_puts(UART_ID, init_message);
}

#endif // UART_CONFIG_H