/* ****************************************************************
 * @ func   : L009 Game Keypad Driver  
 * @ author : maddrone@gmail.com
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

static unsigned int key_value;
extern void umido_dete_hardware_info(void);
extern int umido_keypad_lock_valid(void);
extern int get_hardware_version(void);




#define KEY_SCANE_0 (32*4+8)/*GPE8*/
#define KEY_SCANE_1 (32*4+7)/*GPE7*/
#define KEY_SCANE_2 (32*3+19)/*GPD19*/

#define KEY_SCANE_3 (32*5+10)/*GPF10*/
#define KEY_SCANE_4 (32*5+12)/*GPF12*/
#define KEY_SCANE_5 (32*5+11)/*GPF11*/
#define KEY_SCANE_6 (32*4+18)/*GPE18*/


#define MAX_WAIT_AD_TIME_OUT 10
#define SADCIN_BIGGER 3549
#define SADCIN_LESSER 570
#define SADCIN_BIGGER_TRIGER 2304
#define SADCIN_LESSER_TRIGER 1850
#define SADCIN_OLD_BIGGER 3860
#define SADCIN_OLD_LESSER 161

//#define DEBUG_L009

#ifdef DEBUG_L009
#define lprint_dbg(f, arg...) printk("dbg::" __FILE__ ",LINE(%d): " f "\n", __LINE__, ## arg)
#else
#define lprint_dbg(f, arg...) do {} while (0)
#endif

static int disable_ad_read = 0;
static int umido_hardware_version = -1;

unsigned int jz_button[] = 
{
  (0),	// UP        0
  (0),   // DOWN      1
  (0),   // LEFT      2
  (0),   // RIGHT     3
  (0),  // A         4
  (0),  // B         5
  (0),  // X         6
  (0),  // Y         7
  (32*4+11),   // L         8
  (0),  // R         9
  (0),  // START     10
  (0),   // SELECT    11
  (125)           // POWER     13
};
extern unsigned int umido_ad_key_conflict;
static int key_open(struct inode *inode, struct file *filp)
{
	printk (KERN_ALERT "key_open\n");
        key_value = 0;
        umido_ad_key_conflict = 1;
	//umido_sdl_key_scan_alt = 0;
	return 0;
}

static int key_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "key_release\n");
        umido_ad_key_conflict = 0;
	//umido_sdl_key_scan_alt = 1;
	return 0;
}
inline static void set_gpio_l009(int mode)
{
  switch (mode)
  {
	  case 0:
		  __gpio_as_input(KEY_SCANE_4);
		  __gpio_as_input(KEY_SCANE_5);
		  __gpio_as_input(KEY_SCANE_6);
		  __gpio_as_output(KEY_SCANE_3);
		  __gpio_clear_pin(KEY_SCANE_3);
		  break;
	  case 1:
		  __gpio_as_input(KEY_SCANE_3);
		  __gpio_as_input(KEY_SCANE_5);
		  __gpio_as_input(KEY_SCANE_6);
		  __gpio_as_output(KEY_SCANE_4);
		  __gpio_clear_pin(KEY_SCANE_4);
		  break;
	  case 2:
		  __gpio_as_input(KEY_SCANE_4);
		  __gpio_as_input(KEY_SCANE_3);
		  __gpio_as_input(KEY_SCANE_6);
		  __gpio_as_output(KEY_SCANE_5);
		  __gpio_clear_pin(KEY_SCANE_5);
		  break;
	  case 3:
		  __gpio_as_input(KEY_SCANE_4);
		  __gpio_as_input(KEY_SCANE_5);
		  __gpio_as_input(KEY_SCANE_3);
		  __gpio_as_output(KEY_SCANE_6);
		  __gpio_clear_pin(KEY_SCANE_6);
		  break;
defalut:
		  __gpio_as_input(KEY_SCANE_3);
		  __gpio_as_input(KEY_SCANE_4);
		  __gpio_as_input(KEY_SCANE_5);
		  __gpio_as_input(KEY_SCANE_6);
		  break;

 }
}

//maddrone add elan wireless pad
extern unsigned int elan_key_val;
extern unsigned long elan_state_flag;

extern unsigned int l009_gsensor_flag;
extern unsigned int l009_gsensor_read();

