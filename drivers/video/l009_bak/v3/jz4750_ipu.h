#ifndef _IPU_H_
#define _IPU_H_

#ifdef __KERNEL__

//#define REG32(val)  (*((volatile unsigned int *)(val)))

// CLKGR
//#define CPM_CLKGR         0x10000020
#define CPM_CLKGR_VADDR   0xB0000020

// Module for CLKGR
#define IDCT_CLOCK      (1 << 27)
#define DBLK_CLOCK      (1 << 26)
#define ME_CLOCK        (1 << 25)
#define MC_CLOCK        (1 << 24)
#define IPU_CLOCK       (1 << 13)

// IPU_REG_BASE
#ifndef IPU_V_BASE
#define IPU_V_BASE  0xB3080000
#endif
#ifndef IPU_P_BASE
#define IPU_P_BASE  0x13080000
#endif
#define IPU__OFFSET 0x13080000
#define IPU__SIZE   0x00001000

#define ME__SIZE   0x00001000
#define MC__SIZE   0x00001000
#define DBLK__SIZE   0x00001000
#define IDCT__SIZE   0x00001000


#define ME__OFFSET 0x130A0000
#define MC__OFFSET 0x13090000
#define DBLK__OFFSET 0x130B0000
#define IDCT__OFFSET 0x130C0000



// Register offset
#define  REG_CTRL           0x0
#define  REG_STATUS         0x4
#define  REG_D_FMT          0x8
#define  REG_Y_ADDR         0xc
#define  REG_U_ADDR         0x10
#define  REG_V_ADDR         0x14
#define  REG_IN_FM_GS       0x18
#define  REG_Y_STRIDE       0x1c
#define  REG_UV_STRIDE      0x20
#define  REG_OUT_ADDR       0x24
#define  REG_OUT_GS         0x28
#define  REG_OUT_STRIDE     0x2c
#define  REG_RSZ_COEF_INDEX 0x30
#define  REG_CSC_C0_COEF    0x34
#define  REG_CSC_C1_COEF    0x38
#define  REG_CSC_C2_COEF    0x3c
#define  REG_CSC_C3_COEF    0x40
#define  REG_CSC_C4_COEF    0x44
#define  HRSZ_LUT_BASE      0x48
#define  VRSZ_LUT_BASE      0x4c
#define  REG_CSC_OFFPARA    0x50
#define  REG_ADDR_CTRL      0x64

// REG_CTRL field define
#define IPU_EN          (1 << 0)
#define IPU_RUN         (1 << 1)
#define HRSZ_EN         (1 << 2)
#define VRSZ_EN         (1 << 3)
#define CSC_EN          (1 << 4)
#define FM_IRQ_EN       (1 << 5)
#define IPU_RESET       (1 << 6)
#define IPU_STOP        (1 << 7)
#define H_UP_SCALE      (1 << 8)
#define V_UP_SCALE      (1 << 9)
#define H_SCALE_SHIFT      (8)
#define V_SCALE_SHIFT      (9)
#define SPKG_SEL        (1 << 10)
#define IPU_LCDCSEL     (1 << 11)
#define SPAGE_MAP       (1 << 12)
#define DPAGE_MAP       (1 << 13)
#define DISP_SEL        (1 << 14)
#define FIELD_CONF_EN   (1 << 15)
#define FIELD_SEL       (1 << 16)
#define IPU_ADDRSEL     (1 << 20) /* jz4750 not support? */


//#define YUV_READY       (1<<)

// REG_STATUS field define
#define OUT_END         (1 << 0)

// REG_D_FMT field define
#define INFMT_YUV420    (0 << 0)
#define INFMT_YUV422    (1 << 0)
#define INFMT_YUV444    (2 << 0)
#define INFMT_YUV411    (3 << 0)
#define INFMT_YCbCr420  (0 << 0)
#define INFMT_YCbCr422  (1 << 0)
#define INFMT_YCbCr444  (2 << 0)
#define INFMT_YCbCr411  (3 << 0)

#define OUTFMT_RGB555   (0 << 19)
#define OUTFMT_RGB565   (1 << 19)
#define OUTFMT_RGB888   (2 << 19)
#define OUTFMT_YUV422   (3 << 19)

