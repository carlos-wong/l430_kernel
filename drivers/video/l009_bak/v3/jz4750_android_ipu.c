/*
 * linux/drivers/video/jz4750_android_ipu.c 
 *
 * Copyright (C) 2005-2010, Ingenic Semiconductor Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.

 *  Author:   <chlfeng@ingenic.cn>
 *
 *  Create:   2009-10-21, by Emily
 *  
 *  http://www.ingenic.cn
 */

#include <linux/module.h>
#include <linux/kernel.h>

#include <asm/irq.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <asm/irq.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <asm/jzsoc.h>

#include "jz4750_lcd.h"
#include "jz4750_ipu.h"



#if 0
#if defined CONFIG_SOC_JZ4750
#include <asm/mach-jz4750/jz4750.h>
#include <asm/mach-jz4750/regs.h>
#include <asm/mach-jz4750/ops.h>
#elif defined CONFIG_SOC_JZ4760
#include <asm/mach-jz4760/jz4760.h>
#endif
#endif


#define IPU_DBG
//#undef  IPU_DBG


#ifdef IPU_DBG
#define ENTER()								\
	do {								\
		printk("%03d ENTER %s\n", __LINE__, __FUNCTION__);	\
	} while (0)
#define LEAVE()								\
	do {								\
		printk("%03d LEAVE %s\n", __LINE__, __FUNCTION__);	\
	} while (0)
#define MY_DBG(sss, aaa...)						\
	do {								\
		printk("%03d %s, " sss "\n", __LINE__, __FUNCTION__, ##aaa); \
		udelay(100000);						\
	} while (0)
#else
#define ENTER()					\
	do {					\
	} while (0)
#define LEAVE()					\
	do {					\
	} while (0)
#define MY_DBG(sss, aaa...)			\
	do {					\
	} while (0)

#endif	// 


#if 1				//emily
#define IPU_OPEN							(1 << 0)
#define IPU_INIT							(1 << 1)
#define IPU_SET_STATE_BIT					(25)	// 7 bit
#define IPU_SET_STATE_MASK					(0x7f << IPU_SET_STATE_BIT)
#define SET_BIT(x)							(1 << (IPU_SET_STATE_BIT+x))
#define IPU_CHANGE_BUF						(SET_BIT(0))
#define IPU_SET_CTRL						(SET_BIT(1))
#define IPU_SET_DAT_FMT						(SET_BIT(2))
#define IPU_SET_CSC							(SET_BIT(3))
#define IPU_RATIO_MUL						(100000)
//#define IPU_FUNC_EX

#define IPU_INTC_DISABLE()					OUTREG32(A_INTC_ICMSR, 1 << 30);
#define IPU_INTC_ENABLE()					OUTREG32(A_INTC_ICMCR, 1 << 30);
#endif				//Emily

#ifdef PHYS
#undef PHYS
#endif

#define PHYS(x) (x)

struct ipu_driver_priv g_ipu_native_data = 
{
	.img = {
		.version = sizeof(struct ipu_img_param_t),
	},
	.frame_requested = 0,
	.frame_done = 0,
};
struct ipu_driver_priv *ipu_priv = &g_ipu_native_data;

extern struct jz4750lcd_info jz4750_lcd_panel;
extern unsigned char *lcd_frame;

/*----------------------------------------------------------------------------------*/
char *ipu_regs_name[] = {
	"REG_CTRL",		/* 0x0 */
	"REG_STATUS",		/* 0x4 */
	"REG_D_FMT",		/* 0x8 */
	"REG_Y_ADDR",		/* 0xc */
	"REG_U_ADDR",		/* 0x10 */
	"REG_V_ADDR",		/* 0x14 */
	"REG_IN_FM_GS",		/* 0x18 */
	"REG_Y_STRIDE",		/* 0x1c */
	"REG_UV_STRIDE",	/* 0x20 */
	"REG_OUT_ADDR",		/* 0x24 */
	"REG_OUT_GS",		/* 0x28 */
	"REG_OUT_STRIDE",	/* 0x2c */
	"RSZ_COEF_INDEX",	/* 0x30 */
	"REG_CSC_C0_COEF",	/* 0x34 */
	"REG_CSC_C1_COEF",	/* 0x38 */
	"REG_CSC_C2_COEF",	/* 0x3c */
	"REG_CSC_C3_COEF",	/* 0x40 */
};

int jz47_dump_ipu_regs(struct ipu_driver_priv *ipu, int num)
{
	int i, total;

	rsz_lut *h_lut;
	rsz_lut *v_lut;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}

	h_lut = ipu->h_lut;
	v_lut = ipu->v_lut;

	if (num >= 0) {
		//printk ("ipu_reg: %s: 0x%x\n", ipu_regs_name[num >> 2], REG32(IPU_V_BASE + num));
		return (1);
	}

	if (num == -1) {
		total = sizeof(ipu_regs_name) / sizeof(char *);
		for (i = 0; i < total; i++) {
			printk("ipu_reg: %s: \t0x%08x\r\n", ipu_regs_name[i],
			       REG32((unsigned int *) IPU_V_BASE + i));
		}
	}
	if (num == -2) {
		for (i = 0; i < IPU_LUT_LEN; i++) {
			printk("ipu_H_LUT(%02d): in:%d, out:%d, coef: 0x%08x\n",
			       i, h_lut[i].in_n, h_lut[i].out_n, h_lut[i].coef);
		}

		for (i = 0; i < IPU_LUT_LEN; i++) {
			printk("ipu_V_LUT(%02d): in:%d, out:%d, coef: 0x%08x\n",
			       i, v_lut[i].in_n, v_lut[i].out_n, v_lut[i].coef);
		}
	}

	return 1;
}


