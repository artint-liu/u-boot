#include <common.h>
#include <asm/gpio.h>
#include <asm/io.h>

int LCD_CLK;  // PE9
int LCD_MOSI; // PE8
int LCD_CS;   // PE7
int LCD_BL;   // PE6

// CONFIG_VIDEO_LCD_MODE
// 参考参数: CONFIG_VIDEO_LCD_MODE="x:800,y:480,depth:18,pclk_khz:33000,le:87,ri:40,up:31,lo:13,hs:1,vs:1,sync:3,vmode:0"
// 使用参数: CONFIG_VIDEO_LCD_MODE="x:480,y:480,depth:18,pclk_khz:10000,hs:38,vs:39,le:24,ri:6,up:16,lo:4,sync:3,vmode:0"

#if 0
#define SPI_MOSI(L)     gpio_direction_output(LCD_MOSI, L)
#define SPI_MOSI_HIGH() SPI_MOSI(1)
#define SPI_MOSI_LOW()  SPI_MOSI(0)
#define SPI_SCK_HIGH()  gpio_direction_output(LCD_CLK, 1)
#define SPI_SCK_LOW()   gpio_direction_output(LCD_CLK, 0)
#define SPI_CS_HIGH()  gpio_direction_output(LCD_CS, 1)
#define SPI_CS_LOW()   gpio_direction_output(LCD_CS, 0)
#else
#define SPI_MOSI(L)     sunxi_gpio_output2(SUNXI_GPE(8), L)
#define SPI_MOSI_HIGH() SPI_MOSI(1)
#define SPI_MOSI_LOW()  SPI_MOSI(0)
#define SPI_SCK_HIGH()  sunxi_gpio_output2(SUNXI_GPE(9), 1)
#define SPI_SCK_LOW()   sunxi_gpio_output2(SUNXI_GPE(9), 0)
#define SPI_CS_HIGH()   sunxi_gpio_output2(SUNXI_GPE(7), 1)
#define SPI_CS_LOW()    sunxi_gpio_output2(SUNXI_GPE(7), 0)
#endif

#define SPI_DELAY()       lcd_delay(100)
#define WRITE9BITCOMMAND  Write9BitsCommand
#define WRITE9BITDATA     Write9BitsData

void lcd_delay(int delay)
{
  udelay(delay);
  //delay *= 1000;
  //while(--delay);
}

static int sunxi_gpio_output2(uint32_t pin, uint32_t val)
{
	uint32_t dat;
	uint32_t bank = GPIO_BANK(pin);
	uint32_t num = GPIO_NUM(pin);
	struct sunxi_gpio *pio = BANK_TO_GPIO(bank);

	dat = readl(&pio->dat);
	if (val)
		dat |= (0x1 << num);
	else
		dat &= ~(0x1 << num);

	writel(dat, &pio->dat);

	return 0;
}


void Write9Bits(uint8_t d, uint8_t dc)
{
  // dc ? SPI_MOSI_HIGH() : SPI_MOSI_LOW();
  SPI_MOSI(dc);

  SPI_SCK_HIGH();
  SPI_DELAY();

  SPI_SCK_LOW();
  SPI_DELAY();

  uint8_t bit = 0x80;
  while (bit)
  {
    if (d & bit)
    {
      SPI_MOSI_HIGH();
    }
    else
    {
      SPI_MOSI_LOW();
    }
    SPI_SCK_HIGH();
    SPI_DELAY();

    bit >>= 1;

    SPI_SCK_LOW();
    SPI_DELAY();
  }
}

void Write9BitsData(uint8_t d)
{
  Write9Bits(d, 1);
}

void Write9BitsCommand(uint8_t c)
{
  Write9Bits(c, 0);
}

