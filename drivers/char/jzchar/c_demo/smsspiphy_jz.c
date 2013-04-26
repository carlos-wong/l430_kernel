#include <asm/jzsoc.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include "smsspiphy.h"
#include "smsdbg_prn.h"

//#define TEST

#define  SIANO_CMMB_SLEEP  (32 * 3 + 2)
#define  SIANO_CMMB_PWREN  (32 * 3 + 1)
#define  SIANO_CMMB_IRQ (32 * 3 + 31)

#define GPIO_CMMB_CLK  (32*3 + 18) 
#define GPIO_CMMB_DIN   (32*3 + 21)
#define GPIO_CMMB_DOUT  (32*3 + 20)
#define GPIO_CMMB_CS  (32*3 +19 ) 

unsigned char spi_rw_char(unsigned char wr_data);
unsigned char *dummy_device = 0x12345678;
#if 0
void SPIBus_Test_0(void)
{
	int i,j;
	unsigned char TEST_TX_Buf[256];	
	unsigned char TEST_RX_Buf[256];
//  1. send startup code

	// write 8 bytes down to SMS1180
while(1)
{
//	j++;
	memset( TEST_TX_Buf, 0, 8);

	TEST_TX_Buf[0] = 0x00;
	TEST_TX_Buf[1] = 0x00;
	TEST_TX_Buf[2] = 0xde;//11011110
	TEST_TX_Buf[3] = 0xc1;//11000001

	TEST_TX_Buf[4] = 0xa5;	//1010 0101
	TEST_TX_Buf[5] = 0x51;//01010 001
	TEST_TX_Buf[6] = 0xf1;//11110001
	TEST_TX_Buf[7] = 0xed;//11101101
	for (i = 0; i < 8; i++)
	{
		spi_rw_char(TEST_TX_Buf[i]);
	}
//	printk("j == %d\n", j);
//	mdelay(2000);
	}
}

void SPIBus_Test_1(void)
{
	int i;
	int j=0;
	unsigned char *TEST_TX_Buf=NULL;
	unsigned char *TEST_RX_Buf=NULL;
	TEST_TX_Buf = kmalloc(256, GFP_KERNEL);
	if (!TEST_TX_Buf){
		printk(KERN_ERR "%s kmalloc failed for TEST_TX_Buf", __func__);
		return;
	}

	TEST_RX_Buf = kmalloc(256, GFP_KERNEL);
	if (!TEST_RX_Buf){
		printk(KERN_ERR "%s kmalloc failed for TEST_RX_Buf", __func__);
		return;
	}
	//  1. send startup code
	// write 8 bytes down to SMS1180
	memset( TEST_TX_Buf, 0, 8);

	TEST_TX_Buf[0] = 0x00;
	TEST_TX_Buf[1] = 0x00;
	TEST_TX_Buf[2] = 0xde;//11011110
	TEST_TX_Buf[3] = 0xc1;//11000001

	TEST_TX_Buf[4] = 0xa5;	//1010 0101
	TEST_TX_Buf[5] = 0x51;//01010 001
	TEST_TX_Buf[6] = 0xf1;//11110001
	TEST_TX_Buf[7] = 0xed;//11101101
	for (i = 0; i < 8; i++)
	{
		spi_rw_char(TEST_TX_Buf[i]);
	}

	memset( TEST_TX_Buf, 0, 256 );
	// preamble
	
	TEST_TX_Buf[0] = 0xA5;//1010 0101
	TEST_TX_Buf[1] = 0x5A;//0101 1010
	TEST_TX_Buf[2] = 0xE7;//1110 0111
	TEST_TX_Buf[3] = 0x7E;//0111 1110
	
	// msg type
	TEST_TX_Buf[4] = 0x9C;//1001 1100. -->correct here
	TEST_TX_Buf[5] = 0x02;//0000 0010 OK
	
	//SRCID
	TEST_TX_Buf[6] = 0x23;//0010 0011 OK
	//DST ID
	TEST_TX_Buf[7] = 0x0B;//0000 1011 OK
	
	//Msg Len
	TEST_TX_Buf[8] = 0x08;//0X0C 0000 1000 OK
	
	TEST_TX_Buf[9] = 0x00; //OK
	
	// Msg Flag
	
	TEST_TX_Buf[10] = 0x00;
	
	TEST_TX_Buf[11] = 0x00;

	while (1)
	{
		j++;
		memset( TEST_RX_Buf, 0, 256 );
		for (i = 0; i < 256; i++)
		{
			TEST_RX_Buf[i]=spi_rw_char(TEST_TX_Buf[i]);
		}
		for (i = 0; i < 256; i++)
		{ 
			if ((TEST_RX_Buf[i] == 0xa5))
			{
				if ((TEST_RX_Buf[i+1] == 0x5a)
				&&(TEST_RX_Buf[i+2] == 0xe7)&& (TEST_RX_Buf[i+3] == 0x7e) 
				&&(TEST_RX_Buf[i+4] == 0x9d)&& (TEST_RX_Buf[i+5] == 0x02)
				&&(TEST_RX_Buf[i+6] == 0x0b)&& (TEST_RX_Buf[i+7] == 0x23))
				{
				//	printk("rx data right!\n");
					break;
				}
				else
				{
					printk("rx data error!\n");
					break;
				}
			}
		}
		printk("write time=%d\r\n",j);
	//	mdelay(1000);
	}
}
static void spi_loop_test()
{
	int i;
	int data;
	
	printk("spi loop test start!\n");

#if 0
	for(i=0; i<256; i++)
	{
	   data = spi_rw_char(i);
	  
	   if(data != i)
	   printk("spi test error,i = %d\n",i);
	}
#endif
	while(1)
		spi_rw_char(0x28);
	
	printk("spi loop test end!\n");

}
#endif

