#include "ssd1306esp.h"

#define SSD1306I2C_ADDR 0x3c

#define ACK_CHECK_EN   0x1

int i2cp;


static void SSD1306ESP_WriteByte(uint8_t data);
void SSD1306ESP_SetContrast(uint8_t contrast);

void InitCommands()
{
	SSD1306ESP_DisableScreen();
	
	SSD1306ESP_SetContrast(0xFF);
	
	// horz addr mode
	SSD1306ESP_WriteByte(0x20);
	SSD1306ESP_WriteByte(0x10);
	
	// page start 0
	SSD1306ESP_WriteByte(0xB0);
	
	// COM direction
	SSD1306ESP_WriteByte(0xC8);
	
	// column and line start addrs
	SSD1306ESP_WriteByte(0x21);
	SSD1306ESP_WriteByte(0x00);
	SSD1306ESP_WriteByte(0xFF);
	
	SSD1306ESP_WriteByte(0x00);
	SSD1306ESP_WriteByte(0x10);
	SSD1306ESP_WriteByte(0x40);
	
	// segment remap
	SSD1306ESP_WriteByte(0xA1);
	
	// normal/not inverted pixels
	SSD1306ESP_WriteByte(0xA6);
	
	// no display offset
	SSD1306ESP_WriteByte(0xD3);
	SSD1306ESP_WriteByte(0x00);
	
	// multiplex ratio
	SSD1306ESP_WriteByte(0xA8);
	SSD1306ESP_WriteByte(0x3F);
	
	// display from ram
	SSD1306ESP_WriteByte(0xA4);
	
	// clock divide
	SSD1306ESP_WriteByte(0xD5);
	
	// divide ratio
	SSD1306ESP_WriteByte(0xF0);
	
	//precharge period
	SSD1306ESP_WriteByte(0xD9);
	SSD1306ESP_WriteByte(0x22);
	
	// com pins
	SSD1306ESP_WriteByte(0xDA);
	SSD1306ESP_WriteByte(0x12);
	
	// vcomh
	SSD1306ESP_WriteByte(0xDB);
	SSD1306ESP_WriteByte(0x20);
	
	// dc-dc enable
	SSD1306ESP_WriteByte(0x8D);
	SSD1306ESP_WriteByte(0x14);
	
}

void SSD1306ESP_Init(int scl, int sda, int rst, int i2c_port, int freq)
{
	printf("initing ssd1306\n");
	
	i2cp = i2c_port;
	
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.master.clk_speed = freq;
	
	conf.scl_io_num = scl;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	
	conf.sda_io_num = sda;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	
	i2c_param_config(i2c_port, &conf);
	
	esp_err_t res = i2c_driver_install(i2cp, I2C_MODE_MASTER, 0, 0, 0);
	
	if (res != ESP_OK)
		printf("ssd1306 i2c install fail\n");
	
	// ssd1306 requires rst pulled low to initialize
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<16) )
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 0;
	
	gpio_config(&io_conf);
	
	gpio_set_level(16, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(16, 1);
	
	InitCommands();
}

void SSD1306ESP_InitNoHWInit(int scl, int sda, int rst, int i2c_port, int freq)
{
	i2cp = i2c_port;
	
	InitCommands();
}



void SSD1306ESP_DisableScreen()
{
	SSD1306ESP_WriteByte(0xAE);
}

void SSD1306ESP_EnableScreen()
{
	SSD1306ESP_WriteByte(0xAF);
}


void SSD1306ESP_UploadScreen(SSD1306ESP_Buffer data)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd;
	int i;
	
	for (i = 0; i < 8; i++)
	{
		SSD1306ESP_WriteByte(0xB0 + i);
		SSD1306ESP_WriteByte(0x00);
		SSD1306ESP_WriteByte(0x10);
		
		cmd = i2c_cmd_link_create();
		
		i2c_master_start(cmd);
		
		i2c_master_write_byte(cmd, ( SSD1306I2C_ADDR<< 1 ) | I2C_MASTER_WRITE, ACK_CHECK_EN);
		
		i2c_master_write_byte(cmd, 0x40, ACK_CHECK_EN);
		
		i2c_master_write(cmd, &data[SSD1306_WIDTH * i], SSD1306_WIDTH, ACK_CHECK_EN);
		
		i2c_master_stop(cmd);
		
		ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 50 / portTICK_RATE_MS);
		i2c_cmd_link_delete(cmd);
	}
}

void SSD1306ESP_SetContrast(uint8_t contrast)
{
	SSD1306ESP_WriteByte(0x81);
	SSD1306ESP_WriteByte(contrast);
}

void SSD1306ESP_SetInvert(uint8_t invert)
{
	if (invert)
		SSD1306ESP_WriteByte(0xA7);
	else
		SSD1306ESP_WriteByte(0xA6);
}

static void SSD1306ESP_WriteByte(uint8_t data)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd;
	
	cmd = i2c_cmd_link_create();
	
	i2c_master_start(cmd);
	
	i2c_master_write_byte(cmd, ( SSD1306I2C_ADDR<< 1 ) | I2C_MASTER_WRITE, ACK_CHECK_EN);
	
	i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
	
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	
	i2c_master_stop(cmd);
	
	ret = i2c_master_cmd_begin(i2cp, cmd, 50 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	
	if (ret != ESP_OK)
		printf("ssd1306 writebyte fail\n");
	
	return;
}