// REG_IN_FM_GS field define
#define IN_FM_W(val)    ((val) << 16)
#define IN_FM_H(val)    ((val) << 0)

// REG_IN_FM_GS field define
#define OUT_FM_W(val)    ((val) << 16)
#define OUT_FM_H(val)    ((val) << 0)

// REG_UV_STRIDE field define
#define U_STRIDE(val)     ((val) << 16)
#define V_STRIDE(val)     ((val) << 0)


//#define VE_IDX_SFT        0
//#define HE_IDX_SFT        16

// RSZ_LUT_FIELD
//#define OUT_N_SFT         0
#define OUT_N_MSK         0x1
//#define IN_N_SFT          1
#define IN_N_MSK          0x1
//#define W_COEF_SFT        2
#define W_COEF_MSK        0x3FF
#define START_N_SFT       12

// function about REG_CTRL
#define stop_ipu(IPU_V_BASE) \
REG32(IPU_V_BASE + REG_CTRL) |= IPU_STOP;

#define run_ipu(IPU_V_BASE) \
REG32(IPU_V_BASE + REG_CTRL) |= IPU_RUN;

#define enable_ipu_addrsel(IPU_V_BASE) \
(REG32(IPU_V_BASE + REG_CTRL) |= IPU_ADDRSEL)

#define enable_yuv_ready(IPU_V_BASE) \
(REG32(IPU_V_BASE + REG_ADDR_CTRL) = 0x7)

#define disable_ipu_addrsel(IPU_V_BASE) \
(REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_ADDRSEL)

#define enable_ipu_direct(IPU_V_BASE) \
(REG32(IPU_V_BASE + REG_CTRL) |= IPU_LCDCSEL)

#define disable_ipu_direct(IPU_V_BASE) \
(REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_LCDCSEL)

#define enable_ipu(IPU_V_BASE) \
(REG32(IPU_V_BASE + REG_CTRL) |= IPU_EN)

#define ipu_enable(IPU_V_BASE) \
  (REG32(IPU_V_BASE + REG_CTRL) |= IPU_EN)

#define ipu_disable(IPU_V_BASE) \
  (REG32(IPU_V_BASE + REG_CTRL) &= ~IPU_EN)

#define ipu_is_enable(IPU_V_BASE) \
(REG32(IPU_V_BASE + REG_CTRL) & IPU_EN)

#ifdef __LINUX__
#define reset_ipu(IPU_V_BASE) \
do {                                        \
  /*REG32(CPM_CLKGR_VADDR) &= ~(IPU_CLOCK);*/   \
  REG32(IPU_V_BASE + REG_CTRL) = IPU_RESET;  \
  REG32(IPU_V_BASE + REG_CTRL) = IPU_EN;     \
} while (0)
#else
#define reset_ipu(IPU_V_BASE) \
do {                                        \
  REG32(CPM_CLKGR_VADDR) &= ~(IPU_CLOCK);   \
  REG32(IPU_V_BASE + REG_CTRL) = IPU_RESET;  \
  REG32(IPU_V_BASE + REG_CTRL) = IPU_EN;     \
} while (0)
#endif

#define ipu_reg_val(IPU_V_BASE) \
do {                                                                          \
	printf("\n\nREG_CTRL=0x%08x\n",REG32(IPU_V_BASE + REG_CTRL));         \
	printf("REG_D_FMT=0x%08x\n",REG32(IPU_V_BASE + REG_D_FMT));           \
	printf("REG_Y_ADDR=0x%08x\n",REG32(IPU_V_BASE + REG_Y_ADDR));         \
	printf("REG_U_ADDR=0x%08x\n",REG32(IPU_V_BASE + REG_U_ADDR));         \
	printf("REG_V_ADDR=0x%08x\n",REG32(IPU_V_BASE + REG_V_ADDR));         \
	printf("REG_IN_FM_GS=0x%08x\n",REG32(IPU_V_BASE + REG_IN_FM_GS));     \
	printf("REG_Y_STRIDE=0x%08x\n",REG32(IPU_V_BASE + REG_Y_STRIDE));     \
	printf("REG_UV_STRIDE=0x%08x\n",REG32(IPU_V_BASE + REG_UV_STRIDE));   \
	printf("REG_OUT_ADDR=0x%08x\n",REG32(IPU_V_BASE + REG_OUT_ADDR));     \
	printf("REG_OUT_GS=0x%08x\n",REG32(IPU_V_BASE + REG_OUT_GS));         \
	printf("REG_OUT_STRIDE=0x%08x\n",REG32(IPU_V_BASE + REG_OUT_STRIDE)); \
} while (0)