void print_img(struct ipu_driver_priv *ipu)
{
	struct ipu_img_param_t *img;
	ENTER();
	if (ipu == NULL) {
		return;
	}

	img = &ipu->img;

	printk("ipu_cmd[%#x]\r\n", (unsigned int) img->ipu_cmd);
	printk("output_mode[%#x]\r\n", (unsigned int) img->output_mode);
	printk("ipu_ctrl[%#x]\r\n", (unsigned int) img->ipu_ctrl);
	printk("ipu_d_fmt[%#x]\r\n", (unsigned int) img->ipu_d_fmt);
	printk("in_width[%#x]\r\n", (unsigned int) img->in_width);
	printk("in_height[%#x]\r\n", (unsigned int) img->in_height);
	printk("in_bpp[%#x]\r\n", (unsigned int) img->in_bpp);
	printk("out_width[%#x]\r\n", (unsigned int) img->out_width);
	printk("out_height[%#x]\r\n", (unsigned int) img->out_height);
	printk("y_buf_v[%#x]\r\n", (unsigned int) img->y_buf_v);
	printk("u_buf_v[%#x]\r\n", (unsigned int) img->u_buf_v);
	printk("v_buf_v[%#x]\r\n", (unsigned int) img->v_buf_v);
	printk("y_buf_p[%#x]\r\n", (unsigned int) img->y_buf_p);
	printk("u_buf_p[%#x]\r\n", (unsigned int) img->u_buf_p);
	printk("v_buf_p[%#x]\r\n", (unsigned int) img->v_buf_p);
	printk("out_buf_v[%#x]\r\n", (unsigned int) img->out_buf_v);
	printk("out_buf_p[%#x]\r\n", (unsigned int) img->out_buf_p);
	printk("y_t_addr[%#x]\r\n", (unsigned int) img->y_t_addr);
	printk("u_t_addr[%#x]\r\n", (unsigned int) img->u_t_addr);
	printk("v_t_addr[%#x]\r\n", (unsigned int) img->v_t_addr);
	printk("out_t_addr[%#x]\r\n", (unsigned int) img->out_t_addr);
	printk("stride.y[%#x]\r\n", (unsigned int) img->stride.y);
	printk("stride.u[%#x]\r\n", (unsigned int) img->stride.u);
	printk("stride.v[%#x]\r\n", (unsigned int) img->stride.v);
	printk("stride.out[%#x]\r\n", (unsigned int) img->stride.out);

	return ;
}



static inline int jz47_ipu_wait_frame_end_flag(void)
{
	while ((REG32(IPU_V_BASE + REG_STATUS) & OUT_END) == 0) ;	// wait the end flag
	return 0;
}


// set ipu data format.
static int jz47_set_ipu_csc_cfg(struct ipu_driver_priv *ipu, int outW, int outH,
				int Wdiff, int Hdiff)
{
	struct ipu_img_param_t *img;
	unsigned int in_fmt;
	unsigned int out_fmt;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}

	img = &ipu->img;

	in_fmt = img->ipu_d_fmt & IN_FMT_MASK;
	out_fmt = img->ipu_d_fmt & OUT_FMT_MASK;

	switch (in_fmt) {
	case IN_FMT_YUV420:
		Hdiff = (Hdiff + 1) & ~1;
		Wdiff = (Wdiff + 1) & ~1;
		break;
	case IN_FMT_YUV422:
		Wdiff = (Wdiff + 1) & ~1;
		break;
	case IN_FMT_YUV444:
	case IN_FMT_YUV411:
		break;
	default:
		printk("Error: Input data format isn't support\n");
		return (-1);
	}

	switch (out_fmt) {
	case OUT_FMT_RGB888:
		outW = outW << 2;
		break;
	case OUT_FMT_RGB555:
		outW = outW << 1;
		break;
	case OUT_FMT_RGB565:
		outW = outW << 1;
		break;
	}

	// Set GS register
	REG32(IPU_V_BASE + REG_IN_FM_GS) =
	    IN_FM_W(img->in_width) | IN_FM_H((img->in_height - Hdiff) & ~0x1);
	REG32(IPU_V_BASE + REG_OUT_GS) = OUT_FM_W(outW) | OUT_FM_H(outH);

//	printk("REG32(IPU_V_BASE + REG_IN_FM_GS) =0x%08X\n", REG32(IPU_V_BASE + REG_IN_FM_GS) );
//	printk("REG32(IPU_V_BASE + REG_OUT_GS)=0x%08X\n", REG32(IPU_V_BASE + REG_OUT_GS));

	// Set out stirde
	if (img->stride.out != 0) {
		REG32(IPU_V_BASE + REG_OUT_STRIDE) = img->stride.out;
	} else {
		switch ( img->output_mode & IPU_OUTPUT_MODE_MASK) {
		case IPU_OUTPUT_TO_LCD_FG1:
			break;
		case IPU_OUTPUT_TO_LCD_FB0:
		case IPU_OUTPUT_TO_LCD_FB1:
			REG32(IPU_V_BASE + REG_OUT_STRIDE) = ipu->fb_w * img->in_bpp >> 3;
			break;
		case IPU_OUTPUT_TO_FRAMEBUFFER:
		default:
			outW = img->out_width;
			switch (out_fmt) {
			default:
			case OUT_FMT_RGB888:
				outW = outW << 2;
				break;
			case OUT_FMT_RGB555:
			case OUT_FMT_RGB565:
				outW = outW << 1;
				break;
			}
			REG32(IPU_V_BASE + REG_OUT_STRIDE) = outW;
			break;
		}

	}

	if ( in_fmt == IN_FMT_YUV422 ) {
		printk("*** jz4750 ipu driver: IN_FMT_YUV422 use IN_OFT_Y1VY0U\n");
		in_fmt |= IN_OFT_Y1VY0U;
	}

	// set Format
	REG32(IPU_V_BASE + REG_D_FMT) = in_fmt | out_fmt;
	// set CSC parameter
	if ((in_fmt != IN_FMT_YUV444) && (out_fmt != OUT_FMT_YUV422)) {
		img->ipu_ctrl |= CSC_EN;
		REG32(IPU_V_BASE + REG_CSC_C0_COEF) = YUV_CSC_C0;
		REG32(IPU_V_BASE + REG_CSC_C1_COEF) = YUV_CSC_C1;
		REG32(IPU_V_BASE + REG_CSC_C2_COEF) = YUV_CSC_C2;
		REG32(IPU_V_BASE + REG_CSC_C3_COEF) = YUV_CSC_C3;
		REG32(IPU_V_BASE + REG_CSC_C4_COEF) = YUV_CSC_C4;
		REG32(IPU_V_BASE + REG_CSC_OFFPARA) = YUV_CSC_OFFPARA;
	}
	return 0;
}


static int init_ipu_ratio_table(struct ipu_driver_priv *ipu)
{
	unsigned int i, j, cnt;
	int diff;
	struct Ration2m *ipu_ratio_table;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}

	ipu_ratio_table = ipu->ipu_ratio_table;

	if (ipu_ratio_table == 0) {
		printk("no find ratio table!\n");
		return (-1);
	}
	//      get_ipu_ratio_table( (void **)&ipu_ratio_table, (IPU_LUT_LEN) * (IPU_LUT_LEN)*sizeof(struct Ration2m) );
	// orig table, first calculate
	for (i = 1; i <= (IPU_LUT_LEN); i++) {
		for (j = 1; j <= (IPU_LUT_LEN); j++) {
			ipu_ratio_table[(i - 1) * IPU_LUT_LEN + j - 1].ratio =
			    i * IPU_RATIO_MUL / j;
			ipu_ratio_table[(i - 1) * IPU_LUT_LEN + j - 1].n = i;
			ipu_ratio_table[(i - 1) * IPU_LUT_LEN + j - 1].m = j;
		}
	}

	// Eliminate the ratio greater than 1:2
	for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++) {
		if (ipu_ratio_table[i].ratio < IPU_RATIO_MUL / 2) {
			ipu_ratio_table[i].n = ipu_ratio_table[i].m = -1;
		}
	}
	// eliminate the same ratio
	for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++) {
		for (j = i + 1; j < (IPU_LUT_LEN) * (IPU_LUT_LEN); j++) {
			diff =
			    ipu_ratio_table[i].ratio - ipu_ratio_table[j].ratio;
			if (diff > -100 && diff < 100) {
				ipu_ratio_table[j].n = -1;
				ipu_ratio_table[j].m = -1;
			}
		}
	}

	// reorder ipu_ratio_table
	cnt = 0;
	for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++) {
		if (ipu_ratio_table[i].n != -1) {
			if (cnt != i) {
				ipu_ratio_table[cnt] = ipu_ratio_table[i];
			}
			cnt++;
		}
	}
	ipu->rtable_len = cnt;
	MY_DBG("ipu->rtable_len = %d\n", ipu->rtable_len);
	return (0);
}

