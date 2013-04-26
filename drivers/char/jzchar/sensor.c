/*
 * linux/drivers/char/jzchar/sensor.c
 *
 * Common G-Sensor Driver
 *
 * Copyright (C) 2006  Ingenic Semiconductor Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/jzsoc.h>
#include <linux/proc_fs.h>

#include "jzchars.h"

MODULE_AUTHOR("caijicheng@umidotech.com");
MODULE_DESCRIPTION("mxc6225xu Sensor Driver");
MODULE_LICENSE("GPL");

#define SENSOR_NAME	"mxc6225xu"
#define SENSOR_I2C_ADDR		0x15

char xout,yout;
#define READ_INTER 8

static void l009_gsensor_reset()
{
  i2c_close();
  __cpm_stop_i2c();	
  __cpm_start_i2c();	
  i2c_open();
} 

unsigned int l009_gsensor_read()
{
	static unsigned int val = 0;
	static unsigned long old_jiffies = 0;
	
	if((jiffies - old_jiffies) < READ_INTER) 
	return val;
	else
	old_jiffies = jiffies;
	
	val = 0;
        int ret = 0;
        
        ret = i2c_read(SENSOR_I2C_ADDR, (unsigned char *)(&xout), 0x00, 1); // read x
        if(ret < 0)
        {
          l009_gsensor_reset();
          return val;
        }
        ret = i2c_read(SENSOR_I2C_ADDR, (unsigned char *)(&yout), 0x01, 1); // read y
	if(ret < 0)
        {
          l009_gsensor_reset();
          return val;
        }

	if(yout > 12 )	    val |= 0x01;    //UP
	if(yout < -12 )     val |= 0x02;    //DOWN
	if(xout < -12 )     val |=  0x04;   //LEFT
	if(xout > 12 )      val |=  0x08;   //RIGHT
//printk("val = 0x%x\n",val);	
	return val;
}
EXPORT_SYMBOL(l009_gsensor_read);

unsigned int l009_gsensor_flag = 0;
EXPORT_SYMBOL(l009_gsensor_flag);

static int proc_gsensor_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", l009_gsensor_flag);
}

static int proc_gsensor_write_proc(
			struct file *file, const char *buffer,
			unsigned long count, void *data)
{
	l009_gsensor_flag =  simple_strtoul(buffer, 0, 10);

	if(l009_gsensor_flag)
	{
			__cpm_start_i2c();	

			i2c_close();
			i2c_open();
	}
	else
	{
			i2c_close();
			__cpm_stop_i2c();	
	}
	return count;
}

/*  AMP and Headphone */
#define HP_DETE_IRQ  (IRQ_GPIO_0 + GPIO_HP_DETE)
static int hp_in;
static unsigned int sound_flag = 0;  //default the amp is off
static struct timer_list hp_irq_timer;
#if 1
unsigned int medive_flag = 0;
EXPORT_SYMBOL(medive_flag);

static int proc_medive_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	printk("\nMedive printk: shutdown read shell end flag!\n");
	return sprintf(page, "%lu\n", medive_flag);
}


static int proc_medive_write_proc(
			struct file *file, const char *buffer,
			unsigned long count, void *data)
{
	printk("\nMedive printk: shutdown write shell end flag!\n");
	medive_flag =  simple_strtoul(buffer, 0, 10);
	return count;
}
#endif

static int proc_amp_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", sound_flag);
}
static int proc_hp_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", hp_in);
}


static int proc_hp_write_proc(
			struct file *file, const char *buffer,
			unsigned long count, void *data)
{

	return count;

}
int is_close_amp_hp = 1;

static int proc_amp_write_proc(
			struct file *file, const char *buffer,
			unsigned long count, void *data)
{
#if 1
		sound_flag =  simple_strtoul(buffer, 0, 10);

		if(sound_flag == 1)   //sound on
		{
			//printk("sound on\n");
			if(hp_in == 0)
			__gpio_set_pin(GPIO_AMPEN);
			__gpio_clear_pin(GPIO_HP_OFF);
		//	__gpio_set_pin(GPIO_HP_OFF);
			is_close_amp_hp = 0;
	
		}
		else if(sound_flag == 0)  //mute
		{
			//printk("mute\n");
			//if(hp_in == 0)
			__gpio_clear_pin(GPIO_AMPEN);
			
			__gpio_set_pin(GPIO_HP_OFF);
			__gpio_set_pin(GPIO_HP_OFF);
		//	__gpio_clear_pin(GPIO_HP_OFF);
			is_close_amp_hp = 1;

		}
		else
			;
#endif
}

static void
hp_ack_timer(unsigned long data)
{
extern unsigned int l009_globle_volume;


//printk("hp_pnp_irq---- is %d \n",__gpio_get_pin(GPIO_HP_DETE));

		//if(__gpio_get_pin(GPIO_HP_DETE))  //HP OF
#if 1
		if(__gpio_get_pin(GPIO_HP_DETE))  //HP OFF
        {
          //printk("hp_pnp_irq----, hp off\n");
          hp_in = 0;
          if(l009_globle_volume != 0)
          {
            __gpio_set_pin(GPIO_AMPEN);	
          }
          //__gpio_as_irq_fall_edge(GPIO_HP_DETE);
        }
        else  //HP ON
        {
          //printk("hp_pnp_irq----, hp on\n");
          hp_in = 1;
          if(l009_globle_volume != 0)
          {
            __gpio_clear_pin(GPIO_AMPEN);	
          }
          //__gpio_as_irq_rise_edge(GPIO_HP_DETE);
        }
#else
#endif
        hp_irq_timer.expires = jiffies + HZ*3;
        
        add_timer(&hp_irq_timer);
        /* clear interrupt pending status */
		//__gpio_ack_irq(GPIO_HP_DETE); 
		//__gpio_unmask_irq(GPIO_HP_DETE);

}

