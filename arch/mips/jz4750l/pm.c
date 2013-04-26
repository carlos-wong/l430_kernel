/*
 * linux/arch/mips/jz4750l/common/pm.c
 * 
 * JZ4750L Power Management Routines
 * 
 * Copyright (C) 2006 Ingenic Semiconductor Inc.
 * Author: <jlwei@ingenic.cn>
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 * 
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 * 
 */

#include <linux/init.h>
#include <linux/pm.h>
#include <linux/pm_legacy.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h> 
#include <linux/sysctl.h>

#include <asm/cacheops.h>
#include <asm/jzsoc.h>

#undef DEBUG
//#define DEBUG 
#ifdef DEBUG
#define dprintk(x...)	printk(x)
#else
#define dprintk(x...)
#endif

#define GPIO_PORT_NUM   6

/* 
 * __gpio_as_sleep set all pins to pull-disable, and set all pins as input
 * except sdram and the pins which can be used as CS1_N to CS4_N for chip select. 
 */
#define __gpio_as_sleep()	              \
do {	                                      \
	REG_GPIO_PXFUNC(1) = ~0x03ff7fff;     \
	REG_GPIO_PXSELC(1) = ~0x03ff7fff;     \
	REG_GPIO_PXDIRC(1) = ~0x03ff7fff;     \
	REG_GPIO_PXPES(1)  =  0xffffffff;     \
	REG_GPIO_PXFUNC(2) = ~0x01e00000;     \
	REG_GPIO_PXSELC(2) = ~0x01e00000;     \
	REG_GPIO_PXDIRC(2) = ~0x01e00000;     \
	REG_GPIO_PXPES(2)  =  0xffffffff;     \
	REG_GPIO_PXFUNC(3) =  0xffffffff;     \
	REG_GPIO_PXSELC(3) =  0xffffffff;     \
	REG_GPIO_PXDIRC(3) =  0xffffffff;     \
	REG_GPIO_PXPES(3)  =  0xffffffff;     \
	REG_GPIO_PXFUNC(4) =  0xffffffff;     \
	REG_GPIO_PXSELC(4) =  0xffffffff;     \
	REG_GPIO_PXDIRC(4) =  0xffffffff;     \
	REG_GPIO_PXPES(4)  =  0xffffffff;     \
	REG_GPIO_PXFUNC(5) =  0xffffffff;     \
	REG_GPIO_PXSELC(5) =  0xffffffff;     \
	REG_GPIO_PXDIRC(5) =  0xffffffff;     \
	REG_GPIO_PXPES(5)  =  0xffffffff;     \
} while (0)

static int jz_pm_do_hibernate(void)
{
	printk("Put CPU into hibernate mode.\n");

	/* Mask all interrupts */
	REG_INTC_IMSR = 0xffffffff;

	/* 
	 * RTC Wakeup or 1Hz interrupt can be enabled or disabled 
	 * through  RTC driver's ioctl (linux/driver/char/rtc_jz.c).
	 */

	/* Set minimum wakeup_n pin low-level assertion time for wakeup: 100ms */
	while (!(REG_RTC_RCR & RTC_RCR_WRDY));
	REG_RTC_HWFCR = (100 << RTC_HWFCR_BIT);

	/* Set reset pin low-level assertion time after wakeup: must  > 60ms */
	while (!(REG_RTC_RCR & RTC_RCR_WRDY));
	REG_RTC_HRCR = (60 << RTC_HRCR_BIT); /* 60 ms */

	/* Scratch pad register to be reserved */
	while (!(REG_RTC_RCR & RTC_RCR_WRDY));
	REG_RTC_HSPR = 0x12345678;

	/* clear wakeup status register */
	while (!(REG_RTC_RCR & RTC_RCR_WRDY));
	REG_RTC_HWRSR = 0x0;

	/* Put CPU to power down mode */
	while (!(REG_RTC_RCR & RTC_RCR_WRDY));
	REG_RTC_HCR = RTC_HCR_PD;

	while (!(REG_RTC_RCR & RTC_RCR_WRDY));
	while(1);

	/* We can't get here */
	return 0;
}

