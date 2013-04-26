/*
 *  linux/drivers/video/jzslcd.h -- Ingenic On-Chip SLCD frame buffer device
 *
 *  Copyright (C) 2005-2007 Ingenic Semiconductor Inc.
 *  Copyright (C) 2009      Ignacio Garcia Perez <iggarpe@gmail.com>
 *
 *  Author:
 *  Modified: <iggarpe@gmail.com> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __JZ4740_SLCD_H__
#define __JZ4740_SLCD_H__

#define UINT16 unsigned short
#define UINT32 unsigned int

#define NR_PALETTE	256
/* Jz LCDFB supported I/O controls. */
#define FBIOSETBACKLIGHT	0x4688
#define FBIODISPON		0x4689
#define FBIODISPOFF		0x468a
#define FBIORESET		0x468b
#define FBIOPRINT_REG		0x468c
#define FBIO_REFRESH_ALWAYS	0x468d
#define FBIO_REFRESH_EVENTS	0x468e
#define FBIO_DO_REFRESH		0x468f
#define FBIO_SET_REG		0x4690



/*
 * Dingoo A320 IL9325 specific stuff.
 * Reverse engineered from A320_PD27_ILI9325_RLS.DL
 * found in the first released unbricking tool.
 * Only 16 bit bus supported.
 */

#ifdef CONFIG_JZ_SLCD_A320_ILI9325
#define PIN_RS_N	(32*2+19)	/* Port 2 pin 19: RS# (register select, active low) */
#define PIN_CS_N	(32*1+17)	/* Port 1 pin 17: CS# (chip select, active low) */
#define PIN_RESET_N	(32*1+18)	/* Port 1 pin 18: RESET# (reset, active low) */

#define	__slcd_special_pin_init()	\
do {					\
	__gpio_as_output(PIN_RS_N);	\
	__gpio_set_pin(PIN_RS_N);	\
	__gpio_as_output(PIN_CS_N);	\
	__gpio_set_pin(PIN_CS_N);	\
	__gpio_as_output(PIN_RESET_N);	\
	__gpio_clear_pin(PIN_RESET_N);	\
} while(0)

#define __slcd_special_on() 		\
do {					\
	/* RESET pulse */		\
	__gpio_clear_pin(PIN_RESET_N);	\
	mdelay(10);			\
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(50);			\
					\
	/* Enable chip select */	\
	__gpio_clear_pin(PIN_CS_N);	\
					\
	/* Black magic */		\
	Mcupanel_RegSet(0xE3,0x3008);	\
	Mcupanel_RegSet(0xE7,0x0012);	\
	Mcupanel_RegSet(0xEF,0x1231);	\
	Mcupanel_RegSet(0x01,0x0100);	\
	Mcupanel_RegSet(0x02,0x0700);	\
	Mcupanel_RegSet(0x03,0x1098);	\
	Mcupanel_RegSet(0x04,0x0000);	\
	Mcupanel_RegSet(0x08,0x0207);	\
	Mcupanel_RegSet(0x09,0x0000);	\
	Mcupanel_RegSet(0x0A,0x0000);	\
	Mcupanel_RegSet(0x0C,0x0000);	\
	Mcupanel_RegSet(0x0D,0x0000);	\
	Mcupanel_RegSet(0x0F,0x0000);	\
	Mcupanel_RegSet(0x10,0x0000);	\
	Mcupanel_RegSet(0x11,0x0007);	\
	Mcupanel_RegSet(0x12,0x0000);	\
	Mcupanel_RegSet(0x13,0x0000);	\
	mdelay(200);			\
	Mcupanel_RegSet(0x10,0x1290);	\
	Mcupanel_RegSet(0x11,0x0227);	\
	mdelay(50);			\
	Mcupanel_RegSet(0x12,0x001B);	\
	mdelay(50);			\
	Mcupanel_RegSet(0x13,0x0500);	\
	Mcupanel_RegSet(0x29,0x000C);	\
	Mcupanel_RegSet(0x2B,0x000D);	\
	mdelay(50);			\
	Mcupanel_RegSet(0x20,0x0000);	\
	Mcupanel_RegSet(0x21,0x0000);	\
	Mcupanel_RegSet(0x30,0x0000);	\
	Mcupanel_RegSet(0x31,0x0204);	\
	Mcupanel_RegSet(0x32,0x0200);	\
	Mcupanel_RegSet(0x35,0x0007);	\
	Mcupanel_RegSet(0x36,0x1404);	\
	Mcupanel_RegSet(0x37,0x0705);	\
	Mcupanel_RegSet(0x38,0x0305);	\
	Mcupanel_RegSet(0x39,0x0707);	\
	Mcupanel_RegSet(0x3C,0x0701);	\
	Mcupanel_RegSet(0x3D,0x000E);	\
	Mcupanel_RegSet(0x50,0x0000);	\
	Mcupanel_RegSet(0x51,0x00EF);	\
	Mcupanel_RegSet(0x52,0x0000);	\
	Mcupanel_RegSet(0x53,0x013F);	\
	Mcupanel_RegSet(0x60,0xA700);	\
	Mcupanel_RegSet(0x61,0x0001);	\
	Mcupanel_RegSet(0x6A,0x0000);	\
	Mcupanel_RegSet(0x80,0x0000);	\
	Mcupanel_RegSet(0x81,0x0000);	\
	Mcupanel_RegSet(0x82,0x0000);	\
	Mcupanel_RegSet(0x83,0x0000);	\
	Mcupanel_RegSet(0x84,0x0000);	\
	Mcupanel_RegSet(0x85,0x0000);	\
	Mcupanel_RegSet(0x90,0x0010);	\
	Mcupanel_RegSet(0x92,0x0600);	\
	mdelay(50);			\
	Mcupanel_RegSet(0x07,0x0133);	\
	mdelay(50);			\
	Mcupanel_Command(0x22);		\
} while (0)