static int find_ipu_ratio_factor(struct ipu_driver_priv *ipu, unsigned int ratio, unsigned int up)
{
	unsigned int i, sel;
	unsigned int diff, min = IPU_RATIO_MUL;
	struct Ration2m *ipu_ratio_table;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}

	ipu_ratio_table = ipu->ipu_ratio_table;


	sel = ipu->rtable_len;
#if 1
	for (i = 0; i < ipu->rtable_len; i++) {
		if ((up == 0) && ((ipu_ratio_table[i].n & 1) != 0)) {
			continue;
		}

		if (ratio > ipu_ratio_table[i].ratio) {
			diff = ratio - ipu_ratio_table[i].ratio;
		} else {
			diff = ipu_ratio_table[i].ratio - ratio;
		}

		if (diff < min || i == 0) {
			min = diff;
			sel = i;
		}
	}
#endif
	return (sel);
}

#ifndef IPU_FUNC_EX
static int resize_lut_cal(int srcN, int dstM, int upScale, rsz_lut lut[]);
static int resize_out_cal(int insize, int outsize, int srcN, int dstM,
			  int upScale, int *diff);
#else
static int (*resize_lut_cal) (int srcN, int dstM, int upScale, rsz_lut lut[]);
static int (*resize_out_cal) (int insize, int outsize, int srcN, int dstM,
			      int upScale, int *diff);

void set_ipu_cal_func(void *lut_cal, void *out_cal)
{
	resize_lut_cal = lut_cal;
	resize_out_cal = out_cal;
}
#endif

static int jz47_set_ipu_resize(struct ipu_driver_priv *ipu, int *outWp, int *outHp, int *Wdiff, int *Hdiff)
{
	unsigned int rsize_w = 0, rsize_h = 0;
	int W = 0, H = 0, Hsel = 0, Wsel = 0;
	int srcN, dstM, width_up, height_up;
	int Height_lut_max, Width_lut_max;
	int i;
	struct ipu_img_param_t *img;
	struct Ration2m *ipu_ratio_table;
	rsz_lut *h_lut;
	rsz_lut *v_lut;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}

	img = &ipu->img;
	ipu_ratio_table = ipu->ipu_ratio_table;

	h_lut = ipu->h_lut;
	v_lut = ipu->v_lut;


#if 1

	if ( img->output_mode & IPU_OUTPUT_TO_LCD_FG1) {
		rsize_w =
			((img->out_width + img->out_x) >
			 ipu->fb_w) ? (ipu->fb_w - img->out_x) : img->out_width;
		rsize_h =
			((img->out_height + img->out_y) >
			 ipu->fb_h) ? (ipu->fb_h - img->out_y) : img->out_height;
	}
	else {
		rsize_w = img->out_width;
		rsize_h = img->out_height;
	}

	rsize_w = (rsize_w > 2 * img->in_width) ? (2 * img->in_width) : rsize_w;
	rsize_h =
	    (rsize_h > 2 * img->in_height) ? (2 * img->in_height) : rsize_h;
	*Wdiff = *Hdiff = 0;

	MY_DBG("rsize_w=%d, rsize_h=%d", rsize_w, rsize_h);
	img->ipu_ctrl &= ~(VRSZ_EN | HRSZ_EN | H_UP_SCALE | V_UP_SCALE);
	// Don't need do resize ?
	if ((img->in_width == rsize_w) && (img->in_height == rsize_h)) {
		MY_DBG();
		img->out_height = *outHp = rsize_h;
		img->out_width = *outWp = rsize_w;
		return (0);
	} else {
		MY_DBG();
		if (img->in_width != rsize_w) {
			img->ipu_ctrl |= HRSZ_EN;
		}

		if (img->in_height != rsize_h) {
			img->ipu_ctrl |= VRSZ_EN;
		}
	}

	// init the resize factor table
	init_ipu_ratio_table(ipu);

	width_up = (rsize_w >= img->in_width);
	height_up = (rsize_h >= img->in_height);
	MY_DBG("rsize_w=%d, img->in_width=%d, rsize_h=%d, img->in_height=%d", 
	       rsize_w, img->in_width, rsize_h, img->in_height);
	img->ipu_ctrl |= (rsize_w > img->in_width) ? H_UP_SCALE : 0;
	img->ipu_ctrl |= (rsize_h > img->in_height) ? V_UP_SCALE : 0;

	// get the resize factor
	if (W != rsize_w) {
		Wsel = find_ipu_ratio_factor(ipu, img->in_width * IPU_RATIO_MUL / rsize_w, 1);
//              printk ("horizontal resize: sel = %d, n=%d, m=%d\n", Wsel, ipu_ratio_table[Wsel].n,
//              ipu_ratio_table[Wsel].m);
		W = rsize_w;
	}

	if (H != rsize_h) {
		Hsel =
		    find_ipu_ratio_factor(ipu, img->in_height * IPU_RATIO_MUL /
					  rsize_h, height_up);
//              printk ("Vertical resize: sel = %d, n=%d, m=%d\n", Hsel, ipu_ratio_table[Hsel].n,
//              ipu_ratio_table[Hsel].m);
		H = rsize_h;
	}

	// set IPU_INDEX register
	Width_lut_max =
	    width_up ? ipu_ratio_table[Wsel].m : ipu_ratio_table[Wsel].n;
	Height_lut_max =
	    height_up ? ipu_ratio_table[Hsel].m : ipu_ratio_table[Hsel].n;
	//OUTREG32(A_IPU_RSZ_COEF_INDEX, ((Height_lut_max - 1) << VE_IDX_H_BIT) | ((Width_lut_max - 1)  << HE_IDX_W_BIT));
	REG32(IPU_V_BASE + REG_RSZ_COEF_INDEX) =
	    ((Height_lut_max - 1) << VE_IDX_SFT)
	    | ((Width_lut_max - 1) << HE_IDX_SFT);

	// calculate out W/H and LUT
	srcN = ipu_ratio_table[Wsel].n;
	dstM = ipu_ratio_table[Wsel].m;
	*outWp =
	    resize_out_cal(img->in_width, rsize_w, srcN, dstM, width_up, Wdiff);
	//resize_out_cal( img->in_width, rsize_w, srcN, dstM, width_up, Wdiff);
	resize_lut_cal(srcN, dstM, width_up, h_lut);

	srcN = ipu_ratio_table[Hsel].n;
	dstM = ipu_ratio_table[Hsel].m;
	*outHp =
	    resize_out_cal(img->in_height, rsize_h, srcN, dstM, height_up,
			   Hdiff);
	//resize_out_cal( img->in_height, rsize_h, srcN, dstM, height_up, Hdiff);
	resize_lut_cal(srcN, dstM, height_up, v_lut);

	MY_DBG(" *outHp=%d, *outWp=%d", *outHp, *outWp);
	img->out_height = *outHp;
	img->out_width = *outWp;

	// set IPU LUT register
	//      SETREG32(A_IPU_VRSZ_COEF_LUT, LUT_START);
	REG32(IPU_V_BASE + VRSZ_LUT_BASE) = (1 << START_N_SFT);
	MY_DBG("Height_lut_max=%d\n", Height_lut_max);

	for (i = 0; i < Height_lut_max; i++) {
		if ((height_up == 0) && ((i & 1) == 0)) {
			if ((v_lut[i].out_n == 0) && (v_lut[i + 1].out_n == 1)) {
				v_lut[i].out_n = 1;
				v_lut[i + 1].out_n = 0;
			}
		}

		REG32(IPU_V_BASE + VRSZ_LUT_BASE) =
		    (v_lut[i].coef << W_COEF_SFT)
		    | (v_lut[i].in_n << IN_N_SFT) | (v_lut[i].
						     out_n << OUT_N_SFT);
	}

	//SETREG32(A_IPU_HRSZ_COEF_LUT, LUT_START);
	REG32(IPU_V_BASE + HRSZ_LUT_BASE) = (1 << START_N_SFT);
	MY_DBG("Width_lut_max=%d\n", Width_lut_max);
	for (i = 0; i < Width_lut_max; i++) {
		REG32(IPU_V_BASE + HRSZ_LUT_BASE) =
		    (h_lut[i].coef << W_COEF_SFT)
		    | (h_lut[i].in_n << IN_N_SFT) | (h_lut[i].
						     out_n << OUT_N_SFT);
		//  OUTREG32(A_IPU_HRSZ_COEF_LUT, (h_lut[i].coef << W_COEF_BIT)
		// | (h_lut[i].in_n << IN_EN_BIT) | (h_lut[i].out_n  << OUT_EN_BIT));
	}