#define sw_reset_ipu(IPU_V_BASE) \
do {                                        \
  REG32(IPU_V_BASE + REG_CTRL) = IPU_RESET;  \
} while (0)

#define enable_irq(IPU_V_BASE) \
REG32(IPU_V_BASE + REG_CTRL) |= FM_IRQ_EN;

#define disable_irq(IPU_V_BASE) \
REG32(IPU_V_BASE + REG_CTRL) &= ~FM_IRQ_EN;

#define disable_rsize(IPU_V_BASE) \
REG32(IPU_V_BASE + REG_CTRL) &= ~(HRSZ_EN | VRSZ_EN);

#define enable_hrsize(IPU_V_BASE) \
REG32(IPU_V_BASE + REG_CTRL) |= HRSZ_EN;

#define enable_vrsize(IPU_V_BASE) \
REG32(IPU_V_BASE + REG_CTRL) |= VRSZ_EN;

#define ipu_is_run(IPU_V_BASE) \
  (REG32(IPU_V_BASE + REG_CTRL) & IPU_RUN)

// function about REG_STATUS
#define clear_end_flag(IPU_V_BASE) \
do {\
REG32(IPU_V_BASE +  REG_STATUS) &= ~OUT_END;		\
printf("++++++clear_end_flag++++++++\n");\
}while(0);

#if 0
#define polling_end_flag(IPU_V_BASE) \
do {\
	(REG32(IPU_V_BASE +  REG_STATUS) & OUT_END);\
	printf("++++++polling_end_flag++++++++\n");\
}while(0);
#endif
#define polling_end_flag(IPU_V_BASE) \
	(REG32(IPU_V_BASE +  REG_STATUS) & OUT_END)

#define set_end_flag(IPU_V_BASE) \
	(REG32(IPU_V_BASE +  REG_STATUS) |= OUT_END)
// parameter
// R = 1.164 * (Y - 16) + 1.596 * (cr - 128)    {C0, C1}
// G = 1.164 * (Y - 16) - 0.392 * (cb -128) - 0.813 * (cr - 128)  {C0, C2, C3}
// B = 1.164 * (Y - 16) + 2.017 * (cb - 128)    {C0, C4}

#if 1
#define YUV_CSC_C0 0x4A8        /* 1.164 * 1024 */
#define YUV_CSC_C1 0x662        /* 1.596 * 1024 */
#define YUV_CSC_C2 0x191        /* 0.392 * 1024 */
#define YUV_CSC_C3 0x341        /* 0.813 * 1024 */
#define YUV_CSC_C4 0x811        /* 2.017 * 1024 */

//#define YUV_CSC_CHROM				128
//#define YUV_CSC_LUMA				0

#define YUV_CSC_OFFPARA         0x800010  /* chroma,luma */
  //#define YUV_CSC_OFFPARA         0x800080  /* chroma,luma */
#else
#define YUV_CSC_C0 0x400
#define YUV_CSC_C1 0x59C
#define YUV_CSC_C2 0x161
#define YUV_CSC_C3 0x2DC
#define YUV_CSC_C4 0x718
#endif


#endif	/* #ifdef __KERNEL__ */

///////////////////////////////////////////

// Data Format Register
#define RGB_888_OUT_FMT				( 1 << 24 )

#define RGB_OUT_OFT_BIT				( 21 )
#define RGB_OUT_OFT_MASK			( 7 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_RGB				( 0 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_RBG				( 1 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_GBR				( 2 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_GRB				( 3 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_BRG				( 4 << RGB_OUT_OFT_BIT )
#define RGB_OUT_OFT_BGR				( 5 << RGB_OUT_OFT_BIT )

#define OUT_FMT_BIT					( 19 )
#define OUT_FMT_MASK				( 3 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB555				( 0 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB565				( 1 <<  OUT_FMT_BIT )
#define OUT_FMT_RGB888				( 2 <<  OUT_FMT_BIT )
#define OUT_FMT_YUV422				( 3 <<  OUT_FMT_BIT )