/* NOTES:
 * 1: Pins that are floated (NC) should be set as input and pull-enable.
 * 2: Pins that are pull-up or pull-down by outside should be set as input 
 *    and pull-disable.
 * 3: Pins that are connected to a chip except sdram and nand flash 
 *    should be set as input and pull-disable, too.
 */
static void jz_board_do_sleep(unsigned long *ptr)
{
	unsigned char i;
        
        /* Print messages of GPIO registers for debug */
	for(i=0;i<GPIO_PORT_NUM;i++) {
		dprintk("run dat:%x pin:%x fun:%x sel:%x dir:%x pull:%x msk:%x trg:%x\n",        \
			REG_GPIO_PXDAT(i),REG_GPIO_PXPIN(i),REG_GPIO_PXFUN(i),REG_GPIO_PXSEL(i), \
			REG_GPIO_PXDIR(i),REG_GPIO_PXPE(i),REG_GPIO_PXIM(i),REG_GPIO_PXTRG(i));
	}

        /* Save GPIO registers */
	for(i = 1; i < GPIO_PORT_NUM; i++) {
		*ptr++ = REG_GPIO_PXFUN(i);
		*ptr++ = REG_GPIO_PXSEL(i);
		*ptr++ = REG_GPIO_PXDIR(i);
		*ptr++ = REG_GPIO_PXPE(i);
		*ptr++ = REG_GPIO_PXIM(i);
		*ptr++ = REG_GPIO_PXDAT(i);
		*ptr++ = REG_GPIO_PXTRG(i);
	}

        /*
         * Set all pins to pull-disable, and set all pins as input except 
         * sdram and the pins which can be used as CS1_N to CS4_N for chip select. 
         */
        __gpio_as_sleep();

        /*
	 * Set proper status for GPC21 to GPC24 which can be used as CS1_N to CS4_N.
	 * Keep the pins' function used for chip select(CS) here according to your   
         * system to avoid chip select crashing with sdram when resuming from sleep mode.
         */

#if defined(CONFIG_JZ4750L_APUS)
        /* GPB25/CS1_N is used as chip select for nand flash, shouldn't be change. */ 

        /* GPB26/CS2_N is connected to nand flash, needn't be changed. */

        /* GPB28/CS3_N is used as cs8900's chip select, shouldn't be changed. */
 
	/* GPB27/CS4_N is used as NOR's chip select, shouldn't be changed. */ 
#endif

 	/* 
         * Enable pull for NC pins here according to your system 
	 */

#if defined(CONFIG_JZ4750L_APUS)
#endif

	/* 
         * If you must set some GPIOs as output to high level or low level,  
         * you can set them here, using:
         * __gpio_as_output(n);
         * __gpio_set_pin(n); or  __gpio_clear_pin(n);
	 */

#if defined(CONFIG_JZ4750L_APUS)
	/* GPC7 which is used as AMPEN_N should be set to high to disable audio amplifier */
	__gpio_as_output(32*2+7);
	__gpio_set_pin(32*2+7);
#endif

#ifdef DEBUG
        /* Keep uart function for printing debug message */
	__gpio_as_uart0();
	__gpio_as_uart1();
	__gpio_as_uart2();
	__gpio_as_uart3();

        /* Print messages of GPIO registers for debug */
	for(i=0;i<GPIO_PORT_NUM;i++) {
		dprintk("sleep dat:%x pin:%x fun:%x sel:%x dir:%x pull:%x msk:%x trg:%x\n",      \
			REG_GPIO_PXDAT(i),REG_GPIO_PXPIN(i),REG_GPIO_PXFUN(i),REG_GPIO_PXSEL(i), \
			REG_GPIO_PXDIR(i),REG_GPIO_PXPE(i),REG_GPIO_PXIM(i),REG_GPIO_PXTRG(i));
	}
#endif
}

