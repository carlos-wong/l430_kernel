/*
 *  linux/arch/mips/jz4740/board-a320.c
 *
 *  JZ4740 A320 board setup routines.
 *
 *  Copyright (c) 2006-2007  Ingenic Semiconductor Inc.
 *  Copyright (c) 2009       Ignacio Garcia Perez <iggarpe@gmail.com>
 *
 *  Author:   <lhhuang@ingenic.cn>
 *  Modified: <iggarpe@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/mipsregs.h>
#include <asm/reboot.h>

#include <asm/jzsoc.h>

extern void (*jz_timer_callback)(void);

static void dancing(void)
{
	static unsigned int count = 0;

	count ++;
	count &= 1;

	/* TODO(IGP): there's probably no sense in make the A320 dance... */
}

/*
 * This is called by the panic reboot delay loop if panic=<n> parameter
 * is passed to the kernel. The A320 does not have any LEDs, so the best
 * we can do is to blink the LCD backlight.
 *
 * TODO(IGP): this should use the LCD handling code.
 */

static long a320_panic_blink_callback(long time)
{
//	__gpio_as_output(GPIO_LCD_BACKLIGHT);
//	if ((time / 500) & 1)
//		__gpio_set_pin(GPIO_LCD_BACKLIGHT);
//	else 	__gpio_clear_pin(GPIO_LCD_BACKLIGHT);
	return 0;
}

static void a320_timer_callback(void)
{
	static unsigned long count = 0;

	if ((++count) % 50 == 0) {
		dancing();
		count = 0;
	}
}

static void __init board_cpm_setup(void)
{
	/* Stop unused module clocks here.
	 * We have started all module clocks at arch/mips/jz4740/setup.c.
	 */

	/* TODO(IGP): stop the clocks to save power */
}

static void __init board_gpio_setup(void)
{
	/*
	 * Most of the GPIO pins should have been initialized by the boot-loader
	 */

	/*
	 * Initialize MSC pins (not needed, already done in the driver (jz_mmc.c)
	 *
	 * __gpio_as_msc();
	 */

	/*
	 * Initialize I2C pins
	 */
	__gpio_as_i2c();	/* TODO(IGP): this should be moved to the I2C driver code */

	/*
	 * Initialize Other pins
	 */
	__gpio_as_input(GPIO_SD_CD);
	__gpio_disable_pull(GPIO_SD_CD);

	__gpio_as_input(GPIO_USB_DETE);

	__gpio_as_output(GPIO_SND_MUTE_N);	/* TODO(IGP): production kernel should start muted */
	__gpio_set_pin(GPIO_SND_MUTE_N);
	__gpio_as_output(GPIO_SND_UNKNOWN);
	__gpio_set_pin(GPIO_SND_UNKNOWN);
}

void __init jz_board_setup(void)
{
	printk("JZ4740 A320 board setup\n");

	board_cpm_setup();
	board_gpio_setup();

	jz_timer_callback = a320_timer_callback;
	panic_blink = a320_panic_blink_callback;
}