#endif
	return (0);
}

////////////////////////////////////////////////////////////////////////////
static int jz47_set_ipu_buf(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img)
{
	//      int ret;
	unsigned int py_buf;
	unsigned int pu_buf;
	unsigned int pv_buf;
	unsigned int py_t_buf;
	unsigned int pu_t_buf;
	unsigned int pv_t_buf;
	unsigned int out_buf;
	//      PIPU_CTRL2 pipu_ctrl = (PIPU_CTRL2)(&img->ipu_ctrl);REG32(IPU_V_BASE + REG_CTRL)

	unsigned int spage_map;
	unsigned int dpage_map;
	unsigned int lcdc_sel;
	unsigned int in_fmt;

	struct ipu_img_param_t *img;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}

	/* wait ipu end flag */


	img = &ipu->img;

	if ( new_img ) {
		unsigned int old_ctrl = ipu->img.ipu_ctrl;
		unsigned int old_bpp = ipu->img.in_bpp;
		*img = *new_img;
		img->ipu_ctrl = old_ctrl;
		img->in_bpp = old_bpp;
	}

	spage_map = img->ipu_ctrl & SPAGE_MAP;
	dpage_map = img->ipu_ctrl & DPAGE_MAP;
	lcdc_sel = img->ipu_ctrl & IPU_LCDCSEL;

	py_buf = ((unsigned int) img->y_buf_p);
	pu_buf = ((unsigned int) img->u_buf_p);
	pv_buf = ((unsigned int) img->v_buf_p);

	py_t_buf = ((unsigned int) img->y_t_addr);
	pu_t_buf = ((unsigned int) img->u_t_addr);
	pv_t_buf = ((unsigned int) img->v_t_addr);

	in_fmt = img->ipu_d_fmt & IN_FMT_MASK;

	MY_DBG("py_buf=0x%08x, pu_buf=0x%08x, pv_buf=0x%08x, py_t_buf=0x%08x, pu_t_buf=0x%08x, pv_t_buf=0x%08x", 
	       py_buf, pu_buf, pv_buf, py_t_buf, pu_t_buf, pv_t_buf);

	MY_DBG("img->ipu_ctrl=%x, spage_map=%x, dpage_map=%x, lcdc_sel=%x",
	       img->ipu_ctrl, spage_map, dpage_map, lcdc_sel);

	if (spage_map != 0) {
		MY_DBG();
		if ((py_t_buf == 0) || (pu_t_buf == 0) || (pv_t_buf == 0)) {
			printk
			    (" Can not found source map table, use no map now!\r\n");
			spage_map = 0;
		} else {
		MY_DBG();
			py_t_buf = PHYS((unsigned int) img->y_t_addr);
			pu_t_buf = PHYS((unsigned int) img->u_t_addr);
			pv_t_buf = PHYS((unsigned int) img->v_t_addr);

			py_buf = py_t_buf & 0xfff;
			pu_buf = pu_t_buf & 0xfff;
			pv_buf = pv_t_buf & 0xfff;

			// set phy table addr
			REG32(IPU_V_BASE + REG_Y_PHY_T_ADDR) = py_t_buf;
			REG32(IPU_V_BASE + REG_U_PHY_T_ADDR) = pu_t_buf;
			REG32(IPU_V_BASE + REG_V_PHY_T_ADDR) = pv_t_buf;
		}
	} else {
		MY_DBG();
		if (py_buf == 0) {
			printk
			    ("++ py_buf Can not found buffer(0x%x,0x%x,0x%x) physical addr since addr errors +++\n",
			     (unsigned int) img->y_buf_p,
			     (unsigned int) img->u_buf_p,
			     (unsigned int) img->v_buf_p);
			return (-1);
		}
		if ( in_fmt == IN_FMT_YUV420) {
			if ((pu_buf == 0) || (pv_buf == 0)) {
				printk
					("++ Can not found buffer(0x%x,0x%x,0x%x) physical addr since addr errors +++\n",
					 (unsigned int) img->y_buf_p,
					 (unsigned int) img->u_buf_p,
					 (unsigned int) img->v_buf_p);
				return (-1);
			}
		}

	}

	MY_DBG("py_buf=0x%08x, pu_buf=0x%08x, pv_buf=0x%08x, py_t_buf=0x%08x, pu_t_buf=0x%08x, pv_t_buf=0x%08x", 
	       py_buf, pu_buf, pv_buf, py_t_buf, pu_t_buf, pv_t_buf);
	//set Y,U,V addr register
	REG32(IPU_V_BASE + REG_Y_ADDR) = py_buf;
	REG32(IPU_V_BASE + REG_U_ADDR) = pu_buf;
	REG32(IPU_V_BASE + REG_V_ADDR) = pv_buf;

	// set out put
	if ((dpage_map != 0) && (lcdc_sel == 0)) {
		MY_DBG();
		if (PHYS((unsigned int) img->out_t_addr) == 0) {
			printk
			    (" Can not found destination map table, use no map now!\r\n");
			//      pipu_ctrl->dpage_map = 0;

			if (PHYS((unsigned int) img->out_buf_p) == 0) {
				printk
				    ("Can not found the destination buf[%#x]\r\n",
				     (unsigned int) img->out_buf_p);
				return (-1);
			} else {
				REG32(IPU_V_BASE + REG_OUT_ADDR) =
				    PHYS((unsigned int) img->out_buf_p);
			}
		} else {
			REG32(IPU_V_BASE + REG_OUT_ADDR) =
			    PHYS((unsigned int) img->out_t_addr) & 0xfff;
			REG32(IPU_V_BASE + REG_OUT_PHY_T_ADDR) =
			    PHYS((unsigned int) img->out_t_addr);
		}
	} else {
		MY_DBG();
		dpage_map = 0;
		if (lcdc_sel == 0) {
			if (PHYS((unsigned int) img->out_buf_p) == 0) {
				printk
				    ("Can not found the destination buf[%#x]\r\n",
				     (unsigned int) img->out_buf_p);
				return (-1);
			} else {
				MY_DBG("img->out_buf_p=0x%x",img->out_buf_p);
				/*
				out_buf =
				    (unsigned int) img->out_buf_p +
				    (img->out_y * ipu->fb_w +
				     img->out_x) * img->in_bpp / 8; */
				out_buf = img->out_buf_p;
				REG32(REG_OUT_ADDR + IPU_V_BASE) =
				    PHYS(out_buf);
			}
		}
	}

	// flush the dcache
	// __dcache_writeback_all();

	return (0);
}