extern unsigned int mmc_infrared_switch;
extern unsigned long infrared_handle_key_scan();

#define KEY_UP_ID 1<<0
#define KEY_DOWN_ID 1<<1
#define KEY_LEFT_ID 1<<2	
#define KEY_RIGHT_ID 1<<3	
#define KEY_A_ID  1<<4	
#define KEY_B_ID  1<<5	
#define KEY_X_ID  1<<6	
#define KEY_Y_ID  1<<7	
#define KEY_L_ID  1<<8	
#define KEY_R_ID  1<<9	
#define KEY_START_ID  1<<10	
#define KEY_SELECT_ID 1<<11
#undef DEBUG_SADC
#undef DEBUG
#ifdef DEBUG
#define dprintk(x...)	printk(x)
#else
#define dprintk(x...)
#endif

unsigned int real_key_pos_r = KEY_R_ID;
unsigned int real_key_pos_select = KEY_SELECT_ID;


#ifdef DEBUG_SADC
void dump_sadc_regs(void)
{
  //dprintk("------------------->>>>>>>>>>>>>>\n");
  //dprintk("REG_SADC_ENA	   is	0x%x\n",REG_SADC_ENA	 );
  //dprintk("REG_SADC_CFG	   is	0x%x\n",REG_SADC_CFG	 );
  //dprintk("REG_SADC_CTRL	   is	0x%x\n",REG_SADC_CTRL	 );
  //dprintk("REG_SADC_STATE	   is	0x%x\n",REG_SADC_STATE	 );
  //dprintk("REG_SADC_SAMETIME is   0x%x\n",REG_SADC_SAMETIME);
  //dprintk("REG_SADC_WAITTIME is   0x%x\n",REG_SADC_WAITTIME);
  //dprintk("REG_SADC_TSDAT	   is	0x%x\n",REG_SADC_TSDAT	 );
  //dprintk("REG_SADC_BATDAT   is	0x%x\n",REG_SADC_BATDAT  );
  unsigned int test_saddat = 0;
  test_saddat = REG_SADC_SADDAT;
  //dprintk("test_saddat   is	0x%x\n",test_saddat  );
//  dprintk("REG_SADC_ADCLK	   is	0x%x\n",REG_SADC_ADCLK	 );
}
#endif
void key_start_sadcin(void)
{
	REG_SADC_CTRL &= ~SADC_CTRL_SRDYM; /* enable interrupt */
	REG_SADC_ENA |= SADC_ENA_SADCINEN;
}
int get_umido_hardware_version(void)
{
  return umido_hardware_version;
}
int board_detect(unsigned int ad_value)
{
  if(ad_value > SADCIN_LESSER && ad_value < SADCIN_BIGGER)
  {
    printk("new board !!!! change key define\n"); 
    real_key_pos_r = KEY_SELECT_ID;
    real_key_pos_select = KEY_R_ID;
    umido_hardware_version = 1;
    return 1;
  }
  else
  {
    printk("old board !!!!\n"); 
    real_key_pos_r = KEY_R_ID;
    real_key_pos_select = KEY_SELECT_ID;
    umido_hardware_version = 0;
    return 0;
  }
}
void umido_set_disable_ad_read(int i)
{
  disable_ad_read = i;
}
int scan_ad_key(unsigned int *key)
{
  //printk("disable_ad_read is %d\n",disable_ad_read);
  if(disable_ad_read)
    return 0;

  __gpio_as_output(ROCKER_AD_CTRL_X_Y);
  __gpio_as_output(ROCKER_AD_CTRL_KEY_ROCK);
  __gpio_set_pin(ROCKER_AD_CTRL_KEY_ROCK); //try to read rocker ad key value
  int i = 0;
  int j = 0;
  j = 0;
  volatile unsigned int test_saddat = 0;
  dprintk("%s %d\n",__FILE__,__LINE__);

  //read x value
  test_saddat = REG_SADC_SADDAT;
  __gpio_clear_pin(ROCKER_AD_CTRL_X_Y);
  udelay(10);
  key_start_sadcin();
  while (!(REG_SADC_STATE & SADC_STATE_SRDY)) {
    i++;
    if(i > MAX_WAIT_AD_TIME_OUT)
      break;
    //mdelay(2000);
    //dprintk("y wait data ready!!\n");
    //dump_sadc_regs();
    //dprintk("y wait data ready!!\n");
  }
  if (i < MAX_WAIT_AD_TIME_OUT) {
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    dprintk("%s %d test_saddat is %d i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    //mdelay(10);
    test_saddat = REG_SADC_SADDAT;
    udelay(100);
    if(test_saddat > SADCIN_BIGGER_TRIGER && test_saddat < SADCIN_BIGGER){
      //printk("direction 1\n");
      *key |= KEY_DOWN_ID;
    }
    else if(test_saddat < SADCIN_LESSER_TRIGER && test_saddat > SADCIN_LESSER){
      //printk("direction 2\n");
      *key |= KEY_UP_ID;
    }
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
  } else {
    dprintk(" %s %d out of time\n",__FILE__,__LINE__);
    __gpio_as_input(ROCKER_AD_CTRL_KEY_ROCK);
    __gpio_enable_pull(ROCKER_AD_CTRL_KEY_ROCK);
    return -1;
  }
  i = 0;
  //read y value
  __gpio_set_pin(ROCKER_AD_CTRL_X_Y);
  udelay(10);
  key_start_sadcin();
  while (!(REG_SADC_STATE & SADC_STATE_SRDY)) {
    i++;
    if(i > MAX_WAIT_AD_TIME_OUT)
      break;
    //			mdelay(2000);
    //			dprintk("x wait data ready!!\n");
    //			//dump_sadc_regs();
    //			dprintk("x wait data ready!!\n");
  }
  if(i < MAX_WAIT_AD_TIME_OUT){
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,test_saddat,i);
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    dprintk("%s %d test_saddat is  %d i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
    //mdelay(50);
    //printk("%s %d key is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT);
    //mdelay(10);
    test_saddat = REG_SADC_SADDAT;
    udelay(100);
    if(test_saddat > SADCIN_BIGGER_TRIGER && test_saddat < SADCIN_BIGGER){
      dprintk("direction 3\n");
      *key |= KEY_LEFT_ID;
    }
    else if(test_saddat < SADCIN_LESSER_TRIGER && test_saddat > SADCIN_LESSER){
      dprintk("direction 4\n");
      *key |= KEY_RIGHT_ID;
    }
    dprintk("%s %d test_saddat is 0x%x i is %d\n",__FILE__,__LINE__,REG_SADC_SADDAT,i);
  }
  else{
    __gpio_as_input(ROCKER_AD_CTRL_KEY_ROCK);
    __gpio_enable_pull(ROCKER_AD_CTRL_KEY_ROCK);
    return -1;
  }
  dprintk("%s %d test_saddat is 0x%x \n",__FILE__,__LINE__,test_saddat);
  __gpio_as_input(ROCKER_AD_CTRL_KEY_ROCK);
  __gpio_enable_pull(ROCKER_AD_CTRL_KEY_ROCK);
  return test_saddat;
}
static ssize_t key_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
  int i;
#ifdef DEBUG_SADC
  dump_sadc_regs();
#endif
  key_value = 0;

  if(get_hardware_version())
  {
    scan_ad_key(&key_value);
  }
  else
  {
    if(!umido_keypad_lock_valid())
    {
      umido_dete_hardware_info();
    }
  }


#if 0
  for(i=0; i<ARRAY_SIZE(jz_button); i++)	
  {
    if(!(__gpio_get_pin(jz_button[i])))  key_value |= (1 << i);		
  }	
#endif
  if(mmc_infrared_switch && !__gpio_get_pin(MSC1_HOTPLUG_PIN))
  {

    key_value = infrared_handle_key_scan();
  }

  //mdelay(20);

  if(!(__gpio_get_pin(jz_button[8])))  key_value |=  real_key_pos_r;		

  for(i = 0; i < 4; i++)
  {
    set_gpio_l009(i);
    if(!__gpio_get_pin(KEY_SCANE_0))
    {
      udelay(10);
      if(!__gpio_get_pin(KEY_SCANE_0))
      {
        //key presss 
        switch (i)
        {
          case 0:
            //X
            key_value |= KEY_X_ID ;
            break;
          case 1:
            //SELECT
            key_value |= real_key_pos_select;
            break;
          case 2:
            //L
            key_value |= KEY_L_ID;
            break;
        }
      }
    }
    if(!__gpio_get_pin(KEY_SCANE_1))
    {
      udelay(10);
      if(!__gpio_get_pin(KEY_SCANE_1))
      {
        //key presss 
        switch (i)
        {
          case 0:
            //Y
            key_value |= KEY_Y_ID;
            break;
          case 1:
            //A
            key_value |= KEY_A_ID;
            break;
          case 2:
            //B
            key_value |= KEY_B_ID;
            break;
          case 3:
            //START
            key_value |= KEY_START_ID;
            break;
        }

      }
    }
    if(!__gpio_get_pin(KEY_SCANE_2))
    {
      udelay(10);
      if(!__gpio_get_pin(KEY_SCANE_2))
      {
        //key presss
        switch (i)
        {
          case 0:
            //LEFT
            key_value |= KEY_LEFT_ID;
            break;
          case 1:
            //DOWN
            key_value |= KEY_DOWN_ID;
            break;
          case 2:
            //RIGHT
            key_value |= KEY_RIGHT_ID;
            break;
          case 3:
            //UP
            key_value |= KEY_UP_ID;
            break;
        }
      }
    }

  }
  //if(key_value != 0) printk("key_value is  %x",key_value);
  if(l009_gsensor_flag)
  {
    key_value |= l009_gsensor_read();
    lprint_dbg("key_value is %x\n",key_value);
  }

  copy_to_user(buf, &key_value, sizeof(int));

  return sizeof(int);
}