unsigned char spi_rw_char(unsigned char wr_data)
{	
	unsigned char rd_data;

	__ssi_flush_rxfifo();
	__ssi_flush_txfifo();

	while(__ssi_txfifo_full()) ;

	__ssi_transmit_data(wr_data);
	
	while(__ssi_rxfifo_empty());
	
	rd_data = (unsigned char)(__ssi_receive_data());

	return rd_data;
}

void smsspibus_xfer(void *context, unsigned char *txbuf,
		unsigned long txbuf_phy_addr, unsigned char *rxbuf,
		unsigned long rxbuf_phy_addr, int len)
{
	int i;
	//if (txbuf == NULL && rxbuf == NULL) return ;
	if ( rxbuf == NULL)
	{
		PERROR("rxbuf==NULL\n");
		return;
	}

	if (txbuf == NULL)
	{
		for(i=0; i<len; i++)
		{
			rxbuf[i] = spi_rw_char(0xff);
		}	
	}
	else
	{
		for(i=0; i<len; i++)
		{
			rxbuf[i] = spi_rw_char(txbuf[i]);
		}	
	}

	return;
}

void *smsspiphy_init(void *context, void (*smsspi_interruptHandler) (void *),
		void *intr_context)
{
	int ret;

	printk("smsspiphy_init..!\n");	

	//power off module
	__gpio_as_output(SIANO_CMMB_PWREN);
	__gpio_clear_pin(SIANO_CMMB_PWREN);
	mdelay(100);
	//__gpio_set_pin(SIANO_CMMB_PWREN);
	
	
	//init gpio
	
	__gpio_as_ssi();

	//config spi
	//__cpm_ssiclk_select_pllout(); // pllout: 360 MHz 
	__cpm_ssiclk_select_exclk(); // pllout: 360 MHz 
	__cpm_set_ssidiv(1);	// ssi source clk = 360 /(n+1) = 180 MHz 
	REG_SSI_GR = 11;	// clock = (180)/(2*(n+1)) MHz   // 7.5  MHz
	__cpm_start_ssi();

	__ssi_disable();
	__ssi_disable_tx_intr();
	__ssi_disable_rx_intr();
	__ssi_enable_receive();
	__ssi_flush_fifo();
	__ssi_clear_errors();

	__ssi_set_spi_clock_phase(0);     //PHA = 0
	__ssi_set_spi_clock_polarity(0);  //POL = 0
	__ssi_set_msb();
	__ssi_set_frame_length(8);
	__ssi_disable_loopback(); // just for test 
	//__ssi_enable_loopback(); // just for test 
	__ssi_select_ce();

	__ssi_set_tx_trigger(112); //n(128 - DMA_BURST_SIZE), total 128 entries 
	__ssi_set_rx_trigger(16);  //n(DMA_BURST_SIZE), total 128 entries 
	__ssi_set_msb();
	__ssi_spi_format();

	__ssi_enable();

	__gpio_as_irq_fall_edge(SIANO_CMMB_IRQ);		
	__gpio_unmask_irq(SIANO_CMMB_IRQ);

	//init irq
	if( ret = request_irq(IRQ_GPIO_0+SIANO_CMMB_IRQ, smsspi_interruptHandler, IRQF_DISABLED, "siano_cmmb_irq", NULL) )
	{
		printk("request siano cmmb irq error!\n");
		return -1;
	}
#ifdef TEST
//	SPIBus_Test_1();
//	test_clk();
//	spi_loop_test();
#endif

	return dummy_device;
}

void smsspiphy_deinit(void *context)
{
	//power off module
	
	free_irq(IRQ_GPIO_0+SIANO_CMMB_IRQ, NULL);
	__ssi_disable();
	__cpm_stop_ssi();
	__gpio_as_output(SIANO_CMMB_PWREN);
	__gpio_set_pin(SIANO_CMMB_PWREN);
	printk("smsspiphy_deinit!\n");
}

void smschipreset(void *context)
{
	//not yet
}

void WriteFWtoStellar(void *pSpiPhy, unsigned char *pFW, unsigned long Len)
{

}

void prepareForFWDnl(void *pSpiPhy)
{

}

void fwDnlComplete(void *context, int App)
{

}