/* TODO(IGP): make sure LCD power consumption is low in these conditions */

#define __slcd_special_off()		\
do {					\
	/* Keep chip select disabled */	\
	__gpio_set_pin(PIN_CS_N);	\
	/* Keep RESET active */		\
	__gpio_clear_pin(PIN_RESET_N);	\
} while (0)

#define __slcd_special_rs_enable()	\
do {					\
	__gpio_clear_pin(PIN_RS_N);	\
} while (0)

#define __slcd_special_rs_disable()	\
do {					\
	__gpio_set_pin(PIN_RS_N);	\
} while(0)

#endif /* CONFIG_JZ_SLCD_A320_ILI9325 */



/*
 * Dingoo A320 IL9331 specific stuff.
 * Reverse engineered from CCPMP_CFG_A320_LCM_FAIR_ILI9331_320_240.DL
 * found in the second released unbricking tool.
 * Only 16 bit bus supported.
 */

#ifdef CONFIG_JZ_SLCD_A320_ILI9331
#define PIN_RS_N	(32*2+19)	/* Port 2 pin 19: RS# (register select, active low) */
#define PIN_CS_N	(32*1+17)	/* Port 1 pin 17: CS# (chip select, active low) */
#define PIN_RESET_N	(32*1+18)	/* Port 1 pin 18: RESET# (reset, active low) */

#define	__slcd_special_pin_init()	\
do {					\
	__gpio_as_output(PIN_RS_N);	\
	__gpio_set_pin(PIN_RS_N);	\
	__gpio_as_output(PIN_CS_N);	\
	__gpio_set_pin(PIN_CS_N);	\
	__gpio_as_output(PIN_RESET_N);	\
	__gpio_clear_pin(PIN_RESET_N);	\
} while(0)