////////////////////////////////////////////////////////////////////////////////////
#ifndef IPU_FUNC_EX
static int resize_out_cal(int insize, int outsize, int srcN, int dstM,
			  int upScale, int *diff)
{
	unsigned int calsize, delta;
	unsigned int tmp, tmp2;

	delta = IPU_RATIO_MUL;
	insize *= IPU_RATIO_MUL;

	do {
		tmp = ((insize - delta) * dstM / srcN) & (~(IPU_RATIO_MUL - 1));
		tmp2 = tmp * srcN / dstM;
		if (upScale) {
			if (tmp2 == insize - delta) {
				calsize = tmp / IPU_RATIO_MUL + 1;
			} else {
				calsize = tmp / IPU_RATIO_MUL + 2;
			}
		} else		// down Scale
		{
			if (tmp2 == insize - delta) {
				calsize = tmp / IPU_RATIO_MUL;
			} else {
				calsize = tmp / IPU_RATIO_MUL + 1;
			}
		}

		delta += IPU_RATIO_MUL;

	} while (calsize > outsize);

	*diff = delta / IPU_RATIO_MUL - 2;

	return (calsize);
}


static int resize_lut_cal(int srcN, int dstM, int upScale, rsz_lut lut[])
{
	int i, t, x;
	unsigned int w_coef, factor, factor2;

	if (upScale) {
		for (i = 0, t = 0; i < dstM; i++) {
			factor = (i * srcN * IPU_RATIO_MUL) / dstM;
			factor2 =
			    factor - factor / IPU_RATIO_MUL * IPU_RATIO_MUL;
			w_coef = IPU_RATIO_MUL - factor2;
			lut[i].coef =
			    (unsigned int) (512 * w_coef /
					    IPU_RATIO_MUL) & W_COEF_MSK;
			// calculate in & out
			lut[i].out_n = 1;
			if (t <= factor) {
				lut[i].in_n = 1;
				t += IPU_RATIO_MUL;
			} else {
				lut[i].in_n = 0;
			}
		}		// end for
	} else {
		for (i = 0, t = 0, x = 0; i < srcN; i++) {
			factor = (t * srcN + 1) * IPU_RATIO_MUL / dstM;
			if (dstM == 1) {
				// calculate in & out
				lut[i].coef = (i == 0) ? 256 : 0;
				lut[i].out_n = (i == 0) ? 1 : 0;
			} else {
				if (((t * srcN + 1) / dstM - i) >= 1) {
					lut[i].coef = 0;
				} else {
					if (factor - i * IPU_RATIO_MUL == 0) {
						lut[i].coef = 512;
						t++;
					} else {
						factor2 =
						    (t * srcN) / dstM *
						    IPU_RATIO_MUL;
						factor = factor - factor2;
						w_coef = IPU_RATIO_MUL - factor;
						lut[i].coef =
						    (unsigned int) (512 *
								    w_coef /
								    IPU_RATIO_MUL)
						    & W_COEF_MSK;
						t++;
					}
				}
			}
			// calculate in & out
			lut[i].in_n = 1;
			if (dstM != 1) {
				if (((x * srcN + 1) / dstM - i) >= 1) {
					lut[i].out_n = 0;
				} else {
					lut[i].out_n = 1;
					x++;
				}
			}

		}		// for end down Scale
	}			// else upScale

	return 0;
}
#endif