int SPI_Init()
{
#if 0
  int ret;
  LCD_CLK = sunxi_name_to_gpio("PE9");
  if (LCD_CLK < 0) {
    printf("Error invalid LCD CLK pin: PE, err %d\n", LCD_CLK);
    return LCD_CLK;
  }
  ret = gpio_request(LCD_CLK, "LCD_CLK");
  if (ret) {
    printf("Error requesting LCD_CLK pin: PE, err %d\n", LCD_CLK);
    return ret;
  }

  LCD_MOSI = sunxi_name_to_gpio("PE8");
  if (LCD_MOSI < 0) {
    printf("Error invalid LCD MOSI pin: PE, err %d\n", LCD_MOSI);
    return LCD_MOSI;
  }
  ret = gpio_request(LCD_MOSI, "LCD_MOSI");
  if (ret) {
    printf("Error requesting LCD_MOSI pin: PE, err %d\n", LCD_MOSI);
    return ret;
  }

  LCD_CS = sunxi_name_to_gpio("PE7");
  if (LCD_CS < 0) {
    printf("Error invalid LCD CS pin: PE, err %d\n", LCD_CS);
    return LCD_CS;
  }
  ret = gpio_request(LCD_CS, "LCD_CS");
  if (ret) {
    printf("Error requesting LCD_CS pin: PE, err %d\n", LCD_CS);
    return ret;
  }

  LCD_BL = sunxi_name_to_gpio("PE6");
  if (LCD_BL < 0) {
    printf("Error invalid LCD BL pin: PE, err %d\n", LCD_BL);
    return LCD_BL;
  }
  ret = gpio_request(LCD_BL, "LCD_BL");
  if (ret) {
    printf("Error requesting LCD_BL pin: PE, err %d\n", LCD_BL);
    return ret;
  }
#else

  sunxi_gpio_set_cfgpin(SUNXI_GPE(6), SUNXI_GPIO_OUTPUT);
  sunxi_gpio_set_cfgpin(SUNXI_GPE(7), SUNXI_GPIO_OUTPUT);
  sunxi_gpio_set_cfgpin(SUNXI_GPE(8), SUNXI_GPIO_OUTPUT);
  sunxi_gpio_set_cfgpin(SUNXI_GPE(9), SUNXI_GPIO_OUTPUT);

  sunxi_gpio_output2(SUNXI_GPE(6), 1);
  
  printf("Soft SPI Init OK\n");
#endif

  return 0;
}