#define __slcd_special_on() 		\
do {					\
	/* RESET pulse */		\
	__gpio_clear_pin(PIN_RESET_N);	\
	mdelay(10);			\
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(50);			\
					\
	/* Enable chip select */	\
	__gpio_clear_pin(PIN_CS_N);	\
					\
	/* Black magic */		\
	Mcupanel_RegSet(0xE7, 0x1014);	\
	Mcupanel_RegSet(0x01, 0x0000);	\
	Mcupanel_RegSet(0x02, 0x0200);	\
	Mcupanel_RegSet(0x03, 0x1048);	\
	Mcupanel_RegSet(0x08, 0x0202);	\
	Mcupanel_RegSet(0x09, 0x0000);	\
	Mcupanel_RegSet(0x0A, 0x0000);	\
	Mcupanel_RegSet(0x0C, 0x0000);	\
	Mcupanel_RegSet(0x0D, 0x0000);	\
	Mcupanel_RegSet(0x0F, 0x0000);	\
	Mcupanel_RegSet(0x10, 0x0000);	\
	Mcupanel_RegSet(0x11, 0x0007);	\
	Mcupanel_RegSet(0x12, 0x0000);	\
	Mcupanel_RegSet(0x13, 0x0000);	\
	mdelay(100);			\
	Mcupanel_RegSet(0x10, 0x1690);	\
	Mcupanel_RegSet(0x11, 0x0224);	\
	mdelay(50);			\
	Mcupanel_RegSet(0x12, 0x001F);	\
	mdelay(50);			\
	Mcupanel_RegSet(0x13, 0x0500);	\
	Mcupanel_RegSet(0x29, 0x000C);	\
	Mcupanel_RegSet(0x2B, 0x000D);	\
	mdelay(50);			\
	Mcupanel_RegSet(0x30, 0x0000);	\
	Mcupanel_RegSet(0x31, 0x0106);	\
	Mcupanel_RegSet(0x32, 0x0000);	\
	Mcupanel_RegSet(0x35, 0x0204);	\
	Mcupanel_RegSet(0x36, 0x160A);	\
	Mcupanel_RegSet(0x37, 0x0707);	\
	Mcupanel_RegSet(0x38, 0x0106);	\
	Mcupanel_RegSet(0x39, 0x0706);	\
	Mcupanel_RegSet(0x3C, 0x0402);	\
	Mcupanel_RegSet(0x3D, 0x0C0F);	\
	Mcupanel_RegSet(0x50, 0x0000);	\
	Mcupanel_RegSet(0x51, 0x00EF);	\
	Mcupanel_RegSet(0x52, 0x0000);	\
	Mcupanel_RegSet(0x53, 0x013F);	\
	Mcupanel_RegSet(0x20, 0x0000);	\
	Mcupanel_RegSet(0x21, 0x0000);	\
	Mcupanel_RegSet(0x60, 0x2700);	\
	Mcupanel_RegSet(0x61, 0x0001);	\
	Mcupanel_RegSet(0x6A, 0x0000);	\
	Mcupanel_RegSet(0x80, 0x0000);	\
	Mcupanel_RegSet(0x81, 0x0000);	\
	Mcupanel_RegSet(0x82, 0x0000);	\
	Mcupanel_RegSet(0x83, 0x0000);	\
	Mcupanel_RegSet(0x84, 0x0000);	\
	Mcupanel_RegSet(0x85, 0x0000);	\
	Mcupanel_RegSet(0x20, 0x00EF);	\
	Mcupanel_RegSet(0x21, 0x0190);	\
	Mcupanel_RegSet(0x90, 0x0010);	\
	Mcupanel_RegSet(0x92, 0x0600);	\
	Mcupanel_RegSet(0x07, 0x0133);	\
	Mcupanel_Command(0x22);		\
} while (0)

/* TODO(IGP): make sure LCD power consumption is low in these conditions */

#define __slcd_special_off()		\
do {					\
	/* Keep chip select disabled */	\
	__gpio_set_pin(PIN_CS_N);	\
	/* Keep RESET active */		\
	__gpio_clear_pin(PIN_RESET_N);	\
} while (0)

#define __slcd_special_rs_enable()	\
do {					\
	__gpio_clear_pin(PIN_RS_N);	\
} while (0)

#define __slcd_special_rs_disable()	\
do {					\
	__gpio_set_pin(PIN_RS_N);	\
} while(0)

#endif /* CONFIG_JZ_SLCD_A320_ILI9331 */



#ifdef CONFIG_JZ_SLCD_LGDP4551
#define PIN_CS_N 	(32*2+18)	/* Chip select      :SLCD_WR: GPC18 */ 
#define PIN_RESET_N 	(32*2+21)	/* LCD reset        :SLCD_RST: GPC21*/ 
#define PIN_RS_N 	(32*2+19)

#define	__slcd_special_pin_init() \
do { \
	__gpio_as_output(PIN_CS_N); 	\
	__gpio_as_output(PIN_RESET_N); 	\
	__gpio_clear_pin(PIN_CS_N); /* Clear CS */\
	mdelay(100);			\
} while(0)

