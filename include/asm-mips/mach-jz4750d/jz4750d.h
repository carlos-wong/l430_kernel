/*
 *  linux/include/asm-mips/mach-jz4750d/jz4750d.h
 *
 *  JZ4750 common definition.
 *
 *  Copyright (C) 2008 Ingenic Semiconductor Inc.
 *
 *  Author: <cwjia@ingenic.cn>
 *  Mod: <maddrone@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4750_H__
#define __ASM_JZ4750_H__

#include <asm/mach-jz4750d/regs.h>
#include <asm/mach-jz4750d/ops.h>
#include <asm/mach-jz4750d/dma.h>
#include <asm/mach-jz4750d/misc.h>
#include <asm/mach-jz4750d/platform.h>
 
/*------------------------------------------------------------------
 * Platform definitions
 */
#ifdef CONFIG_JZ4750D_FUWA1
#include <asm/mach-jz4750d/board-fuwa1.h>
#endif

#ifdef CONFIG_JZ4750D_CETUS
#include <asm/mach-jz4750d/board-cetus.h>
#endif
/* Add other platform definition here ... */
#ifdef CONFIG_JZ4750D_L009
#include <asm/mach-jz4750d/board-l009.h>
#endif


/*------------------------------------------------------------------
 * Follows are related to platform definitions
 */

#include <asm/mach-jz4750d/clock.h>
#include <asm/mach-jz4750d/serial.h>

#endif /* __ASM_JZ4750_H__ */
