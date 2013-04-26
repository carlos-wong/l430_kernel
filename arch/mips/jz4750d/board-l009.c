/*
 * linux/arch/mips/jz4750d/board-l009.c
 *
 * Umido L009 board setup routines.
 *
 * Copyright (c) 2006-2008  Ingenic Semiconductor Inc.
 * Author: <jlwei@ingenic.cn>
 * Mod: <maddrone@gmail.com>
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
#include <linux/mmc/host.h>




#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/mipsregs.h>
#include <asm/reboot.h>

#include <asm/jzsoc.h>

void __init board_msc_init(void);

extern int jz_add_msc_devices(unsigned int controller, struct jz_mmc_platform_data *plat);


extern void (*jz_timer_callback)(void);

static void dancing(void)
{
	static unsigned char slash[] = "\\|/-";
//	static volatile unsigned char *p = (unsigned char *)0xb6000058;
	static volatile unsigned char *p = (unsigned char *)0xb6000016;
	static unsigned int count = 0;
	*p = slash[count++];
	count &= 3;
}

static void l009_timer_callback(void)
{
	static unsigned long count = 0;

	if ((++count) % 50 == 0) {
		dancing();
		count = 0;
	}
}
#define CONFIG_JZ_ROOTFS_IN_SD
static void cetus_sd_gpio_init(struct device *dev)
{
	__gpio_as_msc0_4bit();
#ifndef CONFIG_JZ_ROOTFS_IN_SD
	__gpio_as_output(GPIO_SD0_VCC_EN_N);
	__gpio_as_input(GPIO_SD0_CD_N);
#endif

}

static void cetus_sd_power_on(struct device *dev)
{
#ifndef CONFIG_JZ_ROOTFS_IN_SD
	__gpio_clear_pin(GPIO_SD0_VCC_EN_N);
#endif
}

static void cetus_sd_power_off(struct device *dev)
{
#ifndef CONFIG_JZ_ROOTFS_IN_SD
	__gpio_set_pin(GPIO_SD0_VCC_EN_N);
#endif
}

static void cetus_sd_cpm_start(struct device *dev)
{
	__cpm_start_msc(0);
}

static unsigned int cetus_sd_status(struct device *dev)
{
#ifndef CONFIG_JZ_ROOTFS_IN_SD
	unsigned int status;
	status = (unsigned int) __gpio_get_pin(GPIO_SD0_CD_N);
	return (!status);
#else
	return 1;
#endif
}

#if 0
static void cetus_sd_plug_change(int state)
{
	if(state == CARD_INSERTED)
		__gpio_as_irq_high_level(MSC0_HOTPLUG_PIN); /* wait remove */
	else
		__gpio_as_irq_low_level(MSC0_HOTPLUG_PIN); /* wait insert */
}
#else
static void cetus_sd_plug_change(int state)
{
#ifndef CONFIG_JZ_ROOTFS_IN_SD
	if(state == CARD_INSERTED)
		__gpio_as_irq_rise_edge(MSC0_HOTPLUG_PIN);
	else
		__gpio_as_irq_fall_edge(MSC0_HOTPLUG_PIN);
#endif
}
#endif

static unsigned int cetus_sd_get_wp(struct device *dev)
{
#ifndef CONFIG_JZ_ROOTFS_IN_SD
	unsigned int status;

	status = (unsigned int) __gpio_get_pin(GPIO_SD0_WP);
	return (status);
#else
	return 0;
#endif
}
#ifdef CONFIG_JZ_RECOVERY
struct mmc_partition_info cetus_partitions[] = {
	[0] = {"mbr",0,512,0},
	[1] = {"uboot",512,3*1024*1024-512,0}, 
	[2] = {"misc",0x300000,0x100000,0},
	[3] = {"kernel",0x400000,0x400000,0},
	[4] = {"recovery",0x800000,0x400000,0},