#define __slcd_special_on() 		\
do {	/* RESET# */			\
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(10);			\
	__gpio_clear_pin(PIN_RESET_N);	\
	mdelay(10);			\
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(100);			\
	Mcupanel_RegSet(0x0015,0x0050);	\
	Mcupanel_RegSet(0x0011,0x0000);	\
	Mcupanel_RegSet(0x0010,0x3628);	\
	Mcupanel_RegSet(0x0012,0x0002);	\
	Mcupanel_RegSet(0x0013,0x0E47);	\
	udelay(100);			\
	Mcupanel_RegSet(0x0012,0x0012);	\
	udelay(100);			\
	Mcupanel_RegSet(0x0010,0x3620);	\
	Mcupanel_RegSet(0x0013,0x2E47);	\
	udelay(50);			\
	Mcupanel_RegSet(0x0030,0x0000);	\
	Mcupanel_RegSet(0x0031,0x0502);	\
	Mcupanel_RegSet(0x0032,0x0307);	\
	Mcupanel_RegSet(0x0033,0x0304);	\
	Mcupanel_RegSet(0x0034,0x0004);	\
	Mcupanel_RegSet(0x0035,0x0401);	\
	Mcupanel_RegSet(0x0036,0x0707);	\
	Mcupanel_RegSet(0x0037,0x0303);	\
	Mcupanel_RegSet(0x0038,0x1E02);	\
	Mcupanel_RegSet(0x0039,0x1E02);	\
	Mcupanel_RegSet(0x0001,0x0000);	\
	Mcupanel_RegSet(0x0002,0x0300);	\
	if (jzfb.bpp == 16)		\
		Mcupanel_RegSet(0x0003,0x10B8); /*8-bit system interface two transfers
						  up:0x10B8 down:0x1088 left:0x1090 right:0x10a0*/ \
	else	\
		if (jzfb.bpp == 32)\
			Mcupanel_RegSet(0x0003,0xD0B8);/*8-bit system interface three transfers,666
							 up:0xD0B8 down:0xD088 left:0xD090 right:0xD0A0*/ \
	Mcupanel_RegSet(0x0008,0x0204);\
	Mcupanel_RegSet(0x000A,0x0008);\
	Mcupanel_RegSet(0x0060,0x3100);\
	Mcupanel_RegSet(0x0061,0x0001);\
	Mcupanel_RegSet(0x0090,0x0052);\
	Mcupanel_RegSet(0x0092,0x000F);\
	Mcupanel_RegSet(0x0093,0x0001);\
	Mcupanel_RegSet(0x009A,0x0008);\
	Mcupanel_RegSet(0x00A3,0x0010);\
	Mcupanel_RegSet(0x0050,0x0000);\
	Mcupanel_RegSet(0x0051,0x00EF);\
	Mcupanel_RegSet(0x0052,0x0000);\
	Mcupanel_RegSet(0x0053,0x018F);\
	/*===Display_On_Function=== */ \
	Mcupanel_RegSet(0x0007,0x0001);\
	Mcupanel_RegSet(0x0007,0x0021);\
	Mcupanel_RegSet(0x0007,0x0023);\
	Mcupanel_RegSet(0x0007,0x0033);\
	Mcupanel_RegSet(0x0007,0x0133);\
	Mcupanel_Command(0x0022);/*Write Data to GRAM	*/  \
	udelay(1);		\
	Mcupanel_SetAddr(0,0);	\
	mdelay(100);		\
} while (0)

#define __slcd_special_off() 		\
do { \
} while(0)
#endif /*CONFIG_JZ_SLCD_LGDP4551_xxBUS*/



#ifdef CONFIG_JZ_SLCD_SPFD5420A

  //#define PIN_CS_N 	(32*2+18)	// Chip select 	//GPC18;
#define PIN_CS_N 	(32*2+22)	// Chip select 	//GPC18;
#define PIN_RESET_N 	(32*1+18)	// LCD reset   	//GPB18;
#define PIN_RS_N 	(32*2+19)	// LCD RS		//GPC19;
#define PIN_POWER_N	(32*3+0)	//Power off 	//GPD0;
#define PIN_FMARK_N	(32*3+1)	//fmark			//GPD1;

