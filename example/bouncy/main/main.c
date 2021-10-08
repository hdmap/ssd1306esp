#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "ssd1306esp.h"


int xpos = 69;
int ypos = 32;
int xdir = 1;
int ydir = 1;

SSD1306ESP_Buffer buf;

void draw_loop()
{
	for (int i = 0; i < SSD1306_HEIGHT * SSD1306_WIDTH / 8; i++)
			buf[i] = 0x00;
	
	if (xdir)
		xpos >= SSD1306_WIDTH - 1 ? xdir = 0 : ++xpos;
	else
		xpos == 0 ? xdir = 1 : --xpos;
	
	
	if (ydir)
		ypos >= SSD1306_HEIGHT - 1 ? ydir = 0 : ++ypos;
	else
		ypos == 0 ? ydir = 1 : --ypos;
	
	buf[xpos + (ypos / 8) * SSD1306_WIDTH] = 1 << (ypos % 8);
}

void app_main(void)
{
	printf("initing\n");
	
	
	SSD1306ESP_Init(15, 4, 16, I2C_NUM_0, 70000);

	printf("writing\n");
	
	SSD1306ESP_UploadScreen(buf);
	
	SSD1306ESP_EnableScreen();
	
	while (1)
	{
		vTaskDelay(16 / portTICK_PERIOD_MS);
		
		draw_loop();
		
		SSD1306ESP_UploadScreen(buf);
	}
}