static int ipu_irq_cnt = 0;
static irqreturn_t ipu_interrupt_handler(int irq, void *dev_id)
{
	struct ipu_img_param_t *img;
	struct ipu_driver_priv *ipu = (struct ipu_driver_priv *) dev_id;
	unsigned int irq_flags;

	unsigned int dummy_read = REG32(IPU_V_BASE + REG_STATUS); /* avoid irq looping or disable_irq*/
	disable_irq(IPU_V_BASE); // failed
	img = &ipu->img;

	if (img->output_mode & IPU_OUTPUT_BLOCK_MODE) {
		spin_lock_irqsave(&ipu->update_lock, irq_flags);
	}

#if 0
	unsigned int ctrl;
	//      if (ipu_change_buf != 0)
	{
		if (((REG32(IPU_V_BASE + REG_STATUS)) & OUT_END) == 0) {
			printk("outend\n");
			return -1;
		}

		REG32(IPU_V_BASE + REG_STATUS) &= ~OUT_END;
		ctrl = REG32(IPU_V_BASE + REG_CTRL);
		if (!(ctrl & FIELD_SEL)) {
			// jz47_set_ipu_buf();
			REG32(IPU_V_BASE + REG_Y_ADDR) =
			    PHYS((unsigned int) img->y_buf);
			REG32(IPU_V_BASE + REG_U_ADDR) =
			    PHYS((unsigned int) img->u_buf);
			REG32(IPU_V_BASE + REG_V_ADDR) =
			    PHYS((unsigned int) img->v_buf);
			//REG32(IPU_V_BASE + REG_ADDR_CTRL) = YUV_READY;
		} else {
			REG32(IPU_V_BASE + REG_Y_ADDR) =
			    PHYS((unsigned int) img->y_buf + img->stride->y);
			REG32(IPU_V_BASE + REG_U_ADDR) =
			    PHYS((unsigned int) img->u_buf + img->stride->u);
			REG32(IPU_V_BASE + REG_V_ADDR) =
			    PHYS((unsigned int) img->v_buf + img->stride->v);
			//REG32(IPU_V_BASE + REG_ADDR_CTRL) = YUV_READY;

		}
		// ipu_change_buf = 0;
		//OUTREG32(A_IPU_Y_STRIDE, *py_stride);
		//OUTREG32(A_IPU_UV_STRIDE, U_STRIDE(*pu_stride) | V_STRIDE(*pv_stride));

		REG32(IPU_V_BASE + REG_CTRL) |= IPU_RUN;
		//printk(" %s\n", __FUNCTION__);

		//IPU_INTC_DISABLE();
	}
#else
//	printk("*** ipu_interrupt_handler() not implemented\n");
//	printk("REG32(IPU_V_BASE + REG_STATUS)=0x%08x\n", REG32(IPU_V_BASE + REG_STATUS));
#endif

	if (img->output_mode & IPU_OUTPUT_BLOCK_MODE) {
		ipu->frame_done = ipu->frame_requested;
		spin_unlock_irqrestore(&ipu->update_lock, irq_flags);
		wake_up(&ipu->frame_wq);
	}

	ipu_irq_cnt++;

//	printk("ipu_irq_cnt=%d, ipu->frame_requested=%d\n", ipu_irq_cnt, ipu->frame_requested);
//	printk("*** ipu_interrupt_handler() not implemented\n");
//	printk("REG32(IPU_V_BASE + REG_STATUS)=0x%08x, ipu->frame_done=%d\n", 
//	       REG32(IPU_V_BASE + REG_STATUS), ipu->frame_done);

	return IRQ_HANDLED;
}


int jz47_ipu_init(struct ipu_driver_priv *ipu, struct ipu_img_param_t *img)
{
	int ret = -1;
	int in_fmt;
	int out_fmt;
	int outW, outH, Wdiff, Hdiff;

	ENTER();

#ifdef IPU_FUNC_EX
	if (!img || !resize_lut_cal || !resize_out_cal)
#else
	if (!img)
#endif
	{
		printk("ipu_init: parameter error\r\n");
		return (ret);
	}

	switch ( img->output_mode & IPU_OUTPUT_MODE_MASK) {
	case IPU_OUTPUT_TO_LCD_FG1:
		img->ipu_ctrl = IPU_LCDCSEL | IPU_ADDRSEL;
		break;
	default:
		img->ipu_ctrl = IPU_ADDRSEL; /*  */
	}

	outW = img->out_width;
	outH = img->out_height;
	in_fmt = img->ipu_d_fmt & IN_FMT_MASK;
	out_fmt = img->ipu_d_fmt & OUT_FMT_MASK;
	MY_DBG("outW=%d, outH=%d, Wdiff=%d, Hdiff=%d", outW, outH, Wdiff, Hdiff);

	img->ipu_ctrl |= FM_IRQ_EN; /* enable irq */

	//img->ipu_ctrl &= (LCDC_SEL | FM_IRQ_EN | ADDR_SEL | FIELD_SEL | DISP_SEL | FIELD_CONF_EN);

	MY_DBG("ipu->inited=%d", ipu->inited);
	if (ipu->inited == 0) {
		REG32(CPM_CLKGR_VADDR) &= ~(IPU_CLOCK);	// run ipu clock
		//request_irq(IRQ_IPU, ipu_interrupt_handler, 0);

		REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_RUN;
		REG32(IPU_V_BASE + REG_CTRL) |= IPU_EN;
		REG32(IPU_V_BASE + REG_CTRL) |= IPU_RESET;	// reset controller
		REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_RESET;

		jz47_ipu_wait_frame_end_flag();
	} else {
//		while (((REG32(IPU_V_BASE + REG_CTRL)) & IPU_RUN) && (REG32(IPU_V_BASE + REG_STATUS) & OUT_END) == 0) ;	// wait the end flag
		if ((REG32(IPU_V_BASE + REG_CTRL)) & IPU_RUN)
			jz47_ipu_wait_frame_end_flag();

		REG32(IPU_V_BASE + REG_CTRL) |= IPU_RESET;	// reset controller
		REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_RESET;
		jz47_ipu_wait_frame_end_flag();
	}

	if ((in_fmt == IN_FMT_YUV444) && (out_fmt != OUT_FMT_YUV422)) {
		img->ipu_ctrl &= ~SPKG_SEL;
	}

	if ((in_fmt == IN_FMT_YUV422)) { /* YUV422 force to package format, Wolfgang, 2010-04-01 */
		img->ipu_ctrl |= SPKG_SEL;
	}

	MY_DBG("outW=%d, outH=%d, Wdiff=%d, Hdiff=%d", outW, outH, Wdiff, Hdiff);
	// set IPU resize field
	jz47_set_ipu_resize(ipu, &outW, &outH, &Wdiff, &Hdiff);
	MY_DBG("outW=%d, outH=%d, Wdiff=%d, Hdiff=%d", outW, outH, Wdiff, Hdiff);
	// set CSC parameter and format
	ret = jz47_set_ipu_csc_cfg(ipu, outW, outH, Wdiff, Hdiff);
	if (ret != 0) {
		printk("jz47_set_ipu_csc_cfg error : out!\n");
		return (ret);
	}
	// set the input and output buffer
//	ret = jz47_set_ipu_buf(ipu, NULL);
//	if (ret < 0) {
//		return (ret);
//	}

	// set  stride
	if (img->stride.y == 0) {	/* set default stride */
		if (img->ipu_ctrl & SPKG_SEL) {
			REG32(IPU_V_BASE + REG_Y_STRIDE) = img->in_width * 2;
		} else {
			REG32(IPU_V_BASE + REG_Y_STRIDE) = img->in_width;
		}

		//switch(((PIPU_DFMT2)&(img->ipu_d_fmt))->in_fmt)
		switch (in_fmt) {	///EMILY
		case IN_FMT_YUV420:
		case IN_FMT_YUV422:
			REG32(IPU_V_BASE + REG_UV_STRIDE) =
			    U_STRIDE(img->in_width /
				     2) | V_STRIDE(img->in_width / 2);
			break;
		case IN_FMT_YUV444:
			REG32(IPU_V_BASE + REG_UV_STRIDE) =
			    U_STRIDE(img->in_width) | V_STRIDE(img->in_width);
			break;
		case IN_FMT_YUV411:
			REG32(IPU_V_BASE + REG_UV_STRIDE) =
			    U_STRIDE(img->in_width /
				     4) | V_STRIDE(img->in_width / 4);
			break;
		default:
			printk("Error: Input data format isn't support\n");
			return (-1);
		}
	} else {
		REG32(IPU_V_BASE + REG_Y_STRIDE) = img->stride.y;
		REG32(IPU_V_BASE + REG_UV_STRIDE) =
		    U_STRIDE(img->stride.u) | V_STRIDE(img->stride.v);
	}

	//CLRREG32(A_IPU_STATUS, OUT_END);
	// set the ctrl
	REG32(IPU_V_BASE + REG_CTRL) = img->ipu_ctrl | IPU_EN;

	ipu->inited = 1;

//      jz47_dump_ipu_regs(-1);
//      jz47_dump_ipu_regs(-2); //mdelay(1000); /* delay 1s */
	LEAVE();
	return (0);
}


