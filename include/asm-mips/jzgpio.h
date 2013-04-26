/*
 * Copyright (C) 2009 Ignacio Garcia Perez <iggarpe@gmail.com>
 *
 * Author: <iggarpe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#ifndef __ASM_MIPS_JZGPIO_H__
#define __ASM_MIPS_JZGPIO_H__

#include <asm/jzsoc.h>

static inline int gpio_request(unsigned gpio, const char *label)
{
       return 0;
}

static inline void gpio_free(unsigned gpio)
{
}

static inline int gpio_to_irq(unsigned gpio)
{
	return IRQ_GPIO_0 + gpio;
}

static inline int gpio_get_value(unsigned gpio)
{
	return __gpio_get_pin(gpio);
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	if (value) __gpio_set_pin(gpio);
	else __gpio_clear_pin(gpio);
}

static inline int gpio_direction_input(unsigned gpio)
{
	__gpio_as_input(gpio);
	return 0;
}

static inline int gpio_direction_output(unsigned gpio, int value)
{
	__gpio_as_output(gpio);
	gpio_set_value(gpio, value);
	return 0;
}

/* cansleep wrappers */
#include <asm-generic/gpio.h>

#endif /* __ASM_JZ4740_GPIO_H */
