/*
 *  linux/include/asm-mips/mach-jz4750d/board-cetus.h
 *
 *  JZ4750D-based CETUS board ver 1.x definition.
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

#ifndef __ASM_JZ4750D_L009_H__
#define __ASM_JZ4750D_L009_H__

/*====================================================================== 
 * Frequencies of on-board oscillators
 */
#define JZ_EXTAL		24000000
#define JZ_EXTAL2		32768     /* RTC extal freq: 32.768 KHz */
//#define CFG_DIV                 1         /* hclk=pclk=mclk=CFG_EXTAL/CFG_DIV, just for FPGA board */


/*====================================================================== 
 * GPIO
 */

#define GPIO_SD0_VCC_EN_N	(32*2+24) /* NULL */
#define GPIO_SD0_CD_N		(32*4+4) /* NULL */
//#define GPIO_SD0_WP		    (32*4+2) /* NULL */

#define GPIO_SD1_VCC_EN_N	(32*2+24) /* GPC24 */
#define GPIO_SD1_CD_N		(32*4+4)//(32*4+4) /* GPE4 */

#define MSC_HOTPLUG_PIN	     GPIO_SD1_CD_N

#define ROCKER_AD_CTRL_X_Y         (32*4+2)
#define ROCKER_AD_CTRL_KEY_ROCK    (32*4+3)
#define HOLD_DETET (32*4+3) /*GPE3*/

#define GPIO_USB_DETE		(32 * 4 + 6) /* GPE6 */
#define GPIO_CHARG_STAT_N	(32 * 4 + 10) /* GPD15 */
//#define GPIO_CHARGE  	    (32 * 3 + 19)/* GPD14 */
//#define GPIO_DISP_OFF_N		(32*4+18) /* SDATO */
#define GPIO_LCD_VCC_EN_N	(32 * 4 + 1) /* SDATI */
//#define GPIO_LED_EN       	124 /* GPD28 */
#define GPIO_AMPEN          (32 * 4 + 5)  /* GPE5 */
#define GPIO_HP_DETE		(32 * 4 + 9) /* GPE9 */
#define GPIO_HP_OFF         (32 * 4 + 2)  /* GPE9 */

#define GPIO_DC_DETE_N		GPIO_USB_DETE
#define GPIO_UDC_HOTPLUG	GPIO_USB_DETE
/*====================================================================== 
 * LCD backlight
 */
#define GPIO_LCD_PWM   		(32*4+22) /* GPE22 PWM2 */ 
#define PWM_CHN 2    /* pwm channel */
#define LCD_PWM_FULL 101
/* 100 level: 0,1,...,100 */
#define __lcd_set_backlight_level(n)			\
do {							\
	u32 v = __cpm_get_extalclk() / 50000;		\
	__gpio_as_pwm(2);				\
	__tcu_disable_pwm_output(PWM_CHN);		\
	__tcu_stop_counter(PWM_CHN);			\
	__tcu_init_pwm_output_high(PWM_CHN);		\
	__tcu_set_pwm_output_shutdown_abrupt(PWM_CHN);	\
	__tcu_select_clk_div1(PWM_CHN);			\
	__tcu_mask_full_match_irq(PWM_CHN);		\
	__tcu_mask_half_match_irq(PWM_CHN);		\
	__tcu_set_count(PWM_CHN, 0);			\
	__tcu_set_full_data(PWM_CHN, v + 1);		\
	__tcu_set_half_data(PWM_CHN, v * n / 100);	\
	__tcu_enable_pwm_output(PWM_CHN);		\
	__tcu_select_extalclk(PWM_CHN);			\
	__tcu_start_counter(PWM_CHN);			\
} while (0)



#if 0
#define __lcd_set_backlight_level(n)	\
do {					\
	__gpio_as_output(GPIO_LCD_PWM);	\
	__gpio_set_pin(GPIO_LCD_PWM);	\
} while (0)
#endif

#define __lcd_backlight_on()	\
do {					\
	__gpio_as_output(GPIO_LCD_PWM);	\
	__gpio_set_pin(GPIO_LCD_PWM);	\
} while (0)

#define __lcd_close_backlight()		\
do {					\
	__gpio_as_output(GPIO_LCD_PWM);	\
	__gpio_clear_pin(GPIO_LCD_PWM);	\
} while (0)

/*====================================================================
 * GPIO INFRARED HANDLE KEYS
 */
#define GPB29 (32*1+29)
#define GPB26 (32*1+26)
#define GPB28 (32*1+28)
#define GPB27 (32*1+27)

#define HANDLEDATA GPB29
#define HANDLECLK GPB26
#define HANDLEPS GPB28
#define HANDLERST GPB27

/*====================================================================
 * GPIO KEYS and ADKEYS
 */
#define GPIO_HOME		(32*5+12) // SW4-GPF12 SSI_DR
#define GPIO_MENU		(32*2+31) // SW2-GPC31 boot_sel1
#define GPIO_BACK		(32*5+11) // SW3-GPF11 SSI_DT
#define GPIO_CALL		(32*5+10) // SW5-GPF10 SSI_CLK
#define GPIO_ENDCALL		(32*4+7)  // SW6-GPE7  CIM_D7
#define GPIO_SW10		(32*4+25) // SW10-GPE25 UART1_TXD
#define GPIO_ADKEY_INT		(32*4+11) // KEY_INT-GPE11  CIM_HSYNC
/*====================================================================== 
 * Analog input for VBAT is the battery voltage divided by CFG_PBAT_DIV.
 */
#define CFG_PBAT_DIV            1