#define GAMMA()	\
do {	\
	Mcupanel_RegSet(0x0300,0x0101);	\
	Mcupanel_RegSet(0x0301,0x0b27);	\
	Mcupanel_RegSet(0x0302,0x132a);	\
	Mcupanel_RegSet(0x0303,0x2a13);	\
	Mcupanel_RegSet(0x0304,0x270b);	\
	Mcupanel_RegSet(0x0305,0x0101);	\
	Mcupanel_RegSet(0x0306,0x1205);	\
	Mcupanel_RegSet(0x0307,0x0512);	\
	Mcupanel_RegSet(0x0308,0x0005);	\
	Mcupanel_RegSet(0x0309,0x0003);	\
	Mcupanel_RegSet(0x030a,0x0f04);	\
	Mcupanel_RegSet(0x030b,0x0f00);	\
	Mcupanel_RegSet(0x030c,0x000f);	\
	Mcupanel_RegSet(0x030d,0x040f);	\
	Mcupanel_RegSet(0x030e,0x0300);	\
	Mcupanel_RegSet(0x030f,0x0500);	\
	/*** secorrect gamma2 ***/	\
	Mcupanel_RegSet(0x0400,0x3500);	\
	Mcupanel_RegSet(0x0401,0x0001);	\
	Mcupanel_RegSet(0x0404,0x0000);	\
	Mcupanel_RegSet(0x0500,0x0000);	\
	Mcupanel_RegSet(0x0501,0x0000);	\
	Mcupanel_RegSet(0x0502,0x0000);	\
	Mcupanel_RegSet(0x0503,0x0000);	\
	Mcupanel_RegSet(0x0504,0x0000);	\
	Mcupanel_RegSet(0x0505,0x0000);	\
	Mcupanel_RegSet(0x0600,0x0000);	\
	Mcupanel_RegSet(0x0606,0x0000);	\
	Mcupanel_RegSet(0x06f0,0x0000);	\
	Mcupanel_RegSet(0x07f0,0x5420);	\
	Mcupanel_RegSet(0x07f3,0x288a);	\
	Mcupanel_RegSet(0x07f4,0x0022);	\
	Mcupanel_RegSet(0x07f5,0x0001);	\
	Mcupanel_RegSet(0x07f0,0x0000);	\
} while(0)

