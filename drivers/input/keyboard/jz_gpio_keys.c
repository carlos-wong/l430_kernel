/*
 *  Keyboard driver for the IngeniC JZ SoC
 *
 *  Copyright (c) 2009 Ignacio Garcia Perez <iggarpe@gmail.com>
 *
 *  Mod: <maddrone@gmail.com> 
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  TODO(IGP):
 *  - On power slider long press, use 'o' instead of 'b' to power off instead of reboot.
 *  - Button wake up (when idle and low power stuff works).
 *
 */

#include <linux/init.h>
#include <linux/input-polldev.h>
#include <linux/module.h>
#include <linux/sysrq.h>
#include <linux/sched.h>

#include <asm/jzsoc.h>
#include <linux/proc_fs.h>

#define SCAN_INTERVAL		(20)	/* (ms) */
#define POWER_INTERVAL		(2000)	/* (ms) */
#define POFF_INTERVAL		(5000)	/* (ms) */
#define POWER_COUNT		(POWER_INTERVAL / SCAN_INTERVAL)
#define POFF_COUNT		(POFF_INTERVAL / SCAN_INTERVAL)


//#define DEBUG_L009
#ifdef DEBUG_L009
#define lprint_dbg(f, arg...) printk("dbg::" __FILE__ ",LINE(%d): " f "\n", __LINE__, ## arg)
#define dprintk(f, arg...) printk(## arg)
#else
#define lprint_dbg(f, arg...) do {} while (0)
#define dprintk(f, arg...) do {} while (0)
#endif

#define MAX_WAIT_AD_TIME_OUT 10
#define SADCIN_BIGGER 0x900
#define SADCIN_LESSER 0x300

extern int scan_ad_key(unsigned int *key);
extern unsigned int real_key_pos_r;
extern unsigned int real_key_pos_select;
extern int board_detect(unsigned int ad_value);
extern void umido_set_disable_ad_read(int i);
void umido_dete_hardware_info(void);

unsigned int hardware_deted = 0;

/*
 * NOTE: maximum 32 GPIO, since we use bits in an unsigned long to store states
 */

