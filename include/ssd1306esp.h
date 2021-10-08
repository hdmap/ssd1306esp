#ifndef SSD1306ESP
#define SSD1306ESP

#include <stdio.h>
#include "ssd1306esp.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#define SSD1306_HEIGHT 64
#define SSD1306_WIDTH 128

typedef uint8_t SSD1306ESP_Buffer[SSD1306_HEIGHT * SSD1306_WIDTH / 8];

void SSD1306ESP_Init(int scl, int sda, int rst, int i2c_port, int freq);

void SSD1306ESP_InitNoHWInit(int scl, int sda, int rst, int i2c_port, int freq);

void SSD1306ESP_DisableScreen();

void SSD1306ESP_EnableScreen();

void SSD1306ESP_UploadScreen(SSD1306ESP_Buffer data);

void SSD1306ESP_SetContrast(uint8_t contrast);

void SSD1306ESP_SetInvert(uint8_t invert);

#endif