#define YUV_PKG_OUT_OFT_BIT			( 16 )
#define YUV_PKG_OUT_OFT_MASK		( 7 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y1UY0V		( 0 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y1VY0U		( 1 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_UY1VY0		( 2 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_VY1UY0		( 3 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y0UY1V		( 4 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_Y0VY1U		( 5 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_UY0VY1		( 6 << YUV_PKG_OUT_OFT_BIT )
#define YUV_PKG_OUT_OFT_VY0UY1		( 7 << YUV_PKG_OUT_OFT_BIT )

#define IN_OFT_BIT					( 2 )
#define IN_OFT_MASK					( 3 << IN_OFT_BIT )
#define IN_OFT_Y1UY0V				( 0 << IN_OFT_BIT )
#define IN_OFT_Y1VY0U				( 1 << IN_OFT_BIT )
#define IN_OFT_UY1VY0				( 2 << IN_OFT_BIT )
#define IN_OFT_VY1UY0				( 3 << IN_OFT_BIT )

#define IN_FMT_BIT					( 0 )
#define IN_FMT_MASK					( 3 << IN_FMT_BIT )
#define IN_FMT_YUV420				( 0 << IN_FMT_BIT )
#define IN_FMT_YUV422				( 1 << IN_FMT_BIT )
#define IN_FMT_YUV444				( 2 << IN_FMT_BIT )
#define IN_FMT_YUV411				( 3 << IN_FMT_BIT )


struct ipu_module
{
  unsigned int reg_ctrl;      // 0x0
  unsigned int reg_status;    // 0x4
  unsigned int reg_d_fmt;     // 0x8
  unsigned int reg_y_addr;    // 0xc
  unsigned int reg_u_addr;    // 0x10
  unsigned int reg_v_addr;    // 0x14
  unsigned int reg_in_fm_gs;  // 0x18
  unsigned int reg_y_stride;  // 0x1c
  unsigned int reg_uv_stride; // 0x20
  unsigned int reg_out_addr;  // 0x24
  unsigned int reg_out_gs;    // 0x28
  unsigned int reg_out_stride;// 0x2c
  unsigned int rsz_coef_index;// 0x30
  unsigned int reg_csc_c0_coef;      // 0x34
  unsigned int reg_csc_c1_coef;      // 0x38
  unsigned int reg_csc_c2_coef;      // 0x3c
  unsigned int reg_csc_c3_coef;      // 0x40
  unsigned int reg_csc_c4_coef;      // 0x44
  unsigned int hrsz_coef_lut;    // 0x48
  unsigned int vrsz_coef_lut;    // 0x4c
  unsigned int reg_csc_offpara;  // 0x50
};

typedef struct {
   unsigned int coef;
   unsigned short int in_n;
   unsigned short int out_n;
} rsz_lut;

struct Ration2m
{
  unsigned int ratio;
  int n, m;
};

struct YuvCsc
{									// YUV(default)	or	YCbCr
	unsigned int csc0;				//	0x400			0x4A8
	unsigned int csc1;              //	0x59C   		0x662
	unsigned int csc2;              //	0x161   		0x191
	unsigned int csc3;              //	0x2DC   		0x341
	unsigned int csc4;              //	0x718   		0x811
	unsigned int chrom;             //	128				128
	unsigned int luma;              //	0				16
};

struct YuvStride
{
	unsigned int y;
	unsigned int u;
	unsigned int v;
	unsigned int out;
};




/* ipu driver ioctl command */
enum {
	IOCTL_IPU_OPEN = 1,
	IOCTL_IPU_CLOSE,
	IOCTL_IPU_INIT,
	IOCTL_IPU_DEINIT,
	IOCTL_IPU_START,
	IOCTL_IPU_STOP,
	IOCTL_IPU_RESIZE,
	IOCTL_IPU_SET_BUFF,
	IOCTL_IPU_SWAP_BUFF,
	IOCTL_IPU_FB_SIZE,
	IOCTL_IPU_SET_CTRL_REG,
	IOCTL_IPU_SET_FMT_REG,
	IOCTL_IPU_SET_CSC,
	IOCTL_IPU_SET_STRIDE,
	IOCTL_IPU_SET_OUTSIZE,
	IOCTL_IPU_PENDING_OUTEND,
	IOCTL_IPU_GET_ADDR,
	IOCTL_IPU_DUMP_REGS,
};