static const struct {
	unsigned int gpio;
	unsigned int actlow;	/* Active low */
	unsigned int ncode;	/* Normal keycode */
	unsigned int scode;	/* Special keycode */
	unsigned int sysrq;	/* SYSRQ code */
	unsigned int wakeup;
} jz_button[] = {

#ifdef CONFIG_JZ4740_A320
	{ .gpio = 102,	.actlow = 1,	.ncode = KEY_UP,	.scode = KEY_VOLUMEUP,		.sysrq = 's'	}, /* D-pad up */
	{ .gpio = 123,	.actlow = 1, 	.ncode = KEY_DOWN,	.scode = KEY_VOLUMEDOWN,	.sysrq = 'u'	}, /* D-pad down */
	{ .gpio = 101,	.actlow = 1,	.ncode = KEY_LEFT,	.scode = KEY_BRIGHTNESSDOWN,	.sysrq = 'e'	}, /* D-pad left */
	{ .gpio = 114,	.actlow = 1,	.ncode = KEY_RIGHT,	.scode = KEY_BRIGHTNESSUP,	.sysrq = 'i'	}, /* D-pad right */
	{ .gpio = 96,	.actlow = 1,	.ncode = KEY_LEFTCTRL,							}, /* A button */
	{ .gpio = 97,	.actlow = 1,	.ncode = KEY_LEFTALT,							}, /* B button */
	{ .gpio = 115,	.actlow = 1,	.ncode = KEY_SPACE,							}, /* X button */
	{ .gpio = 98,	.actlow = 1,	.ncode = KEY_LEFTSHIFT,							}, /* Y button */
	{ .gpio = 110,	.actlow = 1,	.ncode = KEY_TAB,	.scode = KEY_EXIT				}, /* Left shoulder button */
	{ .gpio = 111,	.actlow = 1,	.ncode = KEY_BACKSPACE,							}, /* Right shoulder button */
	{ .gpio = 81,	.actlow = 1,	.ncode = KEY_ENTER,							}, /* START button(SYSRQ) */
	{ .gpio = 113,	.actlow = 1,	.ncode = KEY_ESC,	.scode = KEY_MENU,		.sysrq = 'b'	}, /* SELECT button */
	{ .gpio = 118,	.actlow = 1,	.ncode = KEY_PAUSE,							}, /* HOLD */

#define GPIO_POWER		GPIO_WAKEUP	/* Power slider */
#define GPIO_POWER_ACTLOW	1
#define SYSRQ_ALT		10	/* Alternate sysrq button (index in jz_button table) */
#endif

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

#ifdef CONFIG_JZ4740_PAVO
	{ .gpio = 96,	.actlow = 1,	.ncode = KEY_1 },
	{ .gpio = 97,	.actlow = 1,	.ncode = KEY_2 },
	{ .gpio = 98,	.actlow = 1,	.ncode = KEY_3 },
	{ .gpio = 99,	.actlow = 1,	.ncode = KEY_4 },
#endif

#ifdef CONFIG_JZ4750D_L009
	{ .gpio = (0)/*GPE0*/,	.actlow = 1,	.ncode = KEY_UP,	.scode = KEY_VOLUMEUP,		.sysrq = 's'	}, /* D-pad up */
	{ .gpio = (0)/*GPE1*/,	.actlow = 1, 	.ncode = KEY_DOWN,	.scode = KEY_VOLUMEDOWN,	.sysrq = 'u'	}, /* D-pad down */
	{ .gpio = (0)/*GPE2*/,	.actlow = 1,	.ncode = KEY_LEFT,	.scode = KEY_BRIGHTNESSDOWN,	.sysrq = 'e'	}, /* D-pad left */
	{ .gpio = (0)/*GPE3*/,	.actlow = 1,	.ncode = KEY_RIGHT,	.scode = KEY_BRIGHTNESSUP,	.sysrq = 'i'	}, /* D-pad right */
	{ .gpio = (0)/*GPC31*/,	.actlow = 1,	.ncode = KEY_LEFTCTRL,							}, /* A button */
	{ .gpio = (0)/*GPE11*/,	.actlow = 1,	.ncode = KEY_LEFTALT,							}, /* B button */
	{ .gpio = (0)/*GPD16*/,	.actlow = 1,	.ncode = KEY_SPACE,							}, /* X button */
	{ .gpio = (0)/**/,	.actlow = 1,	.ncode = KEY_LEFTSHIFT,							}, /* Y button */
	{ .gpio = (32*4+11)/*GPE11*/,	.actlow = 1,	.ncode = KEY_BACKSPACE,	.scode = KEY_EXIT				}, /* Right shoulder button */
	{ .gpio = (0)/*GPE10*/,	.actlow = 1,	.ncode = KEY_TAB,							}, /* Left shoulder button */
	{ .gpio = (0)/*GPD21*/,	.actlow = 1,	.ncode = KEY_ENTER,							}, /* START button(SYSRQ) */
	{ .gpio = (0)/*GPE8*/,	.actlow = 1,	.ncode = KEY_ESC,	.scode = KEY_MENU,		.sysrq = 'b'	}, /* SELECT button */
//	{ .gpio = (32*3+18)/*GPD18*/,	.actlow = 1,	.ncode = KEY_PAUSE,							}, /* HOLD */

#define GPIO_POWER		(125)	/* Power slider */
#define GPIO_POWER_ACTLOW	1
//#define SYSRQ_ALT		10	/* Alternate sysrq button (index in jz_button table) */
#endif
};



