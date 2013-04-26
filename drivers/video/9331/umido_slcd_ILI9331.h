#ifndef __UMIDO_ILI9331_H__
#define __UMIDO_ILI9331_H__

#include <asm/jzsoc.h>

#if defined (CONFIG_JZ4750_SLCD_L009_ILI9331)
#define WR_GRAM_CMD	0x22

#if defined(CONFIG_JZ4750D_L009)
#define PIN_CS_N 	(32*3+22)		/* GPD22       */
#define PIN_RESET_N 	(32*3+23)	    /* GPD23       */
#else
#error "Define special lcd pins for your platform."
#endif

static void Mcupanel_Data(unsigned int data)
{
	while ((REG_SLCD_STATE)&(1 << 0));
	REG_SLCD_DATA = (0 << 31) | (data&0xffff);
}

/* Sent a command with data (18-bit bus, 16-bit index, 16-bit register value) */
static void Mcupanel_RegSet(unsigned int cmd, unsigned int data)
{
	printk("CMD = 0x%x, data=x%x\n",cmd,data);
	while ((REG_SLCD_STATE)&(1 << 0));
	REG_SLCD_DATA = (1 << 31) | ((cmd&0xff00) >> 8);
	while ((REG_SLCD_STATE)&(1 << 0));
	REG_SLCD_DATA = (1 << 31) | ((cmd&0xff) >> 0);
	while ((REG_SLCD_STATE)&(1 << 0));
	REG_SLCD_DATA = (0 << 31) | (data&0xffff);
}

/* Sent a command without data  (18-bit bus, 16-bit index) */
static void Mcupanel_Command(unsigned int cmd) {
	while ((REG_SLCD_STATE)&(1 << 0));
	REG_SLCD_DATA =  (1 << 31) | ((cmd&0xff00) >> 8);
	while ((REG_SLCD_STATE)&(1 << 0));
	REG_SLCD_DATA =(1 << 31) | ((cmd&0xff) >> 0);
}

/* Set the start address of screen, for example (0, 0) */
void Mcupanel_SetAddr(u32 x, u32 y) //u32
{
	Mcupanel_RegSet(0x20,x) ;
	udelay(1);
	Mcupanel_RegSet(0x21,y) ;
	udelay(1);
	Mcupanel_Command(0x22);
}

#undef __lcd_special_pin_init
#define __lcd_special_pin_init() \
do {	\
	__gpio_as_output(PIN_CS_N);	\
	__gpio_as_output(PIN_RESET_N);	\
	__gpio_clear_pin(PIN_CS_N); /* Clear CS */	\
	mdelay(100);	\
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(50);	\
	__gpio_clear_pin(PIN_RESET_N);	\
	mdelay(50);	\
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(100);	\
} while(0)