/*
 * The GPIO interrupt pin is low voltage or fall edge acitve
 */
#define ACTIVE_LOW_HOME		1
#define ACTIVE_LOW_MENU		1
#define ACTIVE_LOW_BACK		1
#define ACTIVE_LOW_CALL		1
#define ACTIVE_LOW_ENDCALL	1
#define ACTIVE_LOW_SW10		1
#define ACTIVE_LOW_ADKEY	1
#define ACTIVE_LOW_MSC0_CD	1 /* work when GPIO_SD0_CD_N = 0 */
#define ACTIVE_LOW_MSC1_CD	1 /* work when GPIO_SD1_CD_N = 0 */
#define ACTIVE_WAKE_UP 		1


/*====================================================================
 * GPIO KEYS and ADKEYS
 */
#define GPIO_HOME		(32*5+12) // SW4-GPF12 SSI_DR
#define GPIO_MENU		(32*2+31) // SW2-GPC31 boot_sel1
#define GPIO_BACK		(32*5+11) // SW3-GPF11 SSI_DT
#define GPIO_CALL		(32*5+10) // SW5-GPF10 SSI_CLK
#define GPIO_ENDCALL		(32*4+7)  // SW6-GPE7  CIM_D7
#define GPIO_SW10		(32*4+25) // SW10-GPE25 UART1_TXD
#define GPIO_ADKEY_INT		(32*4+11) // KEY_INT-GPE11  CIM_HSYNC
/*====================================================================== 
 * Analog input for VBAT is the battery voltage divided by CFG_PBAT_DIV.
 */
#define CFG_PBAT_DIV            1

/*
 * The GPIO interrupt pin is low voltage or fall edge acitve
 */
#define ACTIVE_LOW_HOME		1
#define ACTIVE_LOW_MENU		1
#define ACTIVE_LOW_BACK		1
#define ACTIVE_LOW_CALL		1
#define ACTIVE_LOW_ENDCALL	1
#define ACTIVE_LOW_SW10		1
#define ACTIVE_LOW_ADKEY	1
#define ACTIVE_LOW_MSC0_CD	1 /* work when GPIO_SD0_CD_N = 0 */
#define ACTIVE_LOW_MSC1_CD	1 /* work when GPIO_SD1_CD_N = 0 */
#define ACTIVE_WAKE_UP 		1

/*====================================================================== 
 * MMC/SD
 */

#define MSC0_WP_PIN		GPIO_SD0_WP
#define MSC0_HOTPLUG_PIN	GPIO_SD0_CD_N
#define MSC0_HOTPLUG_IRQ	(IRQ_GPIO_0 + GPIO_SD0_CD_N)

#define MSC1_WP_PIN		GPIO_SD1_WP
#define MSC1_HOTPLUG_PIN	GPIO_SD1_CD_N
#define MSC1_HOTPLUG_IRQ	(IRQ_GPIO_0 + GPIO_SD1_CD_N)

#define __msc0_init_io()			\
do {						\
	__gpio_as_output(GPIO_SD0_VCC_EN_N);	\
	__gpio_as_input(GPIO_SD0_CD_N);		\
} while (0)

#define __msc0_enable_power()			\
do {						\
	__gpio_clear_pin(GPIO_SD0_VCC_EN_N);	\
} while (0)

#define __msc0_disable_power()			\
do {						\
	__gpio_set_pin(GPIO_SD0_VCC_EN_N);	\
} while (0)

/*
#define __msc0_card_detected(s)			\
({						\
	int detected = 1;			\
	if (__gpio_get_pin(GPIO_SD0_CD_N))	\
		detected = 0;			\
	detected;				\
})
*/

#if ACTIVE_LOW_MSC0_CD == 1 /* work when cd is low */
#define __msc0_card_detected(s)			\
({						\
	int detected = 1;			\
	if (__gpio_get_pin(GPIO_SD0_CD_N))	\
		detected = 0;			\
	detected;				\
})
#else
#define __msc0_card_detected(s)			\
({						\
	int detected = 0;			\
	if (__gpio_get_pin(GPIO_SD0_CD_N))	\
		detected = 1;			\
	detected;				\
})
#endif /*ACTIVE_LOW_MSC0_CD*/

#define __msc1_init_io()			\
do {						\
	__gpio_as_output(GPIO_SD1_VCC_EN_N);	\
	/*	__gpio_as_input(GPIO_SD1_CD_N);*/	\
} while (0)

#define __msc1_enable_power()			\
do {						\
	__gpio_clear_pin(GPIO_SD1_VCC_EN_N);	\
} while (0)

#define __msc1_disable_power()			\
do {						\
	__gpio_set_pin(GPIO_SD1_VCC_EN_N);	\
} while (0)

/*
#define __msc1_card_detected(s)			\
({						\
	int detected = 0;			\
	if (!(__gpio_get_pin(GPIO_SD1_CD_N)))	\
		detected = 1;			\
	detected;				\
})
*/
#if ACTIVE_LOW_MSC1_CD == 1 /* work when cd is low */
#define __msc1_card_detected(s)			\
({						\
	int detected = 1;			\
	if (__gpio_get_pin(GPIO_SD1_CD_N))	\
		detected = 0;			\
	detected;				\
})
#else
#define __msc1_card_detected(s)			\
({						\
	int detected = 0;			\
	if (__gpio_get_pin(GPIO_SD1_CD_N))	\
		detected = 1;			\
	detected;				\
})
#endif /*ACTIVE_LOW_MSC1_CD*/

#endif /* __ASM_JZ4750d_L009_H__ */