static void jz_board_do_resume(unsigned long *ptr)
{
	unsigned char i;

	/* Restore GPIO registers */
	for(i = 1; i < GPIO_PORT_NUM; i++) {
		 REG_GPIO_PXFUNS(i) = *ptr;
		 REG_GPIO_PXFUNC(i) = ~(*ptr++);

		 REG_GPIO_PXSELS(i) = *ptr;
		 REG_GPIO_PXSELC(i) = ~(*ptr++);

		 REG_GPIO_PXDIRS(i) = *ptr;
		 REG_GPIO_PXDIRC(i) = ~(*ptr++);

		 REG_GPIO_PXPES(i) = *ptr;
		 REG_GPIO_PXPEC(i) = ~(*ptr++);

		 REG_GPIO_PXIMS(i)=*ptr;
		 REG_GPIO_PXIMC(i)=~(*ptr++);
	
		 REG_GPIO_PXDATS(i)=*ptr;
		 REG_GPIO_PXDATC(i)=~(*ptr++);
	
		 REG_GPIO_PXTRGS(i)=*ptr;
		 REG_GPIO_PXTRGC(i)=~(*ptr++);
	}

        /* Print messages of GPIO registers for debug */
	for(i=0;i<GPIO_PORT_NUM;i++) {
		dprintk("resume dat:%x pin:%x fun:%x sel:%x dir:%x pull:%x msk:%x trg:%x\n",     \
			REG_GPIO_PXDAT(i),REG_GPIO_PXPIN(i),REG_GPIO_PXFUN(i),REG_GPIO_PXSEL(i), \
			REG_GPIO_PXDIR(i),REG_GPIO_PXPE(i),REG_GPIO_PXIM(i),REG_GPIO_PXTRG(i));
	}
}



static int jz_pm_do_sleep(void)
{ 
	unsigned long delta;
	unsigned long nfcsr = REG_EMC_NFCSR;
	unsigned long opcr = REG_CPM_OPCR;
	unsigned long imr = REG_INTC_IMR;
	unsigned long sadc = REG_SADC_ENA;
	unsigned long sleep_gpio_save[7*(GPIO_PORT_NUM-1)];

	printk("Put CPU into sleep mode.\n");

	/* Preserve current time */
	delta = xtime.tv_sec - REG_RTC_RSR;

        /* Disable nand flash */
	REG_EMC_NFCSR = ~0xff;

        /* stop sadc */
	REG_SADC_ENA &= ~0x7;
	while((REG_SADC_ENA & 0x7) != 0);
 	udelay(100);

        /*stop udc and usb*/
	__cpm_suspend_uhcphy();
	__cpm_suspend_udcphy();

	/* Sleep on-board modules */
	jz_board_do_sleep(sleep_gpio_save);

	/* Mask all interrupts */
	REG_INTC_IMSR = 0xffffffff;

	/* Just allow following interrupts to wakeup the system.
	 * Note: modify this according to your system.
	 */

	/* enable RTC alarm */
	__intc_unmask_irq(IRQ_RTC);
#if 0
        /* make system wake up after n seconds by RTC alarm */
	unsigned int v, n;
	n = 10;
	while (!__rtc_write_ready());
	__rtc_enable_alarm();
	while (!__rtc_write_ready());
	__rtc_enable_alarm_irq();
 	while (!__rtc_write_ready());
 	v = __rtc_get_second();
 	while (!__rtc_write_ready());
 	__rtc_set_alarm_second(v+n);
#endif

	/* WAKEUP key */
	__gpio_as_irq_rise_edge(GPIO_WAKEUP);
	__gpio_unmask_irq(GPIO_WAKEUP);
	__intc_unmask_irq(IRQ_GPIO0 - (GPIO_WAKEUP/32));  /* unmask IRQ_GPIOn depends on GPIO_WAKEUP */

	/* disable externel clock Oscillator in sleep mode */
	__cpm_disable_osc_in_sleep();
	/* select 32K crystal as RTC clock in sleep mode */
	__cpm_select_rtcclk_rtc();

 	/* Enter SLEEP mode */
	REG_CPM_LCR &= ~CPM_LCR_LPM_MASK;
	REG_CPM_LCR |= CPM_LCR_LPM_SLEEP;
	__asm__(".set\tmips3\n\t"
		"wait\n\t"
		".set\tmips0");

	/* Restore to IDLE mode */
	REG_CPM_LCR &= ~CPM_LCR_LPM_MASK;
	REG_CPM_LCR |= CPM_LCR_LPM_IDLE;

        /* Restore nand flash control register */
	REG_EMC_NFCSR = nfcsr;

	/* Restore interrupts */
	REG_INTC_IMSR = imr;
	REG_INTC_IMCR = ~imr;
	
	/* Restore sadc */
	REG_SADC_ENA = sadc;
	
	/* Resume on-board modules */
	jz_board_do_resume(sleep_gpio_save);

	/* Restore Oscillator and Power Control Register */
	REG_CPM_OPCR = opcr;

	/* Restore current time */
	xtime.tv_sec = REG_RTC_RSR + delta;

	return 0;
}

