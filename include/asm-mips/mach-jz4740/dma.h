/*
 *  linux/include/asm-mips/mach-jz4740/dma.h
 *
 *  JZ4740 DMA definition.
 *
 *  Copyright (C) 2006 - 2007 Ingenic Semiconductor Inc.
 *
 *  Author: <lhhuang@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_JZ4740_DMA_H__
#define __ASM_JZ4740_DMA_H__

#include <linux/interrupt.h>
#include <asm/io.h>			/* need byte IO */
#include <linux/spinlock.h>		/* And spinlocks */
#include <linux/delay.h>
#include <asm/system.h>

/*
 * Descriptor structure for JZ4740 DMA engine
 * Note: this structure must always be aligned to a 16-bytes boundary.
 */

typedef struct {
	volatile u32 dcmd;	/* DCMD value for the current transfer */
	volatile u32 dsadr;	/* DSAR value for the current transfer */
	volatile u32 dtadr;	/* DTAR value for the current transfer */
	volatile u32 ddadr;	/* Points to the next descriptor + transfer count */
} jz_dma_desc;


/* DMA Device ID's follow */
enum {
	DMA_ID_UART0_TX = 0,
	DMA_ID_UART0_RX,
	DMA_ID_SSI_TX,
	DMA_ID_SSI_RX,
	DMA_ID_AIC_TX,
	DMA_ID_AIC_RX,
	DMA_ID_MSC_TX,
	DMA_ID_MSC_RX,
	DMA_ID_TCU_OVERFLOW,
	DMA_ID_AUTO,
	DMA_ID_RAW_SET,
	DMA_ID_MAX
};

/* DMA modes, simulated by sw */
#define DMA_MODE_READ	0x0  /* I/O to memory, no autoinit, increment, single mode */
#define DMA_MODE_WRITE	0x1  /* memory to I/O, no autoinit, increment, single mode */
#define DMA_AUTOINIT	0x2
#define DMA_MODE_MASK	0x3

struct jz_dma_chan {
	int dev_id;	/* DMA ID: this channel is allocated if >=0, free otherwise */ 
	unsigned int io;        /* DMA channel number */
	const char *dev_str;    /* string describes the DMA channel */
	int irq;                /* DMA irq number */
	void *irq_dev;          /* DMA private device structure */
	unsigned int fifo_addr; /* physical fifo address of the requested device */
	unsigned int cntl;	/* DMA controll */
	unsigned int mode;      /* DMA configuration */
	unsigned int source;    /* DMA request source */
};

extern struct jz_dma_chan jz_dma_table[];


#define DMA_8BIT_RX_CMD					\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_8BIT | DMAC_DCMD_RDIL_IGN

#define DMA_8BIT_TX_CMD					\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_8 |		\
	DMAC_DCMD_DS_8BIT | DMAC_DCMD_RDIL_IGN

#define DMA_16BIT_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_16BIT_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_32BIT_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BIT | DMAC_DCMD_RDIL_IGN

#define DMA_32BIT_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BIT | DMAC_DCMD_RDIL_IGN

#define DMA_16BYTE_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_16BYTE_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_8 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_32BYTE_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_32BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_32BYTE_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_8 |		\
	DMAC_DCMD_DS_32BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_32_16BYTE_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_32_16BYTE_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BIT_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BIT_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BIT | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BYTE_RX_CMD				\
	DMAC_DCMD_DAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

#define DMA_AIC_16BYTE_TX_CMD				\
	DMAC_DCMD_SAI |					\
	DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |		\
	DMAC_DCMD_DS_16BYTE | DMAC_DCMD_RDIL_IGN

extern int jz_request_dma(int dev_id,
			  const char *dev_str,
			  irqreturn_t (*irqhandler)(int, void *),
			  unsigned long irqflags,
			  void *irq_dev_id);
extern void jz_free_dma(unsigned int dmanr);

extern int jz_dma_read_proc(char *buf, char **start, off_t fpos,
			      int length, int *eof, void *data);
extern void dump_jz_dma_channel(unsigned int dmanr);

extern void enable_dma(unsigned int dmanr);
extern void disable_dma(unsigned int dmanr);
extern void set_dma_addr(unsigned int dmanr, unsigned int phyaddr);
extern void set_dma_count(unsigned int dmanr, unsigned int bytecnt);
extern void set_dma_mode(unsigned int dmanr, unsigned int mode);
extern void jz_set_oss_dma(unsigned int dmanr, unsigned int mode, unsigned int audio_fmt);
extern void jz_set_alsa_dma(unsigned int dmanr, unsigned int mode, unsigned int audio_fmt);
extern unsigned int get_dma_residue(unsigned int dmanr);

extern spinlock_t  dma_spin_lock;

static __inline__ unsigned long claim_dma_lock(void)
{
	unsigned long flags;
	spin_lock_irqsave(&dma_spin_lock, flags);
	return flags;
}

static __inline__ void release_dma_lock(unsigned long flags)
{
	spin_unlock_irqrestore(&dma_spin_lock, flags);
}

/* Clear the 'DMA Pointer Flip Flop'.
 * Write 0 for LSB/MSB, 1 for MSB/LSB access.
 */
#define clear_dma_ff(channel)

static __inline__ struct jz_dma_chan *get_dma_chan(unsigned int dmanr)
{
	if (dmanr > MAX_DMA_NUM
	    || jz_dma_table[dmanr].dev_id < 0)
		return NULL;
	return &jz_dma_table[dmanr];
}

static __inline__ int dma_halted(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);
	if (!chan)
		return 1;
	return  __dmac_channel_transmit_halt_detected(dmanr) ? 1 : 0;
}

static __inline__ unsigned int get_dma_mode(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);
	if (!chan)
		return 0;
	return chan->mode;
}

static __inline__ void clear_dma_done(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);
	if (!chan)
		return;
	REG_DMAC_DCCSR(chan->io) &= ~(DMAC_DCCSR_HLT | DMAC_DCCSR_TT | DMAC_DCCSR_AR);
}

static __inline__ void clear_dma_halt(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);
	if (!chan)
		return;
	REG_DMAC_DCCSR(chan->io) &= ~(DMAC_DCCSR_HLT);
	REG_DMAC_DMACR &= ~(DMAC_DMACR_HLT);
}

static __inline__ void clear_dma_flag(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);
	if (!chan)
		return;
	REG_DMAC_DCCSR(chan->io) &= ~(DMAC_DCCSR_HLT | DMAC_DCCSR_TT | DMAC_DCCSR_AR);
	REG_DMAC_DMACR &= ~(DMAC_DMACR_HLT | DMAC_DMACR_AR);
}

static __inline__ void set_dma_page(unsigned int dmanr, char pagenr)
{
}

static __inline__ unsigned int get_dma_done_status(unsigned int dmanr)
{
	unsigned long dccsr;
	struct jz_dma_chan *chan = get_dma_chan(dmanr);
	if (!chan)
		return 0;
	dccsr = REG_DMAC_DCCSR(chan->io);
	return dccsr & (DMAC_DCCSR_HLT | DMAC_DCCSR_TT | DMAC_DCCSR_AR);
}

static __inline__ int get_dma_done_irq(unsigned int dmanr)
{
	struct jz_dma_chan *chan = get_dma_chan(dmanr);
	if (!chan)
		return -1;
	return chan->irq;
}

#endif  /* __ASM_JZ4740_DMA_H__ */