//////////////////////////////////////////////////
	/* set ipu output mode */
//#define IPU_OUTPUT_TO_LCD_FG0           0x00000001 /* hw not support */
#define IPU_OUTPUT_TO_LCD_FG1           0x00000002
#define IPU_OUTPUT_TO_LCD_FB0           0x00000004
#define IPU_OUTPUT_TO_LCD_FB1           0x00000008
#define IPU_OUTPUT_TO_FRAMEBUFFER       0x00000010 /* output to user defined buffer */
#define IPU_OUTPUT_MODE_MASK            0x000000FF
#define IPU_DST_USE_COLOR_KEY           0x00000100
#define IPU_DST_USE_ALPHA               0x00000200
#define IPU_OUTPUT_BLOCK_MODE           0x00000400

struct ipu_img_param_t
{
	unsigned int 		version;			/* sizeof(struct ipu_img_param_t) */
	int			ipu_cmd;			// IPU command
	unsigned int		output_mode;			// IPU output mode: fb0, fb1, fg1, alpha, colorkey ...
	unsigned int		ipu_ctrl;			// IPU Control Register
	unsigned int		ipu_d_fmt;			// IPU Data Format Register
	unsigned int		alpha;
	unsigned int		colorkey;
	unsigned int		in_width;
	unsigned int		in_height;
	unsigned int		in_bpp;
	unsigned int		out_x;
	unsigned int		out_y;
//	unsigned int		in_fmt;
//	unsigned int		out_fmt;
	unsigned int		out_width;
	unsigned int		out_height;
	unsigned char*		y_buf_v; /* Y buffer virtual address */
	unsigned char*		u_buf_v;
	unsigned char*		v_buf_v;
	unsigned int		y_buf_p; /* Y buffer physical address */
	unsigned int		u_buf_p;
	unsigned int		v_buf_p;
	unsigned char*		out_buf_v;
	unsigned int		out_buf_p;
	unsigned char*		y_t_addr;				// table address
	unsigned char*		u_t_addr;
	unsigned char*		v_t_addr;
	unsigned char*		out_t_addr;
	struct Ration2m*	ratio_table;
	struct YuvCsc*		csc;
	struct YuvStride	stride;
};


#ifdef __KERNEL__
/* 
 * ============================================================ 
 * IPU driver's native data
 */

#define IPU_LUT_LEN                                     (32)

struct ipu_driver_priv {
	struct ipu_img_param_t img;
	struct jz4750lcd_info *lcd_info;
	int inited;
	int rtable_len;

	unsigned int fb_w;
	unsigned int fb_h;
	unsigned int ipu_state;
//	struct ipu_img_param_t ipu_img_param;
	//struct ipu_img_param_t *ipu_img = &g_ipu_img_param;
	struct Ration2m ipu_ratio_table[IPU_LUT_LEN*IPU_LUT_LEN];
	//struct Ration2m *ipu_ratio_table = &g_ipu_ratio_table;
	rsz_lut h_lut[IPU_LUT_LEN];
	rsz_lut v_lut[IPU_LUT_LEN];

	spinlock_t	update_lock;
	wait_queue_head_t frame_wq;
	int frame_requested;
	int frame_done;
};

extern struct ipu_driver_priv *ipu_priv;


// Function prototype

int ipu_driver_open(struct ipu_driver_priv *ipu);
int ipu_driver_close(struct ipu_driver_priv *ipu);
int ipu_driver_init(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img);
int ipu_driver_deinit(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img);
int ipu_driver_start(struct ipu_driver_priv *ipu);
int ipu_driver_stop(struct ipu_driver_priv *ipu);
int ipu_driver_swapBuffer(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img);
int ipu_driver_setBuffer(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img);
int ipu_driver_resize(struct ipu_driver_priv *ipu, struct ipu_img_param_t *new_img);
int ipu_driver_register_irq(struct ipu_driver_priv *ipu);
int ipu_driver_dump_regs(struct ipu_driver_priv *ipu);


int ipu_driver_ioctl(struct ipu_driver_priv *ipu, void *uimg);
#endif	/* #ifdef __KERNEL__ */
#endif //endif _IPU_H