static irqreturn_t hp_pnp_irq(int irq, void *dev_id)
{
		/* mask interrupt */
		__gpio_mask_irq(GPIO_HP_DETE); 

		hp_irq_timer.expires = jiffies + HZ;
		del_timer(&hp_irq_timer);
		add_timer(&hp_irq_timer);
	
		return IRQ_HANDLED;
}

static int procWdtRead(
		char *page, char **start, off_t off,
		int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", 0);
}

static int procWdtWrite(
		struct file *file, const char *buffer,
		unsigned long count, void *data)
{
	int system_reset = simple_strtoul(buffer, 0, 10);
//	printk("DEBUG: system_reset is :%d\n",system_reset);

	if(system_reset  == 1)
	{
		__wdt_select_extalclk(); 
		__wdt_select_clk_div1();
		__wdt_set_count(0);
		__wdt_set_data(10);
		__wdt_start();
		while(1)
		{
			msleep(1);
			printk("wait for reset\n");
		}
	}

	return system_reset;
}

/*
 * Module init and exit
 */

static int __init sensor_init(void)
{
		int ret;
		struct proc_dir_entry *res, *res2,*res_hp,*res_medive,*res_restart;

		__gpio_as_i2c();
		udelay(200);
		__cpm_start_i2c();	

		i2c_close();
		i2c_open();

		res = create_proc_entry("jz/gsensor", 0, NULL);
		if(res)
		{
				res->owner = THIS_MODULE;
				res->read_proc = proc_gsensor_read_proc;
				res->write_proc = proc_gsensor_write_proc;	
		}	

		//maddrone add HeadPhone init here
		__gpio_as_output(GPIO_AMPEN);
		__gpio_as_output(GPIO_HP_OFF);

		__gpio_as_input(GPIO_HP_DETE);
		__gpio_enable_pull(GPIO_HP_DETE);
		udelay(1);

		if(__gpio_get_pin(GPIO_HP_DETE))  //HP OFF
		{
				hp_in = 0;
				__gpio_set_pin(GPIO_AMPEN);	

				//__gpio_as_irq_fall_edge(GPIO_HP_DETE);
			//	printk("++++++++++++ HP OUT +++++++++++++\n");
		}
		else  //HP ON
		{
				hp_in = 1;
				__gpio_clear_pin(GPIO_AMPEN);	

				//__gpio_as_irq_rise_edge(GPIO_HP_DETE);
			//	printk("++++++++++++ HP IN +++++++++++++\n");
		}

#if 0
		__gpio_as_input(GPIO_HP_DETE);
		__gpio_enable_pull(GPIO_HP_DETE);
		while(1)
		{
			printk("------ %d ------\n",__gpio_get_pin(GPIO_HP_DETE));
			mdelay(300);
		}
#endif
        //check HP status
        __gpio_as_input(GPIO_HP_DETE);
        __gpio_enable_pull(GPIO_HP_DETE);
        udelay(1);


        init_timer(&hp_irq_timer);
		hp_irq_timer.function = hp_ack_timer;
        hp_irq_timer.data = 0;
        hp_irq_timer.expires = jiffies + HZ;
        add_timer(&hp_irq_timer);


        //ret = request_irq(HP_DETE_IRQ, hp_pnp_irq,
        //					IRQF_DISABLED, "hp_pnp", NULL);
        //	if (ret) {
        //			printk("Could not get HP irq %d\n", HP_DETE_IRQ);
        //			return ret;
        //	}

	res2 = create_proc_entry("jz/amp", 0, NULL);
	if(res2)
	{
		res2->owner = THIS_MODULE;
		res2->read_proc = proc_amp_read_proc;
		res2->write_proc = proc_amp_write_proc;	
	}

	res_hp = create_proc_entry("jz/hp_l009", 0, NULL);
	if(res_hp)
	{
		res_hp->owner = THIS_MODULE;
		res_hp->read_proc = proc_hp_read_proc;
		res_hp->write_proc = proc_hp_write_proc;	
	}


	//Medive add for shutdown
#if 1
	res_medive = create_proc_entry("jz/medive_nl", 0, NULL);
	if(res_medive)
	{
		res_medive->owner = THIS_MODULE;
		res_medive->read_proc = proc_medive_read_proc;
		res_medive->write_proc = proc_medive_write_proc;	
	}
#endif
	//end
	res_restart = create_proc_entry("jz/restart", 0, NULL);
	if(res_restart)
	{
		res_restart->read_proc = procWdtRead;
		res_restart->write_proc = procWdtWrite;
		res_restart->data = NULL;
	}

	return 0;
}

static void __exit sensor_exit(void)
{
}

module_init(sensor_init);
module_exit(sensor_exit);