#if 0
//int jz47_ipu_resize(void)
int jz47_ipu_resize(struct ipu_img_param_t *resize_img)
{
	int ret = -1;
	int outW, outH, Wdiff, Hdiff;
	//unsigned int out_buf;
//      img = resize_img;//1030
	g_img = *resize_img;
	ret = get_fbaddr_info();
	if ( ret < 0) {
		printk("get_fbaddr_info error\n");
		return ret;
	}

	//1104 while ((REG32(IPU_V_BASE + REG_CTRL) & IPU_RUN) && ((REG32(IPU_V_BASE + REG_STATUS) & OUT_END) == 0)); // wait the end flag
	REG32(IPU_V_BASE + REG_CTRL) |= IPU_RESET;	// reset controller
	REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_RESET;

	jz47_ipu_wait_frame_end_flag();

	// set IPU resize field
	jz47_set_ipu_resize(ipu, &outW, &outH, &Wdiff, &Hdiff);

	// set CSC parameter and format
	ret = jz47_set_ipu_csc_cfg(outW, outH, Wdiff, Hdiff);
	if (ret != 0) {
		printk("jz47_set_ipu_csc_cfg error\n");
		return (ret);
	}


	REG32(IPU_V_BASE + REG_Y_STRIDE) = *py_stride;
	REG32(IPU_V_BASE + REG_UV_STRIDE) =
	    U_STRIDE(*pu_stride) | V_STRIDE(*pv_stride);
	// set the input and out put buffer
	if (jz47_set_ipu_buf() < 0) {
		ret = -1;
		return (ret);
	}
	// set the ctrl
	REG32(IPU_V_BASE + REG_CTRL) = img->ipu_ctrl | IPU_EN;
	return (0);
}
#endif

#if 0
int ipu_deinit(void)
{
	printk("%s START,%d\n", __FUNCTION__, __LINE__);
	if ((ipu_state & IPU_OPEN) != 0) {
		REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_RUN;
	}

	free_irq(IRQ_IPU, 0);

	// disable the ipu module
	REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_EN;

	// stop the ipu clock   
	REG32(CPM_CLKGR_VADDR) |= (IPU_CLOCK);

	ipu_inited = 0;
	ipu_state = 0;
	printk("%s END,%d\n", __FUNCTION__, __LINE__);
	return 0;
}
#endif


int ipu_driver_open(struct ipu_driver_priv *ipu)
{
	ENTER();

	if (ipu == NULL) {
		return -1;
	}

	ipu = &g_ipu_native_data;
	ipu->inited = 0;

	ipu->lcd_info = &jz4750_lcd_panel;

	return 0;
}


int ipu_driver_close(struct ipu_driver_priv *ipu)
{
	struct ipu_img_param_t *img;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}
	img = &ipu->img;

	/* disable fg1 */
	switch ( img->output_mode & IPU_OUTPUT_MODE_MASK) {
	case IPU_OUTPUT_TO_LCD_FG1:
		__lcd_disable_f1();
		break;
	case IPU_OUTPUT_TO_LCD_FB0:
		break;
	case IPU_OUTPUT_TO_LCD_FB1:
		break;
	case IPU_OUTPUT_TO_FRAMEBUFFER:
		break;
	default:
		break;
	}


	if (img->output_mode & IPU_OUTPUT_TO_LCD_FG1) {
		/* disable alpha, colorkey */
		__lcd_disable_alpha();
		__lcd_disable_colorkey0();


		/* Reset LCD Controller */

		__lcd_clr_ena();

		mdelay(25);		/* delay 100ms */


		__lcd_set_ena();	// enable lcd
	}

	return 0;
}

/*
 * ipu_init(), init ipu, not start ipu
 */

