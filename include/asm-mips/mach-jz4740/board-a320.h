/*
 *  linux/include/asm-mips/mach-jz4740/board-a320.h
 *
 *  JZ4740-based A320 board definition.
 *
 *  Copyright (C) 2006 - 2007 Ingenic Semiconductor Inc.
 *  Copyright (C) 2009        Ignacio Garcia Perez <iggarpe@gmail.com>
 *
 *  Author:   <lhhuang@ingenic.cn>
 *  Modified: <iggarpe@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4740_A320_H__
#define __ASM_JZ4740_A320_H__

/*====================================================================== 
 * Installed memory
 */
#define MEMSIZE			0x02000000

/*====================================================================== 
 * Frequencies of on-board oscillators
 */
#define JZ_EXTAL		12000000  /* Main extal freq: 12 MHz */
#define JZ_EXTAL2		32768     /* RTC extal freq: 32.768 KHz */

/*====================================================================== 
 * GPIO
 */
#define GPIO_SD_CD		61	/* GPB29 */
#define GPIO_USB_DETE		124	/* GPD28 */
#define GPIO_SND_MUTE_N		91	/* GPC27 */
#define GPIO_SND_UNKNOWN	103	/* GPD7 */
#define GPIO_LCD_BACKLIGHT	127	/* GPD127 */
#define GPIO_UDC_HOTPLUG	GPIO_USB_DETE

/*====================================================================== 
 * MMC/SD
 */

#define MSC_HOTPLUG_PIN		GPIO_SD_CD
#define MSC_HOTPLUG_IRQ		(IRQ_GPIO_0 + GPIO_SD_CD)

/* TODO(IGP): make sure the SD/MMC card power is always on in the A320 */

#define __msc_init_io()				\
do {						\
	__gpio_as_input(GPIO_SD_CD);		\
} while (0)

#define __msc_enable_power()			\
do {						\
} while (0)

#define __msc_disable_power()			\
do {						\
} while (0)

#define __msc_card_detected(s)			\
({						\
	int detected = 1;			\
	if (!__gpio_get_pin(GPIO_SD_CD))	\
		detected = 0;			\
	detected;				\
})

#endif /* __ASM_JZ4740_A320_H__ */