#define 	KEYPAD_MAJOR 		252
#define 	KEYPAD_DEVICE_NAME	"keypad"

static struct file_operations keypad_fops = {
    owner:              THIS_MODULE,
    open:               key_open,
    read:               key_read,
    release:            key_release,
};

static struct miscdevice keypad_device = {
	.minor      = KEYPAD_MAJOR,
	.name       = KEYPAD_DEVICE_NAME,
	.fops       = &keypad_fops,
};


static int __init keypad_init(void)
{
	int i,ret;

	ret = misc_register(&keypad_device);
	if(ret<0){
		printk("kernel : keypad register failed!\n");
	}

	for(i=0; i<ARRAY_SIZE(jz_button); i++)
        {
          if( 0 != jz_button[i])
          {
            __gpio_as_input(jz_button[i]);
            __gpio_enable_pull(jz_button[i]);
          }
        }
        __gpio_as_func0(KEY_SCANE_0);
        __gpio_as_input(KEY_SCANE_0);
        __gpio_enable_pull(KEY_SCANE_0);
        __gpio_as_func0(KEY_SCANE_1);
        __gpio_as_input(KEY_SCANE_1);
        __gpio_enable_pull(KEY_SCANE_1);
        __gpio_as_func0(KEY_SCANE_2);
        __gpio_as_input(KEY_SCANE_2);
        __gpio_enable_pull(KEY_SCANE_2);

        __gpio_as_func0(KEY_SCANE_3);
        __gpio_as_output(KEY_SCANE_3);
        __gpio_enable_pull(KEY_SCANE_3);
        __gpio_as_func0(KEY_SCANE_4);
        __gpio_as_output(KEY_SCANE_4);
        __gpio_enable_pull(KEY_SCANE_4);
        __gpio_as_func0(KEY_SCANE_5);
        __gpio_as_output(KEY_SCANE_5);
        __gpio_enable_pull(KEY_SCANE_5);
        __gpio_as_func0(KEY_SCANE_6);
        __gpio_as_output(KEY_SCANE_6);
        __gpio_enable_pull(KEY_SCANE_6);

	return 0;
}

static void __exit keypad_cleanup(void)
{
	return ;
}


EXPORT_SYMBOL(scan_ad_key);
EXPORT_SYMBOL(real_key_pos_r);
EXPORT_SYMBOL(real_key_pos_select);
EXPORT_SYMBOL(board_detect);
EXPORT_SYMBOL(umido_set_disable_ad_read);

module_init(keypad_init);
module_exit(keypad_cleanup);

MODULE_AUTHOR("maddrone");
MODULE_DESCRIPTION("L009 keypad driver");
MODULE_LICENSE("GPL");