int ipu_driver_init(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img)
{
	int ret;
	struct ipu_img_param_t *img;
//	struct jz4750lcd_fg_t  fg1;

	ENTER();
	if (ipu == NULL) {
		return -1;
	}

	spin_lock_init(&ipu->update_lock);
	init_waitqueue_head(&ipu->frame_wq);
	ipu->frame_requested = ipu->frame_done = 0;
	ipu_irq_cnt = 0;

	img = &ipu->img;
	*img = *new_img;	/* use the new parameter */

#if 1
	switch ( img->output_mode & IPU_OUTPUT_MODE_MASK) {
	case IPU_OUTPUT_TO_LCD_FG1:
		ipu->fb_w = ipu->lcd_info->osd.fg1.w; /* fg0, fg1 ??? */
		ipu->fb_h = ipu->lcd_info->osd.fg1.h;

		/* lcd controller configure */
		/* check dest rect? */
		if ( img->out_x > ipu->fb_w || img->out_y > ipu->fb_h) {
			printk("*** jz4750 ipu dest rect out range error.\n");
			return -1;
		}

		if ( img->out_x + img->out_width > ipu->fb_w )
			img->out_width = ipu->fb_w - img->out_x;
		if ( img->out_y + img->out_height > ipu->fb_h )
			img->out_height = ipu->fb_h - img->out_y;

		REG_LCD_XYP1 = (img->out_y << 16) | (img->out_x << 0);
		REG_LCD_SIZE1 = (img->out_height << 16) | (img->out_width << 0);
		REG_LCD_STATE = 0;
		break;
	case IPU_OUTPUT_TO_LCD_FB0:
		break;
	case IPU_OUTPUT_TO_LCD_FB1:
		break;
	case IPU_OUTPUT_TO_FRAMEBUFFER:
		break;
	default:
		break;
	}
	/* set alpha, colorkey */
	if (img->output_mode & IPU_DST_USE_ALPHA ) {
		REG_LCD_ALPHA = img->alpha;	/* Max: 0xFF(255) */
		__lcd_enable_alpha();
	}

	if (img->output_mode & IPU_DST_USE_COLOR_KEY ) {
		__lcd_set_colorkey0(img->colorkey);
		__lcd_enable_colorkey0();
	}
#endif

	ret = jz47_ipu_init(ipu, img);	// old init
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int ipu_driver_deinit(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img)
{
	ENTER();

	if (ipu == NULL) {
		return -1;
	}

	return 0;
}

int ipu_driver_start(struct ipu_driver_priv *ipu)
{
	struct ipu_img_param_t *img;
	unsigned long irq_flags;
	ENTER();

	if (ipu == NULL) {
		return -1;
	}

	img = &ipu->img;

#ifdef IPU_DBG
	ipu_driver_dump_regs(ipu);
	//print_lcdc_registers();	/* debug */
#endif

//	ipu_state |= IPU_OPEN;
	if (img->output_mode & IPU_OUTPUT_TO_LCD_FG1) {
		//__lcd_close_backlight();
		__lcd_clr_ena();

//      jz4750_lcd_info->osd.osd_cfg |= LCD_OSDC_F1EN;
		ipu->lcd_info->osd.osd_ctrl |= LCD_OSDCTRL_IPU;
		REG_LCD_OSDCTRL = ipu->lcd_info->osd.osd_ctrl;
	}

//	if (img->output_mode & IPU_OUTPUT_BLOCK_MODE) {
	if (1) {
		/* Wait for current frame to finished */
		spin_lock_irqsave(&ipu->update_lock, irq_flags);
		ipu->frame_requested++;
		spin_unlock_irqrestore(&ipu->update_lock, irq_flags);
	}


	/* start ipu */
	REG32(IPU_V_BASE + REG_STATUS) &= ~OUT_END;
	if (img->output_mode & IPU_OUTPUT_BLOCK_MODE) {
		enable_irq(IPU_V_BASE);
	}

	REG32(IPU_V_BASE + REG_CTRL) |= IPU_RUN;

	if (img->output_mode & IPU_OUTPUT_TO_LCD_FG1) {
		__lcd_enable_f1();
		__lcd_enable_ipu_restart();

		udelay(10);
		__lcd_set_ena();	// enable lcd
		udelay(200);
//      __lcd_display_on();
//      __lcd_set_backlight_level(lcd_backlight_level);
	}

	if (img->output_mode & IPU_OUTPUT_BLOCK_MODE) {
		/* Wait for current frame to finished */
		if (ipu->frame_requested != ipu->frame_done)
			wait_event_interruptible_timeout(
				ipu->frame_wq, ipu->frame_done == ipu->frame_requested, HZ/10 + 1); /* HZ = 100 */
	}
	return 0;
}

int ipu_driver_stop(struct ipu_driver_priv *ipu)
{
	struct ipu_img_param_t *img;
	ENTER();

	if (ipu == NULL) {
		return -1;
	}

	img = &ipu->img;

	if (img->output_mode & IPU_OUTPUT_TO_LCD_FG1) {
		/* Disable lcd fg1 */
		__lcd_disable_f1();
		
	}

	REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_RUN;
//	ipu_state &= ~IPU_OPEN;

	return 0;
}

int ipu_driver_swapBuffer(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img)
{
	ENTER();

	if (ipu == NULL) {
		return -1;
	}

	return jz47_set_ipu_buf(ipu, new_img);
}

int ipu_driver_setBuffer(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img)
{
	ENTER();

	if (ipu == NULL) {
		return -1;
	}


	return jz47_set_ipu_buf(ipu, new_img);
}

int ipu_driver_resize(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img)
{
	int ret;
	ENTER();

	if (ipu == NULL) {
		return -1;
	}

	ret = ipu_driver_init(ipu, new_img);
	return ret;
}


int ipu_driver_register_irq(struct ipu_driver_priv *ipu)
{
	int ret;
	ret = request_irq(IRQ_IPU, ipu_interrupt_handler, IRQF_DISABLED, "ipu", (void*)ipu);
	if ( ret ) {
		printk("Error: request_irq IRQ_IPU=%d\n", IRQ_IPU);
	}

	return ret;
}


int ipu_driver_dump_regs(struct ipu_driver_priv *ipu)
{
	ENTER();

	if (ipu == NULL) {
		return -1;
	}

	print_img(ipu);
	jz47_dump_ipu_regs(ipu, -1);
	jz47_dump_ipu_regs(ipu, -2);

	return 0;
}

int ipu_driver_ioctl(struct ipu_driver_priv *ipu, void *uimg)
{
	unsigned int cmd;
	int ret = 0;
	struct ipu_img_param_t *m_img;
	struct ipu_img_param_t ipu_img;

	if (uimg == NULL) {
		return -1;
	}

	copy_from_user((void *) &ipu_img, (void *)uimg,
		       sizeof(struct ipu_img_param_t));

	m_img = &ipu->img;
	if (ipu_img.version != m_img->version) {
		printk("*** Warning, ipu_img_param_t.version wrong ***\n");
		return -1;
	}

	cmd = ipu_img.ipu_cmd;
	MY_DBG("ipu_ioctl cmd= %d\n", cmd);
	switch (cmd) {
	case IOCTL_IPU_OPEN:
		ret = ipu_driver_open(ipu);
		break;
	case IOCTL_IPU_CLOSE:
		ret = ipu_driver_close(ipu);
		break;
	case IOCTL_IPU_INIT:
		ret = ipu_driver_init(ipu, &ipu_img);
		break;
	case IOCTL_IPU_DEINIT:
		break;
//	case IOCTL_IPU_CHANGE_BUFF:
	case IOCTL_IPU_SET_BUFF:
		ret = ipu_driver_setBuffer(ipu, &ipu_img);
		break;
	case IOCTL_IPU_START:
		ret = ipu_driver_start(ipu);
		break;
	case IOCTL_IPU_STOP:
		ret = ipu_driver_stop(ipu);
		break;
	case IOCTL_IPU_RESIZE:
		ipu_driver_resize(ipu, &ipu_img);
		break;
	case IOCTL_IPU_SET_CTRL_REG:
		break;
	case IOCTL_IPU_SET_FMT_REG:
		break;
	case IOCTL_IPU_SET_CSC:
		break;
	case IOCTL_IPU_SET_STRIDE:
		break;
	case IOCTL_IPU_FB_SIZE:
		break;
	case IOCTL_IPU_PENDING_OUTEND:
		jz47_ipu_wait_frame_end_flag();
		REG32(IPU_V_BASE + REG_STATUS) &= ~OUT_END;
		break;
	case IOCTL_IPU_GET_ADDR:
		printk(" IOCTL_IPU_GET_ADDR:%s[%d] function droped.\n",
		       __FILE__, __LINE__);
		//get_ipu_addr((unsigned int*)buff);
		break;
	case IOCTL_IPU_DUMP_REGS:
		ret = ipu_driver_dump_regs(ipu);
		break;
	default:
		printk("ipu not support ioctl cmd[%#x]\r\n", cmd);
		return (-1);
		break;
	}

	return ret;
}