/* Put CPU to HIBERNATE mode */
int jz_pm_hibernate(void)
{
	return jz_pm_do_hibernate();
}

#ifndef CONFIG_JZ_POWEROFF
static irqreturn_t pm_irq_handler (int irq, void *dev_id)
{
	return IRQ_HANDLED;
}
#endif

/* Put CPU to SLEEP mode */
int jz_pm_sleep(void)
{
	int retval;

#ifndef CONFIG_JZ_POWEROFF
	if ((retval = request_irq (IRQ_GPIO_0 + GPIO_WAKEUP, pm_irq_handler, IRQF_DISABLED,
				   "PM", NULL))) {
		printk ("PM could not get IRQ for GPIO_WAKEUP\n");
		return retval;
	}
#endif

	pm_send_all(PM_SUSPEND, (void *)3);
	retval = jz_pm_do_sleep();
	pm_send_all(PM_RESUME, (void *)0);

#ifndef CONFIG_JZ_POWEROFF
	free_irq (IRQ_GPIO_0 + GPIO_WAKEUP, NULL);
#endif

	return retval;
}

#if 0
/* Deprecated ,was used by dpm */
void jz_pm_idle(void)
{
	local_irq_disable();
	if (!need_resched()) {
		local_irq_enable();
		cpu_wait();
	}
}
#endif


#ifdef CONFIG_SYSCTL

/*
 * Use a temporary sysctl number. Horrid, but will be cleaned up in 2.6
 * when all the PM interfaces exist nicely.
 */
#define CTL_PM_SUSPEND   1
#define CTL_PM_HIBERNATE 2

/*----------------------------------------------------------------------------
 * Power Management sleep sysctl proc interface
 *
 * A write to /proc/sys/pm/suspend invokes this function 
 * which initiates a sleep.
 *--------------------------------------------------------------------------*/
static int sysctl_jz_pm_sleep(void)
{
	return jz_pm_sleep();
}

/*----------------------------------------------------------------------------
 * Power Management sleep sysctl proc interface
 *
 * A write to /proc/sys/pm/hibernate invokes this function 
 * which initiates a poweroff.
 *--------------------------------------------------------------------------*/
static int sysctl_jz_pm_hibernate(void)
{
	return jz_pm_hibernate();
}

static struct ctl_table pm_table[] =
{
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "suspend",
		.data		= NULL,
		.maxlen		= 0,
		.mode		= 0600,
		.proc_handler	= &sysctl_jz_pm_sleep,
	},
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "hibernate",
		.data		= NULL,
		.maxlen		= 0,
		.mode		= 0600,
		.proc_handler	= &sysctl_jz_pm_hibernate,
	},
	{ .ctl_name = 0}
};

static struct ctl_table pm_dir_table[] =
{
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "pm",
		.mode		= 0555,
		.child		= pm_table,
	},
	{ .ctl_name = 0}
};

#endif /* CONFIG_SYSCTL */

/*
 * Initialize power interface
 */
static int __init jz_pm_init(void)
{
	printk("Power Management for JZ\n");

#ifdef CONFIG_SYSCTL
	register_sysctl_table(pm_dir_table);
#endif

	return 0;
}

module_init(jz_pm_init);