#define __slcd_special_on()	\
do {      \
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(10);	\
	__gpio_clear_pin(PIN_RESET_N);	\
	mdelay(10);	\
	__gpio_set_pin(PIN_RESET_N);	\
	mdelay(100);	\
	if (jzfb.bus == 18) {\
	Mcupanel_RegSet(0x0606,0x0000);	\
	udelay(10);	\
	Mcupanel_RegSet(0x0007,0x0001);	\
	udelay(10);	\
	Mcupanel_RegSet(0x0110,0x0001);	\
	udelay(10);	\
	Mcupanel_RegSet(0x0100,0x17b0);	\
	Mcupanel_RegSet(0x0101,0x0147);	\
	Mcupanel_RegSet(0x0102,0x019d);	\
	Mcupanel_RegSet(0x0103,0x8600);	\
	Mcupanel_RegSet(0x0281,0x0010);	\
	udelay(10);	\
	Mcupanel_RegSet(0x0102,0x01bd);	\
	udelay(10);	\
	/************initial************/\
	Mcupanel_RegSet(0x0000,0x0000);	\
	Mcupanel_RegSet(0x0001,0x0000);	\
	Mcupanel_RegSet(0x0002,0x0400);	\
	Mcupanel_RegSet(0x0003,0x1288); /*up:0x1288 down:0x12B8 left:0x1290 right:0x12A0*/ \
	Mcupanel_RegSet(0x0006,0x0000);	\
	Mcupanel_RegSet(0x0008,0x0503);	\
	Mcupanel_RegSet(0x0009,0x0001);	\
	Mcupanel_RegSet(0x000b,0x0010);	\
	Mcupanel_RegSet(0x000c,0x0000);	\
	Mcupanel_RegSet(0x000f,0x0000);	\
	Mcupanel_RegSet(0x0007,0x0001);	\
	Mcupanel_RegSet(0x0010,0x0010);	\
	Mcupanel_RegSet(0x0011,0x0202);	\
	Mcupanel_RegSet(0x0012,0x0300);	\
	Mcupanel_RegSet(0x0020,0x021e);	\
	Mcupanel_RegSet(0x0021,0x0202);	\
	Mcupanel_RegSet(0x0022,0x0100);	\
	Mcupanel_RegSet(0x0090,0x0000);	\
	Mcupanel_RegSet(0x0092,0x0000);	\
	Mcupanel_RegSet(0x0100,0x16b0);	\
	Mcupanel_RegSet(0x0101,0x0147);	\
	Mcupanel_RegSet(0x0102,0x01bd);	\
	Mcupanel_RegSet(0x0103,0x2c00);	\
    	Mcupanel_RegSet(0x0107,0x0000);	\
	Mcupanel_RegSet(0x0110,0x0001);	\
	Mcupanel_RegSet(0x0210,0x0000);	\
	Mcupanel_RegSet(0x0211,0x00ef);	\
	Mcupanel_RegSet(0x0212,0x0000);	\
	Mcupanel_RegSet(0x0213,0x018f);	\
	Mcupanel_RegSet(0x0280,0x0000);	\
	Mcupanel_RegSet(0x0281,0x0001);	\
	Mcupanel_RegSet(0x0282,0x0000);	\
	GAMMA();	\
 	Mcupanel_RegSet(0x0007,0x0173);	\
	} else {		\
		Mcupanel_RegSet(0x0600, 0x0001);   /*soft reset*/	\
		mdelay(10); 		\
		Mcupanel_RegSet(0x0600, 0x0000);   /*soft reset*/	\
		mdelay(10);						\
		Mcupanel_RegSet(0x0606, 0x0000);   /*i80-i/F Endian Control*/ \
		/*===User setting===    */				\
		Mcupanel_RegSet(0x0001, 0x0000);/* Driver Output Control-----0x0100 SM(bit10) | 0x400*/ \
		Mcupanel_RegSet(0x0002, 0x0100);   /*LCD Driving Wave Control      0x0100 */ \
		if (jzfb.bpp == 16)					\
			Mcupanel_RegSet(0x0003, 0x50A8);/*Entry Mode 0x1030*/ \
		else /*bpp = 18*/					\
			Mcupanel_RegSet(0x0003, 0x1010 | 0xC8);   /*Entry Mode 0x1030*/	\
		/*#endif								*/ \
		Mcupanel_RegSet(0x0006, 0x0000);   /*Outline Sharpening Control*/\     
		Mcupanel_RegSet(0x0008, 0x0808);   /*Sets the number of lines for front/back porch period*/\
		Mcupanel_RegSet(0x0009, 0x0001);   /*Display Control 3   */\ 
		Mcupanel_RegSet(0x000B, 0x0010);   /*Low Power Control*/\
		Mcupanel_RegSet(0x000C, 0x0000);   /*External Display Interface Control 1 0x0001 */\
		Mcupanel_RegSet(0x000F, 0x0000);   /*External Display Interface Control 2         */\
		Mcupanel_RegSet(0x0400, 0xB104);/*Base Image Number of Line---GS(bit15) | 0x8000*/ \
		Mcupanel_RegSet(0x0401, 0x0001);   /*Base Image Display        0x0001*/\
		Mcupanel_RegSet(0x0404, 0x0000);   /*Base Image Vertical Scroll Control    0x0000*/\
		Mcupanel_RegSet(0x0500, 0x0000);   /*Partial Image 1: Display Position*/\
		Mcupanel_RegSet(0x0501, 0x0000);   /*RAM Address (Start Line Address) */\
		Mcupanel_RegSet(0x0502, 0x018f);   /*RAM Address (End Line Address)  */	\
		Mcupanel_RegSet(0x0503, 0x0000);   /*Partial Image 2: Display Position  RAM Address*/\
		Mcupanel_RegSet(0x0504, 0x0000);   /*RAM Address (Start Line Address) */\
		Mcupanel_RegSet(0x0505, 0x0000);   /*RAM Address (End Line Address)*/\
		/*Panel interface control===*/\
		Mcupanel_RegSet(0x0010, 0x0011);   /*Division Ratio,Clocks per Line  14  */\
		mdelay(10); \
		Mcupanel_RegSet(0x0011, 0x0202);   /*Division Ratio,Clocks per Line*/\
		Mcupanel_RegSet(0x0012, 0x0300);   /*Sets low power VCOM drive period.   */\
		mdelay(10); \
		Mcupanel_RegSet(0x0020, 0x021e);   /*Panel Interface Control 4  */\
		Mcupanel_RegSet(0x0021, 0x0202);   /*Panel Interface Control 5 */\
		Mcupanel_RegSet(0x0022, 0x0100);   /*Panel Interface Control 6*/\ 
		Mcupanel_RegSet(0x0090, 0x0000);   /*Frame Marker Control  */\
		Mcupanel_RegSet(0x0092, 0x0000);   /*MDDI Sub-display Control  */\
		/*===Gamma setting===    */\
		Mcupanel_RegSet(0x0300, 0x0101);   /*γ Control*/\
		Mcupanel_RegSet(0x0301, 0x0000);   /*γ Control*/\
		Mcupanel_RegSet(0x0302, 0x0016);   /*γ Control*/\
		Mcupanel_RegSet(0x0303, 0x2913);   /*γ Control*/\
		Mcupanel_RegSet(0x0304, 0x260B);   /*γ Control*/\
		Mcupanel_RegSet(0x0305, 0x0101);   /*γ Control*/\
		Mcupanel_RegSet(0x0306, 0x1204);   /*γ Control*/\
		Mcupanel_RegSet(0x0307, 0x0415);   /*γ Control*/\
		Mcupanel_RegSet(0x0308, 0x0205);   /*γ Control*/\
		Mcupanel_RegSet(0x0309, 0x0303);   /*γ Control*/\
		Mcupanel_RegSet(0x030a, 0x0E05);   /*γ Control*/\
		Mcupanel_RegSet(0x030b, 0x0D01);   /*γ Control*/\
		Mcupanel_RegSet(0x030c, 0x010D);   /*γ Control*/\
		Mcupanel_RegSet(0x030d, 0x050E);   /*γ Control*/\
		Mcupanel_RegSet(0x030e, 0x0303);   /*γ Control*/\
		Mcupanel_RegSet(0x030f, 0x0502);   /*γ Control*/\
		/*===Power on sequence===*/\
		Mcupanel_RegSet(0x0007, 0x0001);   /*Display Control 1*/\
		Mcupanel_RegSet(0x0110, 0x0001);   /*Power supply startup enable bit*/\
		Mcupanel_RegSet(0x0112, 0x0060);   /*Power Control 7*/\
		Mcupanel_RegSet(0x0100, 0x16B0);   /*Power Control 1 */\
		Mcupanel_RegSet(0x0101, 0x0115);   /*Power Control 2*/\
		Mcupanel_RegSet(0x0102, 0x0119);   /*Starts VLOUT3,Sets the VREG1OUT.*/\
		mdelay(50); \
		Mcupanel_RegSet(0x0103, 0x2E00);   /*set the amplitude of VCOM*/\
		mdelay(50);\
		Mcupanel_RegSet(0x0282, 0x0093);/*0x008E);0x0093);   VCOMH voltage*/\
		Mcupanel_RegSet(0x0281, 0x000A);   /*Selects the factor of VREG1OUT to generate VCOMH. */\
		Mcupanel_RegSet(0x0102, 0x01BE);   /*Starts VLOUT3,Sets the VREG1OUT.*/\
		mdelay(10);\
		/*Address */\
		Mcupanel_RegSet(0x0210, 0x0000);   /*Window Horizontal RAM Address Start*/\
		Mcupanel_RegSet(0x0211, 0x00ef);   /*Window Horizontal RAM Address End*/\
		Mcupanel_RegSet(0x0212, 0x0000);   /*Window Vertical RAM Address Start*/\
		Mcupanel_RegSet(0x0213, 0x018f);   /*Window Vertical RAM Address End */\
		Mcupanel_RegSet(0x0200, 0x0000);   /*RAM Address Set (Horizontal Address)*/\
		Mcupanel_RegSet(0x0201, 0x018f);   /*RAM Address Set (Vertical Address)*/ \
		/*===Display_On_Function===*/\
		Mcupanel_RegSet(0x0007, 0x0021);   /*Display Control 1 */\
		mdelay(50);   /*40*/\
		Mcupanel_RegSet(0x0007, 0x0061);   /*Display Control 1 */\
		mdelay(50);   /*100*/\
		Mcupanel_RegSet(0x0007, 0x0173);   /*Display Control 1 */\
		mdelay(50);   /*300*/\
	}\
	  Mcupanel_Command(0x0202);                  /*Write Data to GRAM	*/  \
	udelay(10);\
	Mcupanel_SetAddr(0,0);\
	udelay(100);\
} while(0)