#undef SlcdInit
#define SlcdInit()	\
do {					\
  Mcupanel_RegSet(0x00E3,0x3008);       \
  Mcupanel_RegSet(0x00E7,0x0012);       \
  Mcupanel_RegSet(0x00Ef,0x1231);       \
  Mcupanel_RegSet(0x0000,0x0001);       \
  mdelay(50); 				\
  Mcupanel_RegSet(0x0001,0x0100);       \
  Mcupanel_RegSet(0x0002,0x0700);    	\
  Mcupanel_RegSet(0x0003,0x5098);	\
  Mcupanel_RegSet(0x0004,0x0000);       \
  Mcupanel_RegSet(0x0008,0x0202);       \
  Mcupanel_RegSet(0x0009,0x0000);       \
  Mcupanel_RegSet(0x0010,0x0000);       \
  Mcupanel_RegSet(0x0011,0x0007);      	\
  Mcupanel_RegSet(0x0012,0x0000);       \
  Mcupanel_RegSet(0x0013,0x0000);       \
  mdelay(200); 				\
  Mcupanel_RegSet(0x10,0x1590);         \
  Mcupanel_RegSet(0x11,0x0227);         \
  mdelay(50); 				\
  Mcupanel_RegSet(0x12,0x0091);		\
  mdelay(50); 				\
  Mcupanel_RegSet(0x13,0x0900);		\
  Mcupanel_RegSet(0x29,0x0018);		\
  Mcupanel_RegSet(0x2B,0x0009);		\
  mdelay(50); 				\
  Mcupanel_RegSet(0x30,0x0000);		\
  Mcupanel_RegSet(0x31,0x0507);		\
  Mcupanel_RegSet(0x32,0x0303);		\
  Mcupanel_RegSet(0x35,0x0103);		\
  Mcupanel_RegSet(0x36,0x1400);		\
  Mcupanel_RegSet(0x37,0x0105);		\
  Mcupanel_RegSet(0x38,0x0000);		\
  Mcupanel_RegSet(0x39,0x0707);		\
  Mcupanel_RegSet(0x3C,0x0301);		\
  Mcupanel_RegSet(0x3D,0x020a);		\
  Mcupanel_RegSet(0x50,0x0000);		\
  Mcupanel_RegSet(0x51,0x00EF);		\
  Mcupanel_RegSet(0x52,0x0000);		\
  Mcupanel_RegSet(0x53,0x013F);		\
  Mcupanel_RegSet(0x60,0xa700);		\
  Mcupanel_RegSet(0x61,0x0001);		\
  Mcupanel_RegSet(0x6A,0x0000);		\
  Mcupanel_RegSet(0x80,0x0000);		\
  Mcupanel_RegSet(0x81,0x0000);		\
  Mcupanel_RegSet(0x82,0x0000);		\
  Mcupanel_RegSet(0x83,0x0000);		\
  Mcupanel_RegSet(0x84,0x0000);		\
  Mcupanel_RegSet(0x85,0x0000);		\
  Mcupanel_RegSet(0x90,0x0010);		\
  Mcupanel_RegSet(0x92,0x0000);		\
  mdelay(50); 				\
  Mcupanel_RegSet(0x93,0x0003);		\
  Mcupanel_RegSet(0x95,0x0110);		\
  Mcupanel_RegSet(0x97,0x0000);		\
  Mcupanel_RegSet(0x98,0x0000);		\
  Mcupanel_RegSet(0x07,0x0133);		\
  mdelay(50); 				\
  Mcupanel_Command(0x22);		\
} while (0)


/*---- LCD Initial ----*/
#undef __lcd_slcd_pin_init
#define __lcd_slcd_pin_init()						\
	do {								\
        	__gpio_as_slcd_8bit();					\
        	mdelay(10);						\
		__lcd_special_pin_init();				\
}while (0)

#undef __init_slcd_bus
#define __init_slcd_bus()\
do{\
	__slcd_set_data_8bit_x2();\
	__slcd_set_cmd_8bit();\
	__slcd_set_cs_low();\
	__slcd_set_rs_low();\
	__slcd_set_clk_falling();\
	__slcd_set_parallel_type();\
}while(0)

#undef __lcd_slcd_special_on
#define __lcd_slcd_special_on()						\
	do {								\
		printk("============== ILI9331 SLCD Init.==========\n");					\
		printk("============== ILI9331 SLCD 0.==========\n");					\
		__lcd_slcd_pin_init();					\
		REG_LCD_CTRL &= ~(LCD_CTRL_ENA|LCD_CTRL_DIS); /* disable lcdc */ \
		REG_LCD_CFG = LCD_CFG_LCDPIN_SLCD | 0x0D; /* LCM */	\
		REG_SLCD_CTRL &= ~SLCD_CTRL_DMA_EN; /* disable slcd dma */ \
		REG_SLCD_CFG = SLCD_CFG_DWIDTH_8BIT_x2 | SLCD_CFG_CWIDTH_8BIT | SLCD_CFG_CS_ACTIVE_LOW | SLCD_CFG_RS_CMD_LOW | SLCD_CFG_CLK_ACTIVE_FALLING | SLCD_CFG_TYPE_PARALLEL; \
		REG_LCD_REV = 0x04;	/* lcd clock??? */			\
		printk("============== ILI9331 SLCD 1.==========\n");					\
  		mdelay(500); 				\
		SlcdInit();						\
		printk("============== ILI9331 SLCD 2.==========\n");					\
		Mcupanel_SetAddr(0, 0);				\
  		/*Mcupanel_Command(0x22);*/		\
		/*while ((REG_SLCD_STATE)&(1 << 0));*/		\
		/*__slcd_enable_dma();	*/					\
		REG_SLCD_CTRL |= SLCD_CTRL_DMA_EN; /* slcdc dma enable */ \
		REG_LCD_CTRL  |= (LCD_CTRL_ENA|LCD_CTRL_DIS); /* disable lcdc */ \
	} while (0)

#endif	/* #if CONFIG_JZ4750_SLCD_KGM701A3_TFT_SPFD5420A */

#endif  /* __UMIDO_ILI9331_H__ */