void LCD_InitCommand()
{
	SPI_CS_LOW();
	WRITE9BITCOMMAND(0x1);
	SPI_CS_HIGH();

	lcd_delay(120 * 1000);

	// batchOperation2

	SPI_CS_LOW();
	WRITE9BITCOMMAND(0xff);
	WRITE9BITDATA(0x77);
	WRITE9BITDATA(0x1);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x10);
	WRITE9BITCOMMAND(0xc0);
	WRITE9BITDATA(0x3b);
	WRITE9BITDATA(0x0);
	WRITE9BITCOMMAND(0xc1);
	WRITE9BITDATA(0xd);
	WRITE9BITDATA(0x2);
	WRITE9BITCOMMAND(0xc2);
	WRITE9BITDATA(0x31);
	WRITE9BITDATA(0x5);
	WRITE9BITCOMMAND(0xcd);
	WRITE9BITDATA(0x8);
	WRITE9BITCOMMAND(0xb0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x18);
	WRITE9BITDATA(0xe);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x6);
	WRITE9BITDATA(0x7);
	WRITE9BITDATA(0x8);
	WRITE9BITDATA(0x7);
	WRITE9BITDATA(0x22);
	WRITE9BITDATA(0x4);
	WRITE9BITDATA(0x12);
	WRITE9BITDATA(0xf);
	WRITE9BITDATA(0xaa);
	WRITE9BITDATA(0x31);
	WRITE9BITDATA(0x18);
	WRITE9BITCOMMAND(0xb1);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x19);
	WRITE9BITDATA(0xe);
	WRITE9BITDATA(0x12);
	WRITE9BITDATA(0x7);
	WRITE9BITDATA(0x8);
	WRITE9BITDATA(0x8);
	WRITE9BITDATA(0x8);
	WRITE9BITDATA(0x22);
	WRITE9BITDATA(0x4);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0xa9);
	WRITE9BITDATA(0x32);
	WRITE9BITDATA(0x18);
	WRITE9BITCOMMAND(0xff);
	WRITE9BITDATA(0x77);
	WRITE9BITDATA(0x1);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x11);
	WRITE9BITCOMMAND(0xb0);
	WRITE9BITDATA(0x60);
	WRITE9BITCOMMAND(0xb1);
	WRITE9BITDATA(0x32);
	WRITE9BITCOMMAND(0xb2);
	WRITE9BITDATA(0x7);
	WRITE9BITCOMMAND(0xb3);
	WRITE9BITDATA(0x80);
	WRITE9BITCOMMAND(0xb5);
	WRITE9BITDATA(0x49);
	WRITE9BITCOMMAND(0xb7);
	WRITE9BITDATA(0x85);
	WRITE9BITCOMMAND(0xb8);
	WRITE9BITDATA(0x21);
	WRITE9BITCOMMAND(0xc1);
	WRITE9BITDATA(0x78);
	WRITE9BITCOMMAND(0xc2);
	WRITE9BITDATA(0x78);
	WRITE9BITCOMMAND(0xe0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x1b);
	WRITE9BITDATA(0x2);
	WRITE9BITCOMMAND(0xe1);
	WRITE9BITDATA(0x8);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x7);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x44);
	WRITE9BITDATA(0x44);
	WRITE9BITCOMMAND(0xe2);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x44);
	WRITE9BITDATA(0x44);
	WRITE9BITDATA(0xed);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0xec);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITCOMMAND(0xe3);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x11);
	WRITE9BITCOMMAND(0xe4);
	WRITE9BITDATA(0x44);
	WRITE9BITDATA(0x44);
	WRITE9BITCOMMAND(0xe5);
	WRITE9BITDATA(0xa);
	WRITE9BITDATA(0xe9);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0xc);
	WRITE9BITDATA(0xeb);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0xe);
	WRITE9BITDATA(0xed);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0x10);
	WRITE9BITDATA(0xef);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITCOMMAND(0xe6);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x11);
	WRITE9BITDATA(0x11);
	WRITE9BITCOMMAND(0xe7);
	WRITE9BITDATA(0x44);
	WRITE9BITDATA(0x44);
	WRITE9BITCOMMAND(0xe8);
	WRITE9BITDATA(0x9);
	WRITE9BITDATA(0xe8);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0xb);
	WRITE9BITDATA(0xea);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0xd);
	WRITE9BITDATA(0xec);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITDATA(0xf);
	WRITE9BITDATA(0xee);
	WRITE9BITDATA(0xd8);
	WRITE9BITDATA(0xa0);
	WRITE9BITCOMMAND(0xeb);
	WRITE9BITDATA(0x2);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0xe4);
	WRITE9BITDATA(0xe4);
	WRITE9BITDATA(0x88);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x40);
	WRITE9BITCOMMAND(0xec);
	WRITE9BITDATA(0x3c);
	WRITE9BITDATA(0x0);
	WRITE9BITCOMMAND(0xed);
	WRITE9BITDATA(0xab);
	WRITE9BITDATA(0x89);
	WRITE9BITDATA(0x76);
	WRITE9BITDATA(0x54);
	WRITE9BITDATA(0x2);
	WRITE9BITDATA(0xff);
	WRITE9BITDATA(0xff);
	WRITE9BITDATA(0xff);
	WRITE9BITDATA(0xff);
	WRITE9BITDATA(0xff);
	WRITE9BITDATA(0xff);
	WRITE9BITDATA(0x20);
	WRITE9BITDATA(0x45);
	WRITE9BITDATA(0x67);
	WRITE9BITDATA(0x98);
	WRITE9BITDATA(0xba);
	WRITE9BITCOMMAND(0xff);
	WRITE9BITDATA(0x77);
	WRITE9BITDATA(0x1);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x13);
	WRITE9BITCOMMAND(0xe5);
	WRITE9BITDATA(0xe4);
	WRITE9BITCOMMAND(0xff);
	WRITE9BITDATA(0x77);
	WRITE9BITDATA(0x1);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITDATA(0x0);
	WRITE9BITCOMMAND(0x21);
	WRITE9BITCOMMAND(0x3a);
	WRITE9BITDATA(0x60);
	WRITE9BITCOMMAND(0x11);
	SPI_CS_HIGH();

	lcd_delay(120 * 1000);

	SPI_CS_LOW();
	WRITE9BITCOMMAND(0x29);
	SPI_CS_HIGH();
}


void LCD_Init()
{
  int ret = SPI_Init();
  if(ret == 0)
  {
    //SPI_CS_LOW();
    //lcd_delay(100);
    LCD_InitCommand();
    printf("Send LCD SPI command finished.\n");
    //SPI_CS_HIGH();
  }
}