#define __slcd_special_pin_init() \
do {	\
	__gpio_as_output(PIN_CS_N);	\
	__gpio_as_output(PIN_RESET_N);	\
	__gpio_clear_pin(PIN_CS_N); /* Clear CS */	\
	__gpio_as_output(PIN_POWER_N);	\
	mdelay(100);	\
} while(0)

#endif /*CONFIG_JZ_SLCD_SPFD5420A*/



/*
 * Defaults for undefined functions
 */
#ifndef __slcd_special_pin_init
#define __slcd_special_pin_init()
#endif
#ifndef __slcd_special_on
#define __slcd_special_on()
#endif
#ifndef __slcd_special_off
#define __slcd_special_off()
#endif
#ifndef __slcd_special_rs_enable
#define __slcd_special_rs_enable()
#endif
#ifndef __slcd_special_rs_disable
#define __slcd_special_rs_disable()
#endif



#ifdef CONFIG_JZ4740_A320
/* 100 level: 0,1,...,100 */
#define GPIO_PWM	127	/* GPD31 */
#define PWM_CHN		7	/* PWM channel */
#define PWM_FULL	101
#define __slcd_set_backlight_level(n)			\
do {							\
	u32 v = __cpm_get_extalclk() / 1000;		\
	__gpio_as_pwm(7);				\
	__tcu_disable_pwm_output(PWM_CHN);		\
	__tcu_stop_counter(PWM_CHN);			\
	__tcu_init_pwm_output_high(PWM_CHN);		\
	__tcu_set_pwm_output_shutdown_abrupt(PWM_CHN);	\
	__tcu_select_clk_div1(PWM_CHN);			\
	__tcu_mask_full_match_irq(PWM_CHN);		\
	__tcu_mask_half_match_irq(PWM_CHN);		\
	__tcu_set_count(PWM_CHN, 0);			\
	__tcu_set_full_data(PWM_CHN, v + 1);		\
	__tcu_set_half_data(PWM_CHN, v * n / 100);	\
	__tcu_enable_pwm_output(PWM_CHN);		\
	__tcu_select_extalclk(PWM_CHN);			\
	__tcu_start_counter(PWM_CHN);			\
} while (0)