struct jz_kbd {
	unsigned int			keycode [2 * ARRAY_SIZE(jz_button)];
	struct input_polled_dev *	poll_dev;
	unsigned long			normal_state;	/* Normal key state */
	unsigned long			special_state;	/* Special key state */
	unsigned long			sysrq_state;	/* SYSRQ key state */
	unsigned long			actlow;		/* Active low mask */
	unsigned int			power_state;	/* Power slider state */
	unsigned int			power_count;	/* Power slider active count */
};

unsigned int umido_sdl_key_scan_alt = 1;
unsigned int umido_ad_key_conflict = 0;
static int proc_alt_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", umido_sdl_key_scan_alt);
}

static int proc_alt_write_proc(
			struct file *file, const char *buffer,
			unsigned long count, void *data)
{
	umido_sdl_key_scan_alt =  simple_strtoul(buffer, 0, 10);
	return count;
}

int umido_keypad_lock_valid(void)
{
  __gpio_as_input(HOLD_DETET);
  __gpio_enable_pull(HOLD_DETET);
  if(!__gpio_get_pin(HOLD_DETET))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
/*
 poweroff = SIGUSR2 
 reboot   = SIGINT
*/

static void send_sig_to_init( int sig )
{
	struct task_struct *p;

	for_each_process(p) 
	{
		if (is_global_init(p))
			force_sig(sig, p); 
	}
}


static void send_sig_all( int sig )
{
#if 0
	struct task_struct *p;

	for_each_process(p) 
	{
		if (p->mm && !is_global_init(p)) /* without important stuff */
			force_sig(sig, p); 
	}
#endif
}

static void prepare_for_restart(void)
{
#ifdef CONFIG_JZ4740_A320
	/* TODO : rewrite more smart code */
	__gpio_as_output(GPIO_LCD_BACKLIGHT);
	__gpio_clear_pin(GPIO_LCD_BACKLIGHT);
#endif

//maddrone
//	send_sig_all(SIGTERM);
//	send_sig_all(SIGKILL);
}

//maddrone add elan wireless pad
//extern unsigned int elan_key_val;
//extern unsigned long elan_state_flag;
extern unsigned int l009_gsensor_flag;
extern unsigned int l009_gsensor_read();

#define KEY_SCANE_0 (32*4+8)/*GPE8*/
#define KEY_SCANE_1 (32*4+7)/*GPE7*/
#define KEY_SCANE_2 (32*3+19)/*GPD19*/

#define KEY_SCANE_3 (32*5+10)/*GPF10*/
#define KEY_SCANE_4 (32*5+12)/*GPF12*/
#define KEY_SCANE_5 (32*5+11)/*GPF11*/
#define KEY_SCANE_6 (32*4+18)/*GPE18*/

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

extern int __init jz_mmc_init(void);
extern void __exit jz_mmc_exit(void);
unsigned int mmc_infrared_switch = 0;
EXPORT_SYMBOL(mmc_infrared_switch);

int proc_mmc_infrared_handle_read_proc(
			char *page, char **start, off_t off,
			int count, int *eof, void *data)
{
	return sprintf(page, "%lu\n", mmc_infrared_switch);
}

int proc_mmc_infrared_handle_write_proc(
			struct file *file, const char *buffer,
			unsigned long count, void *data)
{
        mmc_infrared_switch =  simple_strtoul(buffer, 0, 10);
        if(mmc_infrared_switch)
		{
                      if(MSC1_HOTPLUG_PIN)
                      {
                          __gpio_mask_irq(MSC1_HOTPLUG_PIN);
                      }
			__gpio_as_func0(HANDLEDATA);
			__gpio_as_input(HANDLEDATA);
			__gpio_disable_pull(HANDLEDATA);

			__gpio_as_func0(HANDLECLK);
			__gpio_as_output(HANDLECLK);
			__gpio_set_pin(HANDLECLK);

			__gpio_as_func0(HANDLEPS);
			__gpio_as_output(HANDLEPS);
			__gpio_clear_pin(HANDLEPS);
		}
		else
		{
			printk("mmc device enabled!\n");
			__gpio_as_msc1_1bit();
			__gpio_as_output(GPIO_SD1_VCC_EN_N);
			__gpio_as_input(GPIO_SD1_CD_N);
                        if(MSC1_HOTPLUG_PIN)
                        {
                          __gpio_unmask_irq(MSC1_HOTPLUG_PIN);
                          __gpio_as_irq_fall_edge(MSC1_HOTPLUG_PIN);
                        }
		}
	return count;
}

unsigned long infrared_handle_key_scan()
{
	int i;
	unsigned long s=0;

	__gpio_set_pin(HANDLEPS);
	udelay_jz(2);
	__gpio_clear_pin(HANDLEPS);
	udelay_jz(180);
	if(!__gpio_get_pin(HANDLEDATA))
		s |= KEY_Y_ID;
	for(i = 0;i<24;i++)
	{
		__gpio_clear_pin(HANDLECLK);
		udelay_jz(2);
		__gpio_set_pin(HANDLECLK);
		udelay_jz(180);
		if(!__gpio_get_pin(HANDLEDATA))
		{
			switch(i)
			{
				case 12:
					s |= KEY_Y_ID << 15;
					break;
				case 1:
					s |= KEY_X_ID;
					break;
				case 13:
					s |= KEY_X_ID << 15;
					break;
				case 2:
					s |= KEY_B_ID;
					break;
				case 14:
					s |= KEY_B_ID << 15;
					break;
				case 3:
					s |= KEY_A_ID;
					break;
				case 15:
					s |= KEY_A_ID << 15;
					break;
				case 4:
					s |= KEY_L_ID;
					break;
				case 16:
					s |= KEY_L_ID << 15;
					break;
				case 5:
					s |= KEY_R_ID;
					break;
				case 17:
					s |= KEY_R_ID << 15;
					break;
				case 6:
					s |= KEY_START_ID;
					break;
				case 18:
					s |= KEY_START_ID << 15;
					break;
				case 7:
					s |= KEY_SELECT_ID;
					break;
				case 19:
					s |= KEY_SELECT_ID << 15;
					break;
				case 8:
					s |= KEY_DOWN_ID;
					break;
				case 20:
					s |= KEY_DOWN_ID << 15;
					break;
				case 9:
					s |= KEY_RIGHT_ID;
					break;
				case 21:
					s |= KEY_RIGHT_ID << 15;
					break;
				case 10:
					s |= KEY_LEFT_ID;
					break;
				case 22:
					s |= KEY_LEFT_ID << 15;
					break;
				case 11:
					s |= KEY_UP_ID;
					break;
				case 23:
					s |= KEY_UP_ID << 15;
					break;
			}
		}
	}
	return s;
}


static void jz_kbd_poll (struct input_polled_dev *dev)
{
	if(umido_sdl_key_scan_alt && !umido_ad_key_conflict )
        {
          struct jz_kbd *kbd = dev->private;
          struct input_dev *input = kbd->poll_dev->input;
          unsigned int i, p, sync = 0;
          unsigned long s, m, x;

          /* TODO: lock */
          s = 0;	
          //maddrone: Add wireless gamepad here	
#if 0
          /* Scan raw key states */
          for (s = 0, m = 1, i = 0; i < ARRAY_SIZE(jz_button); i++, m <<= 1)
          {
            if(i == 4)
            {
              if (!__gpio_get_pin(jz_button[i].gpio)) s |= m;
            }
            else
            {
              if (__gpio_get_pin(jz_button[i].gpio)) s |= m;
            }
          }
#endif
          if(mmc_infrared_switch && !__gpio_get_pin(MSC1_HOTPLUG_PIN))
          {
            s = infrared_handle_key_scan();
          }
          unsigned int key = 0;
          //printk(" %s %d s is 0x%x\n",__FILE__,__LINE__,s);
          if(hardware_deted  == 1)
          {
            scan_ad_key(&s);
          }
          else
          {
            if(!umido_keypad_lock_valid())
            {
              umido_dete_hardware_info();
            }
          }
          //mdelay(2000);
         // if(s != 0)
         //   printk("s is 0x%x\n",s);

          if (!__gpio_get_pin(jz_button[8].gpio)) s |= real_key_pos_r;

          /*Scan raw key states*/
          for(i = 0; i < 4; i++)
          {
            set_gpio_l009(i);
            if(!__gpio_get_pin(KEY_SCANE_0))
            {
              udelay_jz(10);
              if(!__gpio_get_pin(KEY_SCANE_0))
              {
                //key presss 
                switch (i)
                {
                  case 0:
                    //X
                    s |= KEY_X_ID ;
                    break;
                  case 1:
                    //Y
                    s |= real_key_pos_select;
                    break;
                  case 2:
                    //LEFT
                    s |= KEY_L_ID;
                    break;
                }
              }
            }
            if(!__gpio_get_pin(KEY_SCANE_1))
            {
              udelay_jz(10);
              if(!__gpio_get_pin(KEY_SCANE_1))
              {
                //key presss 
                switch (i)
                {
                  case 0:
                    //SELECT
                    s |= KEY_Y_ID;
                    break;
                  case 1:
                    //A
                    s |= KEY_A_ID;
                    break;
                  case 2:
                    //DOWN
                    s |= KEY_B_ID;
                    break;
                  case 3:
                    //START
                    s |= KEY_START_ID;
                    break;

                }
              }
            }
            if(!__gpio_get_pin(KEY_SCANE_2))
            {
              udelay_jz(10);
              if(!__gpio_get_pin(KEY_SCANE_2))
              {
                //key presss 
                switch (i)
                {
                  case 0:
                    //R
                    s |= KEY_LEFT_ID;
                    break;
                  case 1:
                    //B
                    s |= KEY_DOWN_ID;
                    break;
                  case 2:
                    //RIGHT
                    s |= KEY_RIGHT_ID;
                    break;
                  case 3:
                    //UP
                    s |= KEY_UP_ID;
                    break;
                }
              }
            }

          }

#if 0	
          if(elan_state_flag > 0 && elan_key_val != 0)	
          {

            //printk("%s %d %d %d \n",__FILE__,__LINE__,elan_state_flag,elan_key_val);
            if(s == 0xfff) s=0; 
            //printk("elan_key_val = 0x%x, s=0x%x\n",elan_key_val,s);
            s |= ((~elan_key_val) & 0xfff);
            //printk("s = 0x%x\n",s);
          }
#endif
         // if(s != 0)
         //   printk("s is 0x%x\n",s);

#if 1
          //printk("\n\n\n--------- s = 0x%x\n",s);
          if(l009_gsensor_flag)
          {
            unsigned int val;
            val = l009_gsensor_read();	
            //s &= (~val);
            s |= val;
          }
          //printk("+++++++++ s = 0x%x\n",s);
#endif

          /* Invert active low buttons */
          //s ^= kbd->actlow;

          extern void draw_lock_picture(void);

          if(s != 0)
          {
            //if(!__gpio_get_pin(HOLD_DETET))
            if(umido_keypad_lock_valid())
            {
              draw_lock_picture();
              //mdelay(500);
              return;
            }
            else
            {
              ;
            }
          }
          /* Read power slider state */
#ifdef GPIO_POWER
#ifdef GPIO_POWER_ACTLOW
          p = !__gpio_get_pin(GPIO_POWER);
#else
          p = __gpio_get_pin(GPIO_POWER);
#endif
#else
          p = 0;
#endif

          if (p) {

#if 0
            /* If power slider and SYSRQ_ALT button are pressed... */
#ifdef SYSRQ_ALT
            if (s & (1 << SYSRQ_ALT)) {

              /* Calculate changed button state for system requests */
              x = s ^ kbd->sysrq_state;

              /* Generate system requests (only on keypress) */
              for (i = 0, m = 1; i < ARRAY_SIZE(jz_button); i++, m <<= 1) {
                if ((x & s & m) && jz_button[i].sysrq)
                {
                  /* added safety code for reboot */
                  if (jz_button[i].sysrq == 'b')
                  {
                    prepare_for_restart();
                    send_sig_to_init(SIGINT);
                  }
                  else 
                  {
                    handle_sysrq(jz_button[i].sysrq, NULL);
                  }
                }
              }

              kbd->power_count = 0;	/* Stop power slider pressed counter */
              kbd->sysrq_state = s;	/* Update current sysrq button state */
            }

            else
#endif
              /* If power slider is pressed... */
            {

              /* Calculate changed button state for special keycodes */
              x = s ^ kbd->special_state;

              /* Generate special keycodes for changed keys */
              for (i = 0, m = 1; i < ARRAY_SIZE(jz_button); i++, m <<= 1) {
                if ((x & m) && jz_button[i].scode) {
                  input_report_key(input, jz_button[i].scode, s & m);
                  sync++;
                }
              }

              /* If power state just pressed, start counter, otherwise increase it */
              if (!kbd->power_state) kbd->power_count = 1;
              else {
                if (kbd->power_count > 0) {
                  if (kbd->power_count == POWER_COUNT) {
                    input_report_key(input, KEY_POWER, 1);
                    input_report_key(input, KEY_POWER, 0);
                    sync++;
                  }
                  if (kbd->power_count == POFF_COUNT)
                  {
                    prepare_for_restart();
                    send_sig_to_init(SIGUSR2);
                  }
                  kbd->power_count++;
                }
              }

              if (s) kbd->power_count = 0;	/* If any button pressed, stop counter */
              kbd->special_state = s;		/* Update current special button state */
              kbd->power_state = p;		/* Update power slider state */
            }
#endif	
          }

          /* If power slider is NOT pressed... */
          else {

            /* Calculate changed button state for normal keycodes */
            x = s ^ kbd->normal_state;

            /* Generate normal keycodes for changed keys */
            for (i = 0, m = 1; i < ARRAY_SIZE(jz_button); i++, m <<= 1) {
              if ((x & m) && jz_button[i].ncode) {
                input_report_key(input, jz_button[i].ncode, s & m);
                sync++;
              }
            }

            kbd->power_count = 0;	/* Stop power slider pressed counter */
            kbd->normal_state = s;	/* Update current normal button state */
            kbd->power_state = p;	/* Update power slider state */
          }

          /* Synchronize input if any keycodes sent */
          if (sync) input_sync(input);
        }
	/* TODO: unlock */
}
int get_hardware_version(void)
{
  return hardware_deted; 
}
static struct jz_kbd jz_kbd;
void umido_dete_hardware_info(void)
{
  unsigned int add_return;
  unsigned int key_value;
  int try_get_version = 0;
#define TRY_GET_VERSION_LOOP 10
#define GET_VERSION_LOOP_VAILD 6
  int get_version_loop = 10;
  if(hardware_deted == 1)
    return;
  else
    hardware_deted = 1;

  umido_set_disable_ad_read(0);
  while(get_version_loop--){
    do{
      add_return =  scan_ad_key(&key_value);
    }while(add_return == -1);
    //printk("%s %d adc dete data is %d\n",__FILE__,__LINE__,add_return);
    if(board_detect(add_return) == 1)
      try_get_version++;
  }
  if(try_get_version >= GET_VERSION_LOOP_VAILD)
  {
    board_detect(2048);//any value which will make ad as new board       
  }
  else
  {
    board_detect(0);       
    umido_set_disable_ad_read(1);
  }
}
static int __init jz_kbd_init(void)
{
  struct input_polled_dev *poll_dev;
	struct input_dev *input_dev;
	int i, j, error;

	printk("jz-gpio-keys: scan interval %ums\n", SCAN_INTERVAL); 

	//Maddrone: add gpio init
	for(i=0; i<ARRAY_SIZE(jz_button); i++)
        {
          if(0 != jz_button[i].gpio)
          {
            __gpio_as_input(jz_button[i].gpio);
            __gpio_enable_pull(jz_button[i].gpio);
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

       //Nathan: add GPD18 init
        __gpio_as_input(HOLD_DETET);
        __gpio_enable_pull(HOLD_DETET);
        if(!umido_keypad_lock_valid())
        {
          umido_dete_hardware_info();
          
        }
#ifdef USE_UART_TO_DETE_BOARD
        unsigned int rxd_status = 0;
        __gpio_as_input((32*4 + 23));
        rxd_status = __gpio_get_pin((32*4 + 23));

        __gpio_as_uart1();
        //printk("the uart key convert!!!! rxd_status is %d\n\n",rxd_status);
        if(rxd_status == 0)
        {
          __lcd_close_backlight();
        }
        while(1);
#endif

	poll_dev = input_allocate_polled_device();
	if (!poll_dev) {
		error = -ENOMEM;
		goto fail;
	}

	jz_kbd.poll_dev = poll_dev;

	poll_dev->private = &jz_kbd;
	poll_dev->poll = jz_kbd_poll;
	poll_dev->poll_interval = SCAN_INTERVAL;

	input_dev = poll_dev->input;
	input_dev->name = "JZ GPIO keys";
	input_dev->phys = "jz-gpio-keys/input0";
	input_dev->id.bustype = BUS_HOST;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0100;

	/* Prepare active low mask and keycode array/bits */
	for (i = j = 0; i < ARRAY_SIZE(jz_button); i++) {
		if (jz_button[i].actlow) jz_kbd.actlow |= 1 << i;
		if (jz_button[i].ncode) {
			jz_kbd.keycode[j++] = jz_button[i].ncode;
			__set_bit(jz_button[i].ncode, input_dev->keybit);
		}
		if (jz_button[i].scode) {
			jz_kbd.keycode[j++] = jz_button[i].scode;
			__set_bit(jz_button[i].scode, input_dev->keybit);
		}
	}

	input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP) | BIT_MASK(EV_SYN);
	input_dev->keycode = jz_kbd.keycode;
	input_dev->keycodesize = sizeof(jz_kbd.keycode[0]);
	input_dev->keycodemax = j;

	error = input_register_polled_device(jz_kbd.poll_dev);
	if (error) goto fail;

//maddrone add
	struct proc_dir_entry *res,*res_handle;
	res = create_proc_entry("jz/alt", 0, NULL);
	if(res)
	{
		res->owner = THIS_MODULE;
		res->read_proc = proc_alt_read_proc;
		res->write_proc = proc_alt_write_proc;	
	}	

	res_handle = create_proc_entry("jz/infrared_handle", 0, NULL);
	if(res_handle)
	{
		res_handle->owner = THIS_MODULE;
		res_handle->read_proc = proc_mmc_infrared_handle_read_proc;
		res_handle->write_proc = proc_mmc_infrared_handle_write_proc;
	}
	return 0;

 fail:	input_free_polled_device(poll_dev);
	return error;
}

static void __exit jz_kbd_exit(void)
{
	input_unregister_polled_device(jz_kbd.poll_dev);
	input_free_polled_device(jz_kbd.poll_dev);
}

EXPORT_SYMBOL(umido_ad_key_conflict);
EXPORT_SYMBOL(umido_dete_hardware_info);
EXPORT_SYMBOL(umido_keypad_lock_valid);
EXPORT_SYMBOL(get_hardware_version);

module_init(jz_kbd_init);
module_exit(jz_kbd_exit);

MODULE_AUTHOR("Ignacio Garcia Perez <iggarpe@gmail.com>");
MODULE_DESCRIPTION("JZ GPIO keys driver");
MODULE_LICENSE("GPLv2");
