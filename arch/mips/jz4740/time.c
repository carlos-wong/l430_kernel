/*
 * linux/arch/mips/jz4740/time.c
 * 
 * Setting up the clock on the JZ4740 boards.
 * 
 * Copyright (C) 2008 Ingenic Semiconductor Inc.
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
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/clockchips.h>

#include <asm/time.h>
#include <asm/jzsoc.h>

/* This is for machines which generate the exact clock. */

#define JZ_TIMER_CHAN  0
#define JZ_TIMER_IRQ  IRQ_TCU0

#define JZ_TIMER_CLOCK (JZ_EXTAL>>4) /* Jz timer clock frequency */

static struct clocksource clocksource_jz; /* Jz clock source */
static struct clock_event_device jz_clockevent_device; /* Jz clock event */

void (*jz_timer_callback)(void);

static irqreturn_t jz_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *cd = dev_id;

	REG_TCU_TFCR = 1 << JZ_TIMER_CHAN; /* ACK timer */

	if (jz_timer_callback)
		jz_timer_callback();

	cd->event_handler(cd);

	return IRQ_HANDLED;
}

static struct irqaction jz_irqaction = {
	.handler	= jz_timer_interrupt,
	.flags		= IRQF_DISABLED | IRQF_PERCPU | IRQF_TIMER,
	.name		= "jz-timerirq",
};


cycle_t jz_get_cycles(void)
{
	/* convert jiffes to jz timer cycles */
	return (cycle_t)( jiffies*((JZ_TIMER_CLOCK)/HZ) + REG_TCU_TCNT(JZ_TIMER_CHAN));
}

static struct clocksource clocksource_jz = {
	.name 		= "jz_clocksource",
	.rating		= 300,
	.read		= jz_get_cycles,
	.mask		= 0xFFFF,
	.shift 		= 10,
	.flags		= CLOCK_SOURCE_WATCHDOG,
};

static int __init jz_clocksource_init(void)
{
	clocksource_jz.mult = clocksource_hz2mult(JZ_TIMER_CLOCK, clocksource_jz.shift);
	clocksource_register(&clocksource_jz);
	return 0;
}

static int jz_set_next_event(unsigned long evt,
				  struct clock_event_device *unused)
{
	return 0;
}

static void jz_set_mode(enum clock_event_mode mode,
			struct clock_event_device *evt)
{
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
                break;
        case CLOCK_EVT_MODE_ONESHOT:
        case CLOCK_EVT_MODE_UNUSED:
        case CLOCK_EVT_MODE_SHUTDOWN:
                break;
        case CLOCK_EVT_MODE_RESUME:
                break;
        }
}

static struct clock_event_device jz_clockevent_device = {
	.name		= "jz-clockenvent",
	.features	= CLOCK_EVT_FEAT_PERIODIC,
//	.features	= CLOCK_EVT_FEAT_ONESHOT, /* Jz4740 not support dynamic clock now */

	/* .mult, .shift, .max_delta_ns and .min_delta_ns left uninitialized */
	.rating		= 300,
	.irq		= JZ_TIMER_IRQ,
	.set_mode	= jz_set_mode,
	.set_next_event	= jz_set_next_event,
};

static void __init jz_clockevent_init(void)
{
	struct clock_event_device *cd = &jz_clockevent_device;
	unsigned int cpu = smp_processor_id();

	cd->cpumask = cpumask_of_cpu(cpu);
	clockevents_register_device(cd);
}

static void __init jz_timer_setup(void)
{
	jz_clocksource_init();	/* init jz clock source */
	jz_clockevent_init();	/* init jz clock event */

	/*
	 * Make irqs happen for the system timer
	 */
	jz_irqaction.dev_id = &jz_clockevent_device;
	setup_irq(JZ_TIMER_IRQ, &jz_irqaction);
}


void __init plat_time_init(void)
{
	unsigned int latch;
	/* Init timer */
	latch = ( JZ_TIMER_CLOCK + (HZ>>1)) / HZ;

	REG_TCU_TCSR(JZ_TIMER_CHAN) = TCU_TCSR_PRESCALE16 | TCU_TCSR_EXT_EN;
	REG_TCU_TCNT(JZ_TIMER_CHAN) = 0;
	REG_TCU_TDHR(JZ_TIMER_CHAN) = 0;
	REG_TCU_TDFR(JZ_TIMER_CHAN) = latch;

	REG_TCU_TMSR = (1 << (JZ_TIMER_CHAN + 16)); /* mask half irq */
	REG_TCU_TMCR = (1 << JZ_TIMER_CHAN); /* unmask full irq */
	REG_TCU_TSCR = (1 << JZ_TIMER_CHAN); /* enable timer clock */
	REG_TCU_TESR = (1 << JZ_TIMER_CHAN); /* start counting up */

	jz_timer_setup();
}