#define __slcd_close_backlight()	\
do {					\
	__gpio_as_output(GPIO_PWM);	\
	__gpio_clear_pin(GPIO_PWM);	\
} while (0)
#endif /* CONFIG_JZ4740_A320 */



#ifdef CONFIG_JZ4740_PAVO
#define GPIO_PWM	123	/* GPD27 */
#define PWM_CHN		4	/* PWM channel */
#define PWM_FULL	101
/* 100 level: 0,1,...,100 */
#define __slcd_set_backlight_level(n)	\
do {					\
	__gpio_as_output(GPIO_PWM);	\
	__gpio_set_pin(GPIO_PWM);	\
} while (0)

#define __slcd_close_backlight()	\
do {					\
	__gpio_as_output(GPIO_PWM);	\
	__gpio_clear_pin(GPIO_PWM);	\
} while (0)
#endif /* CONFIG_JZ4740_PAVO */



/*
 * Defaults for undefined functions
 */
#ifndef __slcd_set_backlight_level
#define __slcd_set_backlight_level(n)
#endif
#ifndef __slcd_close_backlight
#define __slcd_close_backlight()
#endif



#define __slcd_display_pin_init() \
do { \
	__slcd_special_pin_init(); \
} while (0)

#define __slcd_display_on() \
do { \
	__slcd_special_on(); \
	__slcd_set_backlight_level(80); \
} while (0)

#define __slcd_display_off() \
do { \
	__slcd_special_off(); \
	__slcd_close_backlight(); \
} while (0)

#endif  /*__JZ4740_SLCD_H__*/