	[5] = {"rootfs",12*1024*1024,100*1024*1024,1}, 
	[6] = {"ebookfs",112*1024*1024,60*1024*1024,1},
	[7] = {"qtefs",172*1024*1024,80*1024*1024,1},
	[8] = {"data",252*1024*1024,3500*0x100000,0},
};
#endif
struct jz_mmc_platform_data cetus_sd_data = {
#ifndef CONFIG_JZ_MSC0_SDIO_SUPPORT
	.support_sdio   = 0,
#else
	.support_sdio   = 1,
#endif
	.ocr_mask	= MMC_VDD_32_33 | MMC_VDD_33_34,
//	.status_irq	= MSC0_HOTPLUG_IRQ,
//	.detect_pin     = GPIO_SD0_CD_N,
	.init           = cetus_sd_gpio_init,
	.power_on       = cetus_sd_power_on,
	.power_off      = cetus_sd_power_off,
	.cpm_start	= cetus_sd_cpm_start,
	.status		= cetus_sd_status,
	.plug_change	= cetus_sd_plug_change,
	//.write_protect  = cetus_sd_get_wp,
	.max_bus_width  = MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED | MMC_CAP_4_BIT_DATA,
#ifdef CONFIG_JZ_MSC0_BUS_1
	.bus_width      = 1,
#elif defined  CONFIG_JZ_MSC0_BUS_4
	.bus_width      = 4,
#else
	.bus_width      = 8,
#endif
#ifdef CONFIG_JZ_RECOVERY
	.partitions = cetus_partitions,
	.num_partitions = ARRAY_SIZE(cetus_partitions),
#endif
};

static void cetus_tf_gpio_init(struct device *dev)
{
	__gpio_as_msc1_1bit();
	__gpio_as_output(GPIO_SD1_VCC_EN_N);
	__gpio_as_input(GPIO_SD1_CD_N);
}

static void cetus_tf_power_on(struct device *dev)
{
	__msc1_enable_power();
}

static void cetus_tf_power_off(struct device *dev)
{
	__msc1_disable_power();
}

static void cetus_tf_cpm_start(struct device *dev)
{
	__cpm_start_msc(1);
}

static unsigned int cetus_tf_status(struct device *dev)
{
	unsigned int status;

	status = (unsigned int) __gpio_get_pin(GPIO_SD1_CD_N);
	return (!status);
}

#if 0
static void cetus_tf_plug_change(int state)
{
	if(state == CARD_INSERTED)
		__gpio_as_irq_low_level(MSC1_HOTPLUG_PIN);
	else
		__gpio_as_irq_high_level(MSC1_HOTPLUG_PIN);
}
#else
static void cetus_tf_plug_change(int state)
{
  if(MSC1_HOTPLUG_PIN)
  {
  //printk("%d:%s state is %d\n",__LINE__,__FILE__,state);
	if(state == CARD_INSERTED)
		__gpio_as_irq_fall_edge(MSC1_HOTPLUG_PIN);
	else
		__gpio_as_irq_rise_edge(MSC1_HOTPLUG_PIN);
  }
}
#endif

struct jz_mmc_platform_data cetus_tf_data = {
#ifndef CONFIG_JZ_MSC1_SDIO_SUPPORT
	.support_sdio   = 0,
#else
	.support_sdio   = 1,
#endif
	.ocr_mask	= MMC_VDD_32_33 | MMC_VDD_33_34,
	.status_irq	= MSC1_HOTPLUG_IRQ,
	.detect_pin     = GPIO_SD1_CD_N,
	.init           = cetus_tf_gpio_init,
	.power_on       = cetus_tf_power_on,
	.power_off      = cetus_tf_power_off,
	.cpm_start	= cetus_tf_cpm_start,
	.status		= cetus_tf_status,
	.plug_change	= cetus_tf_plug_change,
	.max_bus_width  = MMC_CAP_SD_HIGHSPEED | MMC_CAP_4_BIT_DATA,
#ifdef CONFIG_JZ_MSC1_BUS_1
	.bus_width      = 1,
#else
	.bus_width      = 4,
#endif
};



static void __init board_cpm_setup(void)
{
	/* Stop unused module clocks here.
	 * We have started all module clocks at arch/mips/jz4750d/setup.c.
	 */

	/* TODO (maddrone) stop cpms to save power */
}

static void __init board_gpio_setup(void)
{
	__gpio_as_output(GPIO_AMPEN);
	__gpio_clear_pin(GPIO_AMPEN);
#ifdef GPIO_CHARGE
	__gpio_as_output(GPIO_CHARGE);
	__gpio_set_pin(GPIO_CHARGE);
#endif
	/*
	 * Initialize SDRAM pins
	 */
}
void __init board_msc_init(void)
{
#ifdef CONFIG_JZ_MSC0
	jz_add_msc_devices(0, &cetus_sd_data);
#endif

#ifdef CONFIG_JZ_MSC1
	jz_add_msc_devices(1, &cetus_tf_data);
#endif
}

void __init jz_board_setup(void)
{
	printk("Umido L009 board setup %s %s\n",__DATE__,__TIME__);

	board_cpm_setup();
	board_gpio_setup();

	jz_timer_callback = l009_timer_callback;
}
