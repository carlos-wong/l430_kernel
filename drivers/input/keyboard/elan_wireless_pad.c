/* ****************************************************************
 * @ func   : elan wireless Keypad Driver  
 * @ author : huaixian.huang@gmail.com
 * ****************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_legacy.h>
#include <linux/kthread.h>
#include <linux/poll.h>
#include <asm/irq.h>
#include <asm/pgtable.h>
#include <asm/system.h>
#include <asm/uaccess.h>		
#include <asm/processor.h>
#include <asm/jzsoc.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>

#include "elan_wireless_pad.h"


unsigned int elan_key_val = 0;

//0: off   1:duima   2:readkey
unsigned long elan_state_flag = 0;
unsigned int elan_mcu_state_flag = 0;//no respon init
//dui ma flag
static unsigned int senddataelan = 0;

static unsigned char buffer_elanwireless[16];

#define L009_KEY_UP     0
#define L009_KEY_DOWN   1
#define L009_KEY_LEFT   2
#define L009_KEY_RIGHT  3
#define L009_KEY_A      4
#define L009_KEY_B      5
#define L009_KEY_X      6
#define L009_KEY_Y      7
#define L009_KEY_L      8
#define L009_KEY_R      9
#define L009_KEY_START    10
#define L009_KEY_SELECT   11
#define L009_KEY_HOLD     12
#define L009_KEY_POWER    13

#define PIN_IRQ_ELAN (32*3 + 12)
#define PIN_CLK_ELAN (32*3 + 11)
#define PIN_DATA_ELAN (32*3 + 10)
#define PIN_POWER_ELAN (32*3 + 13)
#define PIN_IRQ_ELAN_IRQ       (IRQ_GPIO_0 + PIN_IRQ_ELAN)

#define FIFO_W      (32*3 + 27)
#define PTK_W       (32*3 + 26)
#define SPI_SS_W    (32*4 + 18)
#define SPI_SO_W    (32*5 + 11)
#define SPI_SI_W    (32*5 + 12)
#define SPI_CLK_W   (32*5 + 10)
#define RESETN_W    (32*4 + 19)

void udelay_jz(int uses)
{
  int i  = 0;
  i =uses * 50 ; 
  while(i--)
  {
  __asm__(
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      "nop\n\t"
      );
  }
}

static void get_elanwireless_data(char *data)
{
  int i = 0;
  *data = 0x00;
  for(i=0; i<8; i++)
  {
    __gpio_clear_pin(PIN_CLK_ELAN);
    udelay_jz(8);
    __gpio_set_pin(PIN_CLK_ELAN);
    udelay_jz(4);
    *data |= ((__gpio_get_pin(PIN_DATA_ELAN) & 0x01) << i);
    udelay_jz(4);
  }
  __gpio_clear_pin(PIN_CLK_ELAN);
}

static void set_elanwireless_data(char data)
{
  int i = 0;
  for(i=0; i<8; i++)
  {
    __gpio_clear_pin(PIN_CLK_ELAN);
    if((data >> i)&0x01)
    //if((data << i)&0x80)
      __gpio_set_pin(PIN_DATA_ELAN);
    else
      __gpio_clear_pin(PIN_DATA_ELAN);
    udelay_jz(8);
    __gpio_set_pin(PIN_CLK_ELAN);
    udelay_jz(8);
  }
  __gpio_clear_pin(PIN_CLK_ELAN);

}
static int send_data = 0xaa;
static irqreturn_t elan_wireless_irq_fun(int irq, void *dev_id)
{
  int key_num = 0;
  static int key_interval = 0;
#define ELAN_DETECT_INTERVAL 1

  __gpio_ack_irq(PIN_IRQ_ELAN_IRQ);
  key_interval++;
  if(key_interval > ELAN_DETECT_INTERVAL)
  {
    key_interval = 0;
    int i = 0;
    for(i = 0; i < 16; i++)
    {
      get_elanwireless_data(&buffer_elanwireless[i]); 
    }
    //send data
#if 1
    if(senddataelan)
    {
      while(!__gpio_get_pin(PIN_IRQ_ELAN));
      __gpio_as_func0(PIN_DATA_ELAN);
      __gpio_as_output(PIN_DATA_ELAN);
      set_elanwireless_data(send_data);
      __gpio_as_func0(PIN_DATA_ELAN);
      __gpio_as_input(PIN_DATA_ELAN);
      senddataelan = 0;
      printk("attention !!!!!!!!!!!!!\n");
      elan_mcu_state_flag = 1;
    }
#endif
#if 1
#if 0
    for(i = 0; i < 16; i++)
    {
      printk("%x ",buffer_elanwireless[i]);
    }
    printk("\n");
#endif
    elan_key_val = 0;

    if(buffer_elanwireless[0] != 0 && buffer_elanwireless[0] != 0xff)
    {
      //key pad 2
      elan_key_val |= change_elan_pad_to_key((char *)&buffer_elanwireless[0]) << 15;
    }
    if(buffer_elanwireless[8] != 0 && buffer_elanwireless[8] != 0xff)
    {
      //key pad 1 
      elan_key_val |= change_elan_pad_to_key((char *)&buffer_elanwireless[8]);
    }
#endif
    __gpio_clear_pin(PIN_CLK_ELAN);
	//printk("elan_key_val = 0x%x\n",elan_key_val);
  }
  return IRQ_HANDLED;
}


//----------------------------------------------------------------------------
void elanwireless_gpioinit(void)
{
  __gpio_as_func0(PIN_DATA_ELAN);
  __gpio_as_input(PIN_DATA_ELAN);
  __gpio_enable_pull(PIN_DATA_ELAN);

  __gpio_as_func0(PIN_CLK_ELAN);
  __gpio_as_output(PIN_CLK_ELAN);
  __gpio_enable_pull(PIN_CLK_ELAN);
  __gpio_clear_pin(PIN_CLK_ELAN);
#if 1
  __gpio_as_func0(FIFO_W);
  __gpio_as_input(FIFO_W);
  __gpio_as_func0(PTK_W);
  __gpio_as_input(PTK_W);
  __gpio_as_func0(SPI_SS_W);
  __gpio_as_input(SPI_SS_W);
  __gpio_as_func0(SPI_SO_W);
  __gpio_as_input(SPI_SO_W);
  __gpio_as_func0(SPI_SI_W);
  __gpio_as_input(SPI_SI_W);
  __gpio_as_func0(SPI_CLK_W);
  __gpio_as_input(SPI_CLK_W);
  __gpio_as_func0(RESETN_W);
  __gpio_as_input(RESETN_W);
#endif
    __gpio_as_func0(PIN_POWER_ELAN);
    __gpio_enable_pull(PIN_POWER_ELAN);
    __gpio_as_output(PIN_POWER_ELAN);
    __gpio_clear_pin(PIN_POWER_ELAN);//turn on wireless moudle

    __gpio_as_func0(PIN_IRQ_ELAN);
    __gpio_as_input(PIN_IRQ_ELAN);
    __gpio_enable_pull(PIN_IRQ_ELAN);

    printk("%s %d\n\n\n\n\n",__FILE__,__LINE__);
  //gpio_intr_init(GITT_F_EDGE, PIN_IRQ_ELAN);
}
static unsigned char elan_key_pad[8]={
      L009_KEY_Y,
      L009_KEY_X, 
      L009_KEY_A ,
      L009_KEY_B, 
      L009_KEY_L,
      L009_KEY_R, 
      L009_KEY_L,
      L009_KEY_R 
    };

unsigned int change_elan_pad_to_key(char *buffer)
{
  unsigned int ret = 0;
  int i;
  
  if(buffer[0] != 0 && buffer[0] != 0xff)
  {
    if((buffer[1] & 0xff) == 0x00)//left
    {
      ret |= (1 << L009_KEY_LEFT);
    }
    else if((buffer[1] & 0xff) == 0xff)//right
    {
      ret |= (1 << L009_KEY_RIGHT);
    };
    if((buffer[2] & 0xff)== 0x00)//up
    {
      ret |= (1 << L009_KEY_UP);
    }
    else if((buffer[2] & 0xff)== 0xff)//down
    {
      ret |= (1 << L009_KEY_DOWN);
    };
    
    for(i = 0; i < 8;i++)
    {
      if(buffer[5] & (1 << i))
      {
        ret |=(1 << elan_key_pad[i]);
      }
    }
    if(buffer[6]&0x01)
    {
      ret |=(1 << L009_KEY_SELECT);
    }
    if(buffer[6]&0x20)
    {
      ret |=(1 << L009_KEY_START);
    }
  }
  return ret;
}

static int proc_elan_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", elan_state_flag);
}
static int proc_elan_mcu_status_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", elan_mcu_state_flag);
}
extern void elan_hw_on(void);
static int proc_elan_write_proc(
			struct file *file, const char *buffer,
			unsigned long count, void *data)
{
//	unsigned long old_val;
//	old_val = elan_state_flag;

        elan_state_flag =  simple_strtoul(buffer, 0, 10);
        //printk("%s %d elan_state_flag is %d\n",__FILE__,__LINE__,elan_state_flag);	
        if(elan_state_flag == 1)
        {
          __gpio_clear_pin(PIN_POWER_ELAN);//turn on wireless moudle
          send_data = 0xaa;
          senddataelan = 1;		//duima
        }
        else if(elan_state_flag == 0)
        {
          __gpio_clear_pin(PIN_POWER_ELAN);//turn on wireless moudle
          send_data = 0xbb;
          senddataelan = 1;		//shutdown mcu soft
        }
        else if(elan_state_flag == 3)
        {
          __gpio_as_func0(FIFO_W);
          __gpio_as_input(FIFO_W);
          __gpio_as_func0(PTK_W);
          __gpio_as_input(PTK_W);
          __gpio_as_func0(SPI_SS_W);
          __gpio_as_input(SPI_SS_W);
          __gpio_as_func0(SPI_SO_W);
          __gpio_as_input(SPI_SO_W);
          __gpio_as_func0(SPI_SI_W);
          __gpio_as_input(SPI_SI_W);
          __gpio_as_func0(SPI_CLK_W);
          __gpio_as_input(SPI_CLK_W);
          __gpio_as_func0(RESETN_W);
          __gpio_as_input(RESETN_W);
          send_data = 0xbc;
          senddataelan = 1;		//restart the mcu
          __gpio_set_pin(PIN_POWER_ELAN);//turn on wireless moudle
          senddataelan = 0;		//restart the mcu
#define DELAY_REBOOT 200
          int i = DELAY_REBOOT;
          //printk("%s %d\n",__FILE__,__LINE__);
          while(i--)udelay_jz(100);
          __gpio_clear_pin(PIN_POWER_ELAN);//turn on wireless moudle
          //printk("%s %d\n",__FILE__,__LINE__);
          i = DELAY_REBOOT;
          while(i--)udelay_jz(100);
          //printk("%s %d\n",__FILE__,__LINE__);
        }

#if 0
	if(old_val == 0 && elan_state_flag == 1)
	{
		//init gpio, power on and reset
    	__gpio_clear_pin(PIN_POWER_ELAN);//turn on wireless moudle
        send_data = 0xaa;
        senddataelan = 1;		//duima
	}
	else if(old_val == 0 && elan_state_flag == 2)
	{
    	__gpio_clear_pin(PIN_POWER_ELAN);//turn on wireless moudle
	}
	else if(old_val>0 && elan_state_flag)
        {
          //__gpio_set_pin(PIN_POWER_ELAN);//turn on wireless moudle
          __gpio_clear_pin(PIN_POWER_ELAN);//turn off wireless moudle
          send_data = 0xbb;
          senddataelan = 1;		//duima
	}
	else
	;	
#endif
	return count;
}
static int proc_elan_mcu_status_write_proc(
			struct file *file, const char *buffer,
            unsigned long count, void *data)
{;}


static int __init elan_pad_init()
{
#if 1
		struct proc_dir_entry *res;
		int retval;

 		elanwireless_gpioinit();
                printk("%s %d elan_pad_init \n",__FILE__,__LINE__);	
                __gpio_as_irq_fall_edge(PIN_IRQ_ELAN);
		retval = request_irq(PIN_IRQ_ELAN_IRQ, elan_wireless_irq_fun,
						IRQF_DISABLED, "test__key_tint", NULL);
		if(retval) 
		{
				printk("%s %d Could not get teset key irq %d\n\n\n\n\n\n",__FILE__,__LINE__, PIN_IRQ_ELAN_IRQ);
				return retval;
		}

		//create on/off entry
		res = create_proc_entry("jz/elan_pad", 0, NULL);
		if(res)
		{
				res->owner = THIS_MODULE;
				res->read_proc = proc_elan_read_proc;
				res->write_proc = proc_elan_write_proc;
		}
                res = create_proc_entry("jz/elan_mcu_status", 0, NULL);
                if(res)
                {
                  res->owner = THIS_MODULE;
                  res->read_proc = proc_elan_mcu_status_read_proc;
                  res->write_proc = proc_elan_mcu_status_write_proc;
                }
#endif
		return 0;
}

static void elan_pad_uninit()
{

}

module_init(elan_pad_init);
module_exit(elan_pad_uninit);

EXPORT_SYMBOL(elan_key_val);
EXPORT_SYMBOL(elan_state_flag);

MODULE_AUTHOR("maddrone");
MODULE_DESCRIPTION("L009 keypad driver");
MODULE_LICENSE("GPL");